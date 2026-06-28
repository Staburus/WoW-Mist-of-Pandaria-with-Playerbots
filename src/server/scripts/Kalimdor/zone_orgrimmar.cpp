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

#include "GameObject.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include "AreaTrigger.h"
//#include "AreaTriggerAI.h"
#include "LFGMgr.h"
//#include "SceneHelper.h"
#include "ObjectMgr.h"
#include "Player.h"

class spell_waters_of_farseeing_94687 : public SpellScriptLoader
{
public:
    spell_waters_of_farseeing_94687() : SpellScriptLoader("spell_waters_of_farseeing_94687") { }

    class spell_waters_of_farseeing_94687_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_waters_of_farseeing_94687_AuraScript);

        void OnApply(AuraEffect const* aurEff, AuraEffectHandleModes mode)
        {
            if (Unit* caster = GetCaster())
                if (Player* player = caster->ToPlayer())
                    if (player->GetQuestStatus(28805) == QUEST_STATUS_INCOMPLETE || player->GetQuestStatus(28826) == QUEST_STATUS_INCOMPLETE)
                        player->KilledMonsterCredit(50054);
        }

        void Register()
        {
            OnEffectApply += AuraEffectApplyFn(spell_waters_of_farseeing_94687_AuraScript::OnApply, EFFECT_0, SPELL_AURA_SCREEN_EFFECT, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_waters_of_farseeing_94687_AuraScript();
    }
};

// General Nazgrim 54870 — "All Aboard!" (quest 31853) boarding handler.
// The original mechanic used a phased Nazgrim at Bladefist Bay + a scene spell
// (121545) that is not implemented. This script gives Nazgrim in Orgrimmar a
// direct gossip option while the quest is active, then completes it and
// teleports the player to the Horde landing site in Jade Forest (map 870).
class npc_general_nazgrim_orgrimmar : public CreatureScript
{
    enum
    {
        QUEST_ALL_ABOARD    = 31853,
        NPC_ENTRY_NAZGRIM   = 54870,
    };

    static constexpr float JADE_FOREST_X = 3008.0f;
    static constexpr float JADE_FOREST_Y = -552.0f;
    static constexpr float JADE_FOREST_Z = 248.2f;
    static constexpr float JADE_FOREST_O = 3.84f;

public:
    npc_general_nazgrim_orgrimmar() : CreatureScript("npc_general_nazgrim_orgrimmar") { }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        if (player->GetQuestStatus(QUEST_ALL_ABOARD) == QUEST_STATUS_INCOMPLETE)
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "I'm ready to board Hellscream's Fist, General.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

        SendGossipMenuFor(player, player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
    {
        ClearGossipMenuFor(player);

        if (action == GOSSIP_ACTION_INFO_DEF + 1)
        {
            CloseGossipMenuFor(player);

            Quest const* quest = sObjectMgr->GetQuestTemplate(QUEST_ALL_ABOARD);
            if (quest && player->GetQuestStatus(QUEST_ALL_ABOARD) == QUEST_STATUS_INCOMPLETE)
            {
                player->CompleteQuest(QUEST_ALL_ABOARD);
                player->RewardQuest(quest, 0, creature, false);
            }

            player->TeleportTo(870, JADE_FOREST_X, JADE_FOREST_Y, JADE_FOREST_Z, JADE_FOREST_O);
        }

        return true;
    }
};

void AddSC_orgrimmar()
{
    new spell_waters_of_farseeing_94687();
    new npc_general_nazgrim_orgrimmar();
}
