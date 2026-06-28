#include "NewRpgAction.h"

#include <cmath>
#include <cstdint>

#include "NewRpgStrategy.h"
#include "Helper.h"
#include "ObjectDefines.h"
#include "ObjectGuid.h"
#include "PathGenerator.h"
#include "Player.h"
#include "PlayerbotAI.h"
#include "Playerbots.h"
#include "Random.h"
#include "RandomPlayerbotMgr.h"
#include "Timer.h"
#include "WorldPosition.h"
#include "World.h"

bool TellRpgStatusAction::Execute(Event event)
{
    /*Player* owner = event.getOwner();
    if (!owner)
        return false;
    std::string out = botAI->rpgInfo.ToString();
    bot->Whisper(out.c_str(), LANG_UNIVERSAL, owner);*/
    return true;
}

bool NewRpgStatusUpdateAction::Execute(Event event)
{
    NewRpgInfo& info = botAI->GetMutableRpgInfo();
    switch (info.status)
    {
        case NewRpgStatus::IDLE:
        {
            uint32 roll = urand(1, 100);
            if (roll <= 70)
            {
                info.status = NewRpgStatus::NEAR_RANDOM;
                info.lastNearRandom = getMSTime();
                return true;
            }
            info.status = NewRpgStatus::REST;
            info.lastRest = getMSTime();
            bot->SetStandState(UNIT_STAND_STATE_SIT);
            return true;
        }
        case NewRpgStatus::NEAR_RANDOM:
        {
            if (info.lastNearRandom + statusNearRandomDuration < getMSTime())
            {
                info.status = NewRpgStatus::IDLE;
                return true;
            }
            break;
        }
        case NewRpgStatus::REST:
        {
            if (info.lastRest + statusRestDuration < getMSTime())
            {
                info.status = NewRpgStatus::IDLE;
                return true;
            }
            break;
        }
        default:
            info.status = NewRpgStatus::IDLE;
            return true;
    }
    return false;
}

WorldPosition NewRpgStatusUpdateAction::SelectRandomGrindPos()
{
    /*const std::vector<WorldLocation>& locs = sRandomPlayerbotMgr->locsPerLevelCache[bot->GetLevel()];
    std::vector<WorldLocation> lo_prepared_locs, hi_prepared_locs;
    for (auto& loc : locs)
    {
        if (bot->GetMapId() != loc.GetMapId())
            continue;

        if (bot->GetMap()->GetZoneId(bot->GetPhaseMask(), loc.GetPositionX(), loc.GetPositionY(), loc.GetPositionZ()) !=
            bot->GetZoneId())
            continue;

        if (bot->GetExactDist(loc) < 500.0f)
        {
            hi_prepared_locs.push_back(loc);
        }

        if (bot->GetExactDist(loc) < 2500.0f)
        {
            lo_prepared_locs.push_back(loc);
        }
    }
    WorldPosition dest;
    if (urand(1, 100) <= 50 && !hi_prepared_locs.empty())
    {
        uint32 idx = urand(0, hi_prepared_locs.size() - 1);
        dest = hi_prepared_locs[idx];
    }
    else if (!lo_prepared_locs.empty())
    {
        uint32 idx = urand(0, lo_prepared_locs.size() - 1);
        dest = lo_prepared_locs[idx];
    }
    LOG_DEBUG("playerbots", "[New Rpg] Bot {} select random grind pos Map:{} X:{} Y:{} Z:{} ({}+{} available in {})",
              bot->GetName(), dest.GetMapId(), dest.GetPositionX(), dest.GetPositionY(), dest.GetPositionZ(),
              hi_prepared_locs.size(), lo_prepared_locs.size() - hi_prepared_locs.size(), locs.size());
    return dest;*/

    return WorldPosition();
}

WorldPosition NewRpgStatusUpdateAction::SelectRandomInnKeeperPos()
{
    /*const std::vector<WorldLocation>& locs = IsAlliance(bot->getRace())
                                                 ? sRandomPlayerbotMgr->allianceStarterPerLevelCache[bot->GetLevel()]
                                                 : sRandomPlayerbotMgr->hordeStarterPerLevelCache[bot->GetLevel()];
    std::vector<WorldLocation> prepared_locs;
    for (auto& loc : locs)
    {
        if (bot->GetMapId() != loc.GetMapId())
            continue;
        
        if (bot->GetMap()->GetZoneId(bot->GetPhaseMask(), loc.GetPositionX(), loc.GetPositionY(), loc.GetPositionZ()) !=
            bot->GetZoneId())
            continue;
            
        float range = bot->GetLevel() <= 5 ? 500.0f : 2500.0f;
        if (bot->GetExactDist(loc) < range)
        {
            prepared_locs.push_back(loc);
        }
    }
    WorldPosition dest;
    if (!prepared_locs.empty())
    {
        uint32 idx = urand(0, prepared_locs.size() - 1);
        dest = prepared_locs[idx];
    }
    LOG_DEBUG("playerbots", "[New Rpg] Bot {} select random inn keeper pos Map:{} X:{} Y:{} Z:{} ({} available in {})",
              bot->GetName(), dest.GetMapId(), dest.GetPositionX(), dest.GetPositionY(), dest.GetPositionZ(),
              prepared_locs.size(), locs.size());
    return dest;*/
    return WorldPosition();
}

bool NewRpgGoFarAwayPosAction::MoveFarTo(WorldPosition dest)
{
    /*float dis = bot->GetExactDist(dest);
    if (dis < pathFinderDis)
    {
        return MoveTo(dest.getMapId(), dest.GetPositionX(), dest.GetPositionY(), dest.GetPositionZ(), false, false,
                      false, true);
    }

    // performance optimization
    if (IsWaitingForLastMove(MovementPriority::MOVEMENT_NORMAL))
    {
        return false;
    }

    float minDelta = M_PI;
    const float x = bot->GetPositionX();
    const float y = bot->GetPositionY();
    const float z = bot->GetPositionZ();
    float rx, ry, rz;
    bool found = false;
    int attempt = 3;
    while (--attempt)
    {
        float angle = bot->GetAngle(&dest);
        float delta = urand(1, 100) <= 75 ? (rand_norm() - 0.5) * M_PI * 0.5 : (rand_norm() - 0.5) * M_PI * 2;
        angle += delta;
        float dis = rand_norm() * pathFinderDis;
        float dx = x + cos(angle) * dis;
        float dy = y + sin(angle) * dis;
        float dz = z + 0.5f;
        bot->UpdateAllowedPositionZ(dx, dy, dz);
        PathGenerator path(bot);
        path.CalculatePath(dx, dy, dz);
        PathType type = path.GetPathType();

        bool canReach = type == PATHFIND_NORMAL;

        if (canReach && fabs(delta) <= minDelta)
        {
            found = true;
            const G3D::Vector3& endPos = path.GetActualEndPosition();
            rx = endPos.x;
            ry = endPos.y;
            rz = endPos.z;
            minDelta = fabs(delta);
        }
    }
    if (found)
    {
        return MoveTo(bot->GetMapId(), rx, ry, rz, false, false, false, true);
    }
    // don't fallback to direct move
    // float angle = bot->GetAngle(&dest);
    // return MoveTo(bot->GetMapId(), x + cos(angle) * pathFinderDis, y + sin(angle) * pathFinderDis, z);*/
    return false;
}

bool NewRpgGoGrindAction::Execute(Event event)
{
    return false; /*MoveFarTo(botAI->rpgInfo.grindPos);*/
}

bool NewRpgGoInnKeeperAction::Execute(Event event)
{
    return false; /*MoveFarTo(botAI->rpgInfo.innKeeperPos);*/
}

bool NewRpgMoveRandomAction::Execute(Event event)
{
    float distance = rand_norm() * moveStep;
    Map* map = bot->GetMap();
    const float x = bot->GetPositionX();
    const float y = bot->GetPositionY();
    const float z = bot->GetPositionZ();
    int attempts = 5;
    while (--attempts)
    {
        float angle = (float)rand_norm() * 2 * static_cast<float>(M_PI);
        float dx = x + distance * cos(angle);
        float dy = y + distance * sin(angle);
        float dz = z;
        if (!map->CheckCollisionAndGetValidCoords(bot, bot->GetPositionX(), bot->GetPositionY(), bot->GetPositionZ(),
                                                  dx, dy, dz))
            continue;

        if (map->IsInWater(bot->GetPhaseMask(), dx, dy, dz))
            continue;

        bool moved = MoveTo(bot->GetMapId(), dx, dy, dz, false, false, false, true);
        if (moved)
            return true;
    }

    return false;
}

bool NewRpgMoveNpcAction::Execute(Event event)
{
    /*NewRpgInfo& info = botAI->rpgInfo;
    if (!info.npcPos)
    {
        GuidVector possibleTargets = AI_VALUE(GuidVector, "possible rpg targets");
        if (possibleTargets.empty())
            return false;
        int idx = urand(0, possibleTargets.size() - 1);
        ObjectGuid guid = possibleTargets[idx];
        Unit* unit = botAI->GetUnit(guid);
        if (unit)
        {
            info.npcPos = GuidPosition(unit);
            info.lastReachNpc = 0;
        }
        else
            return false;
    }

    if (bot->GetDistance(info.npcPos) <= INTERACTION_DISTANCE)
    {
        if (!info.lastReachNpc)
        {
            info.lastReachNpc = getMSTime();
            return true;
        }

        if (info.lastReachNpc && info.lastReachNpc + stayTime > getMSTime())
            return false;

        info.npcPos = GuidPosition();
        info.lastReachNpc = 0;
    }
    else
    {
        assert(info.npcPos);
        Unit* unit = botAI->GetUnit(info.npcPos);
        if (!unit)
            return false;
        float x = unit->GetPositionX();
        float y = unit->GetPositionY();
        float z = unit->GetPositionZ();
        float mapId = unit->GetMapId();
        float angle = 0.f;
        if (bot->IsWithinLOS(x, y, z))
        {
            if (!unit->isMoving())
                angle = unit->GetAngle(bot) + (M_PI * irand(-25, 25) / 100.0);  // Closest 45 degrees towards the target
            else
                angle = unit->GetOrientation() +
                        (M_PI * irand(-25, 25) / 100.0);  // 45 degrees infront of target (leading it's movement)
        }
        else
            angle = 2 * M_PI * rand_norm();  // A circle around the target.
        float rnd = rand_norm();
        x += cos(angle) * INTERACTION_DISTANCE * rnd;
        y += sin(angle) * INTERACTION_DISTANCE * rnd;
        // bool exact = true;
        if (!unit->GetMap()->CheckCollisionAndGetValidCoords(unit, unit->GetPositionX(), unit->GetPositionY(),
                                                             unit->GetPositionZ(), x, y, z))
        {
            x = unit->GetPositionX();
            y = unit->GetPositionY();
            z = unit->GetPositionZ();
            // exact = false;
        }
        return MoveTo(mapId, x, y, z, false, false, false, true);
    }*/
    return true;
}