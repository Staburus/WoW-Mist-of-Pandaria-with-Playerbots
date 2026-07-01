/*
* This file is part of the Pandaria 5.4.8 Project. See THANKS file for Copyright information
*
* This program is free software; you can redistribute it and/or modify it
* under the terms of the GNU General Public License as published by the
* Free Software Foundation; either version 2 of the License, or (at your
* option) any later version.
*
* This program is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
* more details.
*
* You should have received a copy of the GNU General Public License along
* with this program. If not, see <http://www.gnu.org/licenses/>.
*/

/* ScriptData
SDName: Stormwind City
SD%Complete: 50
SDComment: NPC 53527 prisoner escort (Harbor -> Stockade ambient event)
SDCategory: Stormwind City
EndScriptData */

/* ContentData
npc_sw_prisoner_escort  NPC 53527 — ambient prisoner walk from Harbor to Stockade
EndContentData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "ObjectAccessor.h"
#include "MotionMaster.h"

// -----------------------------------------------------------------------
// NPC 53527 — Stormwind Prisoner Escort
//
// Ambient event that fires on each creature spawn / respawn:
//   - Prisoner walks from the Harbor dock to The Stockade entrance.
//   - 4 Stormwind Guards (entry 68) are summoned in a flanking formation
//     and follow via MoveFollow.
//   - At the final waypoint the group fades (SetVisible false) then
//     DespawnOrUnsummon after 3 s.
//
// Implementation uses ScriptedAI + MovePoint / MovementInform rather than
// npc_escortAI because JustAppeared() is only called after a RESPAWN (i.e.
// after the creature has been dead), not on the initial server-load spawn.
// Reset() is called in both cases and is therefore the correct hook.
//
// Waypoint coordinates derived from live creature/waypoint_data DB queries.
// -----------------------------------------------------------------------

enum PrisonerEscortMisc
{
    NPC_SW_GUARD_ESCORT  = 68,

    SAY_ESCORT_BEGIN     = 0,    // creature_text GroupID 0
    SAY_ESCORT_END       = 1,    // creature_text GroupID 1

    FLAG_ESCORT_IMMUNE   = UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_PC,
};

struct GuardSlot { float dist; float angle; };
static const GuardSlot kFormation[4] =
{
    { 2.5f,  float(M_PI) * 0.45f },
    { 2.5f, -float(M_PI) * 0.45f },
    { 2.5f,  float(M_PI) * 0.80f },
    { 2.5f, -float(M_PI) * 0.80f },
};

struct EscortWP { float x, y, z; uint32 waitMs; };

// Harbor dock → city ramp → Stockade entrance.
// Coordinates sourced from live NPC patrol data (creature/waypoint_data queries).
// Root cause of prior stall: WP1-WP2 went west into open water (no MMAP tiles).
// Fix: go south along the east dock to y≈1200 where the docks connect, then west
// to the ramp at x≈-8573 (NPC 29019 guid 98267 patrol path, verified 53-point route).
static const EscortWP kPath[] =
{
    // East dock – walk south to where harbor road connects east↔west
    { -8394.0f, 1292.0f,   5.30f, 3000 }, //  0  spawn / greet (3 s pause)
    { -8394.0f, 1200.0f,   5.70f,    0 }, //  1  south end of east dock (guard 29712 pos)
    // Harbor south road – walk west to ramp entrance
    { -8573.0f, 1195.0f,   5.57f,    0 }, //  2  west dock / ramp foot (guard 29712 pos)
    // Harbor ramp Z 5→59 – coords from NPC 29019 patrol (pts 11→1 reversed)
    { -8571.0f, 1178.0f,  18.43f,    0 }, //  3  lower platform
    { -8570.0f, 1162.0f,  18.49f,    0 }, //  4  platform
    { -8576.0f, 1122.0f,  17.94f,    0 }, //  5  platform
    { -8580.0f, 1116.0f,  17.94f,    0 }, //  6  platform
    { -8589.0f, 1095.0f,  30.72f,    0 }, //  7  climbing
    { -8592.0f, 1087.0f,  33.52f,    0 }, //  8  climbing
    { -8577.0f, 1072.0f,  37.64f,    0 }, //  9  climbing
    { -8569.0f, 1061.0f,  44.79f,    0 }, // 10  climbing
    { -8551.0f, 1039.0f,  59.47f,    0 }, // 11  harbor gate (Z≈59)
    // Harbor gate → city road Z 59→96 (guard spawn positions)
    { -8485.0f,  979.0f,  72.82f,    0 }, // 12  mid-ramp (guard 68 spawn)
    { -8368.0f,  991.0f,  96.29f,    0 }, // 13  city road entry (guard 721 spawn)
    // City streets south to Stockade entrance
    { -8361.0f,  963.0f,  96.35f,    0 }, // 14  city road (guard 1412 spawn)
    { -8387.0f,  685.0f,  95.36f, 1000 }, // 15  Stockade / Jailer (FINAL)
};
static const uint32 kPathSize = uint32(sizeof(kPath) / sizeof(kPath[0]));

class npc_sw_prisoner_escort : public CreatureScript
{
public:
    npc_sw_prisoner_escort() : CreatureScript("npc_sw_prisoner_escort") {}

    struct npc_sw_prisoner_escortAI : public ScriptedAI
    {
        npc_sw_prisoner_escortAI(Creature* creature)
            : ScriptedAI(creature), _wpIndex(0), _delayTimer(0),
              _guardsSummoned(false), _arrived(false) {}

        uint32 _wpIndex;
        uint32 _delayTimer;
        bool   _guardsSummoned;
        bool   _arrived;
        std::vector<ObjectGuid> _guardGuids;

        // Reset() is called on initial map load (via AIM_Initialize) AND
        // on every respawn — covers both cases that JustAppeared() misses.
        void Reset() override
        {
            _wpIndex       = 0;
            _delayTimer    = 3000; // brief pause before first step
            _guardsSummoned = false;
            _arrived       = false;
            _guardGuids.clear();

            me->SetWalk(true);
            me->SetFlag(UNIT_FIELD_FLAGS, FLAG_ESCORT_IMMUNE);
            me->GetMotionMaster()->MoveIdle();
        }

        void UpdateAI(uint32 diff) override
        {
            if (_arrived)
                return;

            if (_delayTimer)
            {
                if (_delayTimer <= diff)
                {
                    _delayTimer = 0;
                    MoveToWP();
                }
                else
                    _delayTimer -= diff;
            }
        }

        void MoveToWP()
        {
            if (_wpIndex >= kPathSize)
            {
                _arrived = true;
                Talk(SAY_ESCORT_END);
                FadeAndDespawn();
                return;
            }

            if (_wpIndex == 0 && !_guardsSummoned)
            {
                SummonGuards();
                Talk(SAY_ESCORT_BEGIN);
            }

            const EscortWP& wp = kPath[_wpIndex];
            me->GetMotionMaster()->MovePoint(_wpIndex, wp.x, wp.y, wp.z);
        }

        void MovementInform(uint32 type, uint32 id) override
        {
            if (type != POINT_MOTION_TYPE || id != _wpIndex)
                return;

            uint32 wait = kPath[_wpIndex].waitMs;
            ++_wpIndex;

            if (wait > 0)
                _delayTimer = wait;
            else
                MoveToWP();
        }

        void JustDied(Unit* /*killer*/) override
        {
            DespawnGuards(500);
        }

    private:

        void SummonGuards()
        {
            if (_guardsSummoned)
                return;
            _guardsSummoned = true;

            float o = me->GetOrientation();
            for (uint8 i = 0; i < 4; ++i)
            {
                float angle = o + kFormation[i].angle;
                float dx = kFormation[i].dist * std::cos(angle);
                float dy = kFormation[i].dist * std::sin(angle);

                Creature* guard = me->SummonCreature(
                    NPC_SW_GUARD_ESCORT,
                    me->GetPositionX() + dx,
                    me->GetPositionY() + dy,
                    me->GetPositionZ(),
                    o,
                    TEMPSUMMON_TIMED_DESPAWN, 600000u);

                if (!guard)
                    continue;

                guard->SetFlag(UNIT_FIELD_FLAGS, FLAG_ESCORT_IMMUNE);
                guard->GetMotionMaster()->MoveFollow(me, kFormation[i].dist, kFormation[i].angle);
                _guardGuids.push_back(guard->GetGUID());
            }
        }

        void FadeAndDespawn()
        {
            me->SetVisible(false);
            me->DespawnOrUnsummon(3000);
            for (ObjectGuid const& guid : _guardGuids)
                if (Creature* g = me->GetMap()->GetCreature(guid))
                {
                    g->SetVisible(false);
                    g->DespawnOrUnsummon(3000);
                }
            _guardGuids.clear();
        }

        void DespawnGuards(uint32 delayMs)
        {
            for (ObjectGuid const& guid : _guardGuids)
                if (Creature* g = me->GetMap()->GetCreature(guid))
                    g->DespawnOrUnsummon(delayMs);
            _guardGuids.clear();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_sw_prisoner_escortAI(creature);
    }
};

void AddSC_stormwind_city()
{
    new npc_sw_prisoner_escort();
}
