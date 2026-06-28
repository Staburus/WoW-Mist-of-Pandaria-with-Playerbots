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
    return true;
}

bool NewRpgStatusUpdateAction::Execute(Event event)
{
    NewRpgInfo& info = botAI->GetRpgInfo();
    switch (info.status)
    {
        case NewRpgStatus::IDLE:
        {
            uint32 roll = urand(1, 100);
            // IDLE -> NEAR_NPC
            if (roll <= 30)
            {
                info.lastNearNpc = getMSTime();
                GuidVector possibleTargets = AI_VALUE(GuidVector, "possible rpg targets");
                if (!possibleTargets.empty())
                {
                    info.status = NewRpgStatus::NEAR_NPC;
                    return true;
                }
            }
            // IDLE -> GO_INNKEEPER
            else if (roll <= 45)
            {
                WorldPosition pos = SelectRandomInnKeeperPos();
                if (pos != WorldPosition() && bot->GetExactDist(pos) > 50.0f)
                {
                    info.lastGoInnKeeper = getMSTime();
                    info.status = NewRpgStatus::GO_INNKEEPER;
                    info.innKeeperPos = pos;
                    return true;
                }
            }
            // IDLE -> GO_GRIND
            else if (roll <= 90)
            {
                WorldPosition pos = SelectRandomGrindPos();
                if (pos != WorldPosition())
                {
                    info.lastGoGrind = getMSTime();
                    info.status = NewRpgStatus::GO_GRIND;
                    info.grindPos = pos;
                    return true;
                }
            }
            // IDLE -> REST
            info.status = NewRpgStatus::REST;
            info.lastRest = getMSTime();
            bot->SetStandState(UNIT_STAND_STATE_SIT);
            return true;
        }
        case NewRpgStatus::GO_GRIND:
        {
            WorldPosition& originalPos = info.grindPos;
            assert(info.grindPos != WorldPosition());
            // GO_GRIND -> NEAR_RANDOM
            if (bot->GetExactDist(originalPos) < 10.0f)
            {
                info.status = NewRpgStatus::NEAR_RANDOM;
                info.lastNearRandom = getMSTime();
                info.grindPos = WorldPosition();
                return true;
            }
            break;
        }
        case NewRpgStatus::GO_INNKEEPER:
        {
            WorldPosition& originalPos = info.innKeeperPos;
            assert(info.innKeeperPos != WorldPosition());
            // GO_INNKEEPER -> NEAR_NPC
            if (bot->GetExactDist(originalPos) < 10.0f)
            {
                info.lastNearNpc = getMSTime();
                info.status = NewRpgStatus::NEAR_NPC;
                info.innKeeperPos = WorldPosition();
                return true;
            }
            break;
        }
        case NewRpgStatus::NEAR_RANDOM:
        {
            // NEAR_RANDOM -> IDLE
            if (info.lastNearRandom + statusNearRandomDuration < getMSTime())
            {
                info.status = NewRpgStatus::IDLE;
                return true;
            }
            break;
        }
        case NewRpgStatus::NEAR_NPC:
        {
            if (info.lastNearNpc + statusNearNpcDuration < getMSTime())
            {
                info.status = NewRpgStatus::IDLE;
                return true;
            }
            break;
        }
        case NewRpgStatus::REST:
        {
            // REST -> IDLE
            if (info.lastRest + statusRestDuration < getMSTime())
            {
                info.status = NewRpgStatus::IDLE;
                return true;
            }
            break;
        }
        default:
            break;
    }
    return false;
}

WorldPosition NewRpgStatusUpdateAction::SelectRandomGrindPos()
{
    const auto* spot = sRandomPlayerbotMgr->GetFarmZoneForPlayer(bot);
    if (!spot)
        return WorldPosition();

    TC_LOG_DEBUG("playerbots", "[New Rpg] Bot %s select random grind pos Map:%u X:%f Y:%f Z:%f",
              bot->GetName().c_str(), spot->map_id, spot->x, spot->y, spot->z);
    return WorldPosition(spot->map_id, spot->x, spot->y, spot->z);
}

WorldPosition NewRpgStatusUpdateAction::SelectRandomInnKeeperPos()
{
    const auto* c = sRandomPlayerbotMgr->GetCityForPlayer(bot);
    if (!c)
        return WorldPosition();

    TC_LOG_DEBUG("playerbots", "[New Rpg] Bot %s select random inn keeper pos Map:%u X:%f Y:%f Z:%f",
              bot->GetName().c_str(), c->map_id, c->x, c->y, c->z);
    return WorldPosition(c->map_id, c->x, c->y, c->z);
}

bool NewRpgGoFarAwayPosAction::MoveFarTo(WorldPosition dest)
{
    float dis = bot->GetExactDist(dest);
    if (dis < pathFinderDis)
    {
        return MoveTo(dest.getMapId(), dest.GetPositionX(), dest.GetPositionY(), dest.GetPositionZ(), false, false,
                      false, true);
    }

    if (IsWaitingForLastMove(MovementPriority::MOVEMENT_NORMAL))
        return false;

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
        float stepDis = rand_norm() * pathFinderDis;
        float dx = x + cos(angle) * stepDis;
        float dy = y + sin(angle) * stepDis;
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
        return MoveTo(bot->GetMapId(), rx, ry, rz, false, false, false, true);

    return false;
}

bool NewRpgGoGrindAction::Execute(Event event)
{
    return MoveFarTo(botAI->GetRpgInfo().grindPos);
}

bool NewRpgGoInnKeeperAction::Execute(Event event)
{
    return MoveFarTo(botAI->GetRpgInfo().innKeeperPos);
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
    NewRpgInfo& info = botAI->GetRpgInfo();
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
        uint32 mapId = unit->GetMapId();
        float angle = 0.f;
        if (bot->IsWithinLOS(x, y, z))
        {
            if (!unit->isMoving())
                angle = unit->GetAngle(bot) + (M_PI * irand(-25, 25) / 100.0);
            else
                angle = unit->GetOrientation() + (M_PI * irand(-25, 25) / 100.0);
        }
        else
            angle = 2 * M_PI * rand_norm();
        float rnd = rand_norm();
        x += cos(angle) * INTERACTION_DISTANCE * rnd;
        y += sin(angle) * INTERACTION_DISTANCE * rnd;
        if (!unit->GetMap()->CheckCollisionAndGetValidCoords(unit, unit->GetPositionX(), unit->GetPositionY(),
                                                             unit->GetPositionZ(), x, y, z))
        {
            x = unit->GetPositionX();
            y = unit->GetPositionY();
            z = unit->GetPositionZ();
        }
        return MoveTo(mapId, x, y, z, false, false, false, true);
    }
    return true;
}
