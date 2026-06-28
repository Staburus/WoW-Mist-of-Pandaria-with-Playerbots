#include "BattleGroundJoinAction.h"

#include "ArenaTeam.h"
#include "Battleground.h"
#include "BattlegroundMgr.h"
#include "BattlegroundQueue.h"
#include "DBCStores.h"
#include "GroupMgr.h"
#include "PlayerbotAI.h"
#include "PlayerbotSpec.h"
#include "Playerbots.h"
#include "RandomPlayerbotMgr.h"

bool BGJoinAction::Execute(Event /*event*/)
{
    uint32 queueType = AI_VALUE(uint32, "bg type");
    if (!queueType)
    {
        if (bgList.empty())
            return false;

        queueType = bgList[urand(0, bgList.size() - 1)];

        BattlegroundQueueTypeId queueTypeId = BattlegroundQueueTypeId(queueType);
        uint8 arenaType = BattlegroundMgr::BGArenaType(queueTypeId);
        if (arenaType)
        {
            if (!GatherArenaGroup(arenaType))
                return false;
        }
        else if (queueTypeId == BATTLEGORUND_QUEUE_RATED_BG)
        {
            if (!GatherRatedBgGroup())
                return false;
        }

        botAI->GetAiObjectContext()->GetValue<uint32>("bg type")->Set(queueType);
    }

    return JoinQueue(queueType);
}

bool BGJoinAction::GatherArenaGroup(uint8 arenaType)
{
    uint32 teamSize = arenaType;

    if (bot->GetGroup() && bot->GetGroup()->GetMembersCount() >= teamSize)
        return true;

    if (bot->GetGroup())
        bot->GetGroup()->Disband(true);

    Group* group = new Group();
    if (!group->Create(bot))
    {
        delete group;
        return false;
    }
    sGroupMgr->AddGroup(group);

    uint32 added = 1;
    for (auto& pair : sRandomPlayerbotMgr->GetAllBots())
    {
        if (added >= teamSize)
            break;

        Player* member = pair.second;
        if (!member || !member->IsInWorld() || member == bot)
            continue;

        if (!sRandomPlayerbotMgr->IsRandomBot(member))
            continue;

        if (member->GetGroup() || member->InBattleground() || member->InBattlegroundQueue())
            continue;

        if (member->IsInCombat() || member->isDead())
            continue;

        if (member->GetLevel() < 70)
            continue;

        PlayerbotAI* memberAI = GET_PLAYERBOT_AI(member);
        if (!memberAI || memberAI->HasActivePlayerMaster())
            continue;

        if (!group->AddMember(member))
            continue;

        memberAI->Reset();
        added++;
    }

    if (group->GetMembersCount() < teamSize)
    {
        group->Disband();
        return false;
    }

    TC_LOG_INFO("playerbots", "Bot {} <{}>: Formed {}v{} arena group",
        bot->GetGUID().ToString().c_str(), bot->GetName().c_str(), arenaType, arenaType);

    return true;
}

bool BGJoinAction::GatherRatedBgGroup()
{
    uint32 teamSize = 10;

    if (bot->GetGroup() && bot->GetGroup()->GetMembersCount() >= teamSize)
        return true;

    if (bot->GetGroup())
        bot->GetGroup()->Disband(true);

    Group* group = new Group();
    if (!group->Create(bot))
    {
        delete group;
        return false;
    }
    sGroupMgr->AddGroup(group);

    uint32 healers = 0, tanks = 0, dps = 1;
    uint32 maxHealers = 3, maxTanks = 1;

    PlayerbotAI* leaderAI = GET_PLAYERBOT_AI(bot);
    if (leaderAI && PlayerBotSpec::IsHeal(bot))
        healers = 1;
    else if (leaderAI && PlayerBotSpec::IsTank(bot))
        tanks = 1;

    uint32 added = 1;
    for (auto& pair : sRandomPlayerbotMgr->GetAllBots())
    {
        if (added >= teamSize)
            break;

        Player* member = pair.second;
        if (!member || !member->IsInWorld() || member == bot)
            continue;

        if (!sRandomPlayerbotMgr->IsRandomBot(member))
            continue;

        if (member->GetGroup() || member->InBattleground() || member->InBattlegroundQueue())
            continue;

        if (member->IsInCombat() || member->isDead())
            continue;

        if (member->GetLevel() < 90)
            continue;

        if (member->GetTeamId() != bot->GetTeamId())
            continue;

        PlayerbotAI* memberAI = GET_PLAYERBOT_AI(member);
        if (!memberAI || memberAI->HasActivePlayerMaster())
            continue;

        bool isHealer = PlayerBotSpec::IsHeal(member);
        bool isTank = PlayerBotSpec::IsTank(member);

        if (isHealer && healers >= maxHealers)
            continue;
        if (isTank && tanks >= maxTanks)
            continue;

        if (!group->AddMember(member))
            continue;

        if (isHealer) healers++;
        else if (isTank) tanks++;
        else dps++;

        memberAI->Reset();
        added++;
    }

    if (group->GetMembersCount() < teamSize)
    {
        group->Disband();
        return false;
    }

    TC_LOG_INFO("playerbots", "Bot {} <{}>: Formed 10v10 rated BG group ({} tanks, {} healers, {} dps)",
        bot->GetGUID().ToString().c_str(), bot->GetName().c_str(), tanks, healers, dps);

    return true;
}

bool BGJoinAction::canJoinBg(BattlegroundQueueTypeId queueTypeId, BattlegroundBracketId bracketId)
{
    BattlegroundTypeId bgTypeId = BattlegroundMgr::BGTemplateId(queueTypeId);

    if (bot->InBattlegroundQueueForBattlegroundQueueType(queueTypeId))
        return false;

    if (!bot->GetBGAccessByLevel(bgTypeId))
        return false;

    Battleground* bg = sBattlegroundMgr->GetBattlegroundTemplate(bgTypeId);
    if (!bg)
        return false;

    PvPDifficultyEntry const* pvpDiff = GetBattlegroundBracketByLevel(bg->GetMapId(), bot->GetLevel());
    if (!pvpDiff)
        return false;

    if (pvpDiff->GetBracketId() != bracketId)
        return false;

    return true;
}

bool BGJoinAction::shouldJoinBg(BattlegroundQueueTypeId queueTypeId, BattlegroundBracketId bracketId)
{
    BattlegroundTypeId bgTypeId = BattlegroundMgr::BGTemplateId(queueTypeId);
    Battleground* bg = sBattlegroundMgr->GetBattlegroundTemplate(bgTypeId);
    if (!bg)
        return false;

    if (bot->GetGroup() && !bot->GetGroup()->IsLeader(bot->GetGUID()))
        return false;

    TeamId teamId = bot->GetTeamId();
    uint32 TeamSize = bg->GetMaxPlayersPerTeam();
    uint32 BracketSize = TeamSize * 2;

    uint8 arenaType = BattlegroundMgr::BGArenaType(queueTypeId);
    if (arenaType)
    {
        BracketSize = (uint32)(arenaType * 2);
        TeamSize = (uint32)arenaType;

        // Check skirmish arena
        uint32 skirmishBotCount = sRandomPlayerbotMgr->BattlegroundData[queueTypeId][bracketId].skirmishArenaBotCount;
        uint32 skirmishPlayerCount = sRandomPlayerbotMgr->BattlegroundData[queueTypeId][bracketId].skirmishArenaPlayerCount;
        uint32 skirmishInstanceCount = sRandomPlayerbotMgr->BattlegroundData[queueTypeId][bracketId].skirmishArenaInstanceCount;
        uint32 activeSkirmishQueue = sRandomPlayerbotMgr->BattlegroundData[queueTypeId][bracketId].activeSkirmishArenaQueue;

        uint32 maxRequired = BracketSize * (activeSkirmishQueue + skirmishInstanceCount);
        if (maxRequired != 0)
            maxRequired += TeamSize;

        if ((skirmishBotCount + skirmishPlayerCount) < maxRequired)
            return true;

        // Check rated arena
        uint32 ratedBotCount = sRandomPlayerbotMgr->BattlegroundData[queueTypeId][bracketId].ratedArenaBotCount;
        uint32 ratedPlayerCount = sRandomPlayerbotMgr->BattlegroundData[queueTypeId][bracketId].ratedArenaPlayerCount;
        uint32 ratedInstanceCount = sRandomPlayerbotMgr->BattlegroundData[queueTypeId][bracketId].ratedArenaInstanceCount;
        uint32 activeRatedQueue = sRandomPlayerbotMgr->BattlegroundData[queueTypeId][bracketId].activeRatedArenaQueue;

        uint32 maxRatedRequired = BracketSize * (activeRatedQueue + ratedInstanceCount);
        if (maxRatedRequired != 0)
            maxRatedRequired += TeamSize;

        if ((ratedBotCount + ratedPlayerCount) < maxRatedRequired)
            return true;

        return false;
    }

    // Rated BG (10v10)
    if (queueTypeId == BATTLEGORUND_QUEUE_RATED_BG)
    {
        uint32 ratedBgTeamSize = 10;
        BattlegroundInfo& info = sRandomPlayerbotMgr->BattlegroundData[queueTypeId][bracketId];

        if (!info.activeRatedBgQueue)
            return false;

        uint32 allianceTotal = info.ratedBgAllianceBotCount + info.ratedBgAlliancePlayerCount;
        uint32 hordeTotal = info.ratedBgHordeBotCount + info.ratedBgHordePlayerCount;
        uint32 needed = ratedBgTeamSize * (info.activeRatedBgQueue + info.ratedBgInstanceCount);

        if (teamId == TEAM_ALLIANCE)
            return allianceTotal < needed;
        else
            return hordeTotal < needed;
    }

    // Normal BG
    uint32 allianceBotCount = sRandomPlayerbotMgr->BattlegroundData[queueTypeId][bracketId].bgAllianceBotCount;
    uint32 alliancePlayerCount = sRandomPlayerbotMgr->BattlegroundData[queueTypeId][bracketId].bgAlliancePlayerCount;
    uint32 hordeBotCount = sRandomPlayerbotMgr->BattlegroundData[queueTypeId][bracketId].bgHordeBotCount;
    uint32 hordePlayerCount = sRandomPlayerbotMgr->BattlegroundData[queueTypeId][bracketId].bgHordePlayerCount;
    uint32 activeBgQueue = sRandomPlayerbotMgr->BattlegroundData[queueTypeId][bracketId].activeBgQueue;
    uint32 bgInstanceCount = sRandomPlayerbotMgr->BattlegroundData[queueTypeId][bracketId].bgInstanceCount;

    if (teamId == TEAM_ALLIANCE)
    {
        if ((allianceBotCount + alliancePlayerCount) < TeamSize * (activeBgQueue + bgInstanceCount))
            return true;
    }
    else
    {
        if ((hordeBotCount + hordePlayerCount) < TeamSize * (activeBgQueue + bgInstanceCount))
            return true;
    }

    return false;
}

bool BGJoinAction::isUseful()
{
    if (!sPlayerbotAIConfig->randomBotJoinBG)
        return false;

    if (bot->InBattleground() || bot->InBattlegroundQueue())
        return false;

    if (bot->GetLevel() < 10)
        return false;

    if (GET_PLAYERBOT_AI(bot) && GET_PLAYERBOT_AI(bot)->IsRealPlayer())
        return false;

    if (bot->GetGroup() && !bot->GetGroup()->IsLeader(bot->GetGUID()))
        return false;

    if (bot->IsInCombat() || bot->isDead())
        return false;

    if (bot->HasAura(26013)) // Deserter
        return false;

    if (!bot->HasFreeBattlegroundQueueId())
        return false;

    if (!sRandomPlayerbotMgr->IsRandomBot(bot))
        return false;

    Map* map = bot->GetMap();
    if (map && map->Instanceable())
        return false;

    bgList.clear();

    for (int bracket = BG_BRACKET_ID_FIRST; bracket < MAX_BATTLEGROUND_BRACKETS; ++bracket)
    {
        for (int queueType = BATTLEGROUND_QUEUE_AV; queueType < MAX_BATTLEGROUND_QUEUE_TYPES; ++queueType)
        {
            BattlegroundQueueTypeId queueTypeId = BattlegroundQueueTypeId(queueType);
            BattlegroundBracketId bracketId = BattlegroundBracketId(bracket);

            if (queueTypeId == BATTLEGROUND_QUEUE_SOLO)
                continue;

            if (!canJoinBg(queueTypeId, bracketId))
                continue;

            if (shouldJoinBg(queueTypeId, bracketId))
            {
                TC_LOG_INFO("playerbots", "Bot %s should join BG queue %d bracket %d", bot->GetName().c_str(), queueType, bracket);
                bgList.push_back(queueTypeId);
            }
        }
    }

    if (bgList.empty())
    {
        static time_t lastLog = 0;
        if (time(nullptr) > lastLog + 60)
        {
            lastLog = time(nullptr);
            TC_LOG_INFO("playerbots", "BGJoinAction::isUseful: no BG queues found for bot %s (level %u)", bot->GetName().c_str(), bot->GetLevel());
        }
    }

    return !bgList.empty();
}

bool BGJoinAction::JoinQueue(uint32 type)
{
    if (!bot || (!bot->IsInWorld() && !bot->IsBeingTeleported()) || bot->InBattleground())
        return false;

    BattlegroundQueueTypeId queueTypeId = BattlegroundQueueTypeId(type);
    BattlegroundTypeId bgTypeId = BattlegroundMgr::BGTemplateId(queueTypeId);

    Battleground* bg = sBattlegroundMgr->GetBattlegroundTemplate(bgTypeId);
    if (!bg)
        return false;

    PvPDifficultyEntry const* pvpDiff = GetBattlegroundBracketByLevel(bg->GetMapId(), bot->GetLevel());
    if (!pvpDiff)
        return false;

    BattlegroundBracketId bracketId = pvpDiff->GetBracketId();
    TeamId teamId = bot->GetTeamId();

    if (bot->InBattlegroundQueueForBattlegroundQueueType(queueTypeId))
        return false;

    if (!bot->GetBGAccessByLevel(bgTypeId))
        return false;

    uint8 arenaType = BattlegroundMgr::BGArenaType(queueTypeId);
    bool isArena = (arenaType != 0);
    Group* group = bot->GetGroup();
    bool joinAsGroup = group && group->IsLeader(bot->GetGUID());

    std::string bgName;
    switch (bgTypeId)
    {
        case BATTLEGROUND_AV:  bgName = "AV";   break;
        case BATTLEGROUND_WS:  bgName = "WSG";  break;
        case BATTLEGROUND_AB:  bgName = "AB";   break;
        case BATTLEGROUND_EY:  bgName = "EotS"; break;
        case BATTLEGROUND_SA:  bgName = "SotA"; break;
        case BATTLEGROUND_IC:  bgName = "IoC";  break;
        case BATTLEGROUND_TP:  bgName = "TP";   break;
        case BATTLEGROUND_BFG: bgName = "BfG";  break;
        case BATTLEGROUND_TOK: bgName = "ToK";  break;
        case BATTLEGROUND_SM:  bgName = "SM";   break;
        case BATTLEGROUND_DG:  bgName = "DG";   break;
        case BATTLEGROUND_RB:  bgName = "Random"; break;
        case BATTLEGROUND_RATED_10_VS_10: bgName = "Rated BG"; break;
        default:
            if (isArena)
            {
                switch (arenaType)
                {
                    case 2: bgName = "2v2"; break;
                    case 3: bgName = "3v3"; break;
                    case 5: bgName = "5v5"; break;
                }
            }
            break;
    }

    TC_LOG_INFO("playerbots", "Bot {} {}:{} <{}> queuing for {}{}",
        bot->GetGUID().ToString().c_str(),
        teamId == TEAM_ALLIANCE ? "A" : "H", bot->GetLevel(),
        bot->GetName().c_str(), bgName.c_str(), isArena ? " Arena" : "");

    BattlegroundQueue& bgQueue = sBattlegroundMgr->GetBattlegroundQueue(queueTypeId);

    if (isArena)
    {
        if (!group || !joinAsGroup)
        {
            botAI->GetAiObjectContext()->GetValue<uint32>("bg type")->Set(0);
            return false;
        }

        uint8 arenaSlot = ArenaTeam::GetSlotByType(arenaType);
        ArenaTeam team{ RatedPvpSlot(arenaSlot), group };
        uint32 arenaRating = std::max(team.GetRating(), (uint32)1);
        uint32 matchmakerRating = team.GetMatchmakerRating();

        GroupQueueInfo* ginfo = bgQueue.AddGroup(bot, group, bgTypeId, pvpDiff, arenaType, false, false, arenaRating, matchmakerRating);
        if (ginfo)
        {
            for (GroupReference* itr = group->GetFirstMember(); itr != nullptr; itr = itr->next())
            {
                Player* member = itr->GetSource();
                if (!member)
                    continue;

                uint32 queueSlot = member->AddBattlegroundQueueId(queueTypeId);
                member->AddBattlegroundQueueJoinTime(bgTypeId, ginfo->JoinTime);

                WorldPacket data;
                sBattlegroundMgr->BuildBattlegroundStatusPacket(&data, bg, member, queueSlot, STATUS_WAIT_QUEUE, 0, ginfo->JoinTime, arenaType);
                member->GetSession()->SendPacket(&data);
            }

            sRandomPlayerbotMgr->BattlegroundData[queueTypeId][bracketId].skirmishArenaBotCount += group->GetMembersCount();
            sBattlegroundMgr->ScheduleQueueUpdate(matchmakerRating, arenaType, queueTypeId, bgTypeId, bracketId);
        }
    }
    else if (queueTypeId == BATTLEGORUND_QUEUE_RATED_BG)
    {
        if (!group || !joinAsGroup || group->GetMembersCount() < 10)
        {
            botAI->GetAiObjectContext()->GetValue<uint32>("bg type")->Set(0);
            return false;
        }

        uint32 arenaRating = 1500;
        uint32 matchmakerRating = 1500;

        GroupQueueInfo* ginfo = bgQueue.AddGroup(bot, group, bgTypeId, pvpDiff, 0, true, true, arenaRating, matchmakerRating);
        if (ginfo)
        {
            for (GroupReference* itr = group->GetFirstMember(); itr != nullptr; itr = itr->next())
            {
                Player* member = itr->GetSource();
                if (!member)
                    continue;

                uint32 queueSlot = member->AddBattlegroundQueueId(queueTypeId);
                member->AddBattlegroundQueueJoinTime(bgTypeId, ginfo->JoinTime);

                WorldPacket data;
                sBattlegroundMgr->BuildBattlegroundStatusPacket(&data, bg, member, queueSlot, STATUS_WAIT_QUEUE, 0, ginfo->JoinTime, 0);
                member->GetSession()->SendPacket(&data);
            }

            if (teamId == TEAM_ALLIANCE)
                sRandomPlayerbotMgr->BattlegroundData[queueTypeId][bracketId].ratedBgAllianceBotCount += group->GetMembersCount();
            else
                sRandomPlayerbotMgr->BattlegroundData[queueTypeId][bracketId].ratedBgHordeBotCount += group->GetMembersCount();

            sBattlegroundMgr->ScheduleQueueUpdate(matchmakerRating, 0, queueTypeId, bgTypeId, bracketId);
        }
    }
    else
    {
        bool isPremade = joinAsGroup && group && group->GetMembersCount() >= 4;

        if (joinAsGroup && group)
        {
            GroupQueueInfo* ginfo = bgQueue.AddGroup(bot, group, bgTypeId, pvpDiff, 0, false, isPremade, 0, 0);
            if (ginfo)
            {
                for (GroupReference* itr = group->GetFirstMember(); itr != nullptr; itr = itr->next())
                {
                    Player* member = itr->GetSource();
                    if (!member)
                        continue;

                    uint32 queueSlot = member->AddBattlegroundQueueId(queueTypeId);
                    member->AddBattlegroundQueueJoinTime(bgTypeId, ginfo->JoinTime);

                    WorldPacket data;
                    sBattlegroundMgr->BuildBattlegroundStatusPacket(&data, bg, member, queueSlot, STATUS_WAIT_QUEUE, 0, ginfo->JoinTime, 0);
                    member->GetSession()->SendPacket(&data);
                }

                if (teamId == TEAM_ALLIANCE)
                    sRandomPlayerbotMgr->BattlegroundData[queueTypeId][bracketId].bgAllianceBotCount += group->GetMembersCount();
                else
                    sRandomPlayerbotMgr->BattlegroundData[queueTypeId][bracketId].bgHordeBotCount += group->GetMembersCount();
            }
        }
        else
        {
            GroupQueueInfo* ginfo = bgQueue.AddGroup(bot, nullptr, bgTypeId, pvpDiff, 0, false, false, 0, 0);
            if (ginfo)
            {
                uint32 queueSlot = bot->AddBattlegroundQueueId(queueTypeId);
                bot->AddBattlegroundQueueJoinTime(bgTypeId, ginfo->JoinTime);

                WorldPacket data;
                sBattlegroundMgr->BuildBattlegroundStatusPacket(&data, bg, bot, queueSlot, STATUS_WAIT_QUEUE, 0, ginfo->JoinTime, 0);
                bot->GetSession()->SendPacket(&data);

                if (teamId == TEAM_ALLIANCE)
                    sRandomPlayerbotMgr->BattlegroundData[queueTypeId][bracketId].bgAllianceBotCount++;
                else
                    sRandomPlayerbotMgr->BattlegroundData[queueTypeId][bracketId].bgHordeBotCount++;
            }
        }

        sBattlegroundMgr->ScheduleQueueUpdate(0, 0, queueTypeId, bgTypeId, bracketId);
    }

    botAI->GetAiObjectContext()->GetValue<uint32>("bg type")->Set(0);
    return true;
}

bool BGLeaveAction::Execute(Event /*event*/)
{
    if (!bot->InBattlegroundQueue() && !bot->InBattleground())
        return false;

    if (bot->InBattleground())
    {
        Battleground* bg = bot->GetBattleground();
        if (bg)
        {
            bg->RemovePlayerAtLeave(bot->GetGUID(), true, true);
            TC_LOG_INFO("playerbots", "Bot {} <{}>: Left BG", bot->GetGUID().ToString().c_str(), bot->GetName().c_str());
        }
    }
    else
    {
        for (uint8 i = 0; i < PLAYER_MAX_BATTLEGROUND_QUEUES; ++i)
        {
            BattlegroundQueueTypeId queueTypeId = bot->GetBattlegroundQueueTypeId(i);
            if (queueTypeId == BATTLEGROUND_QUEUE_NONE)
                continue;

            bot->RemoveBattlegroundQueueId(queueTypeId);
            sBattlegroundMgr->GetBattlegroundQueue(queueTypeId).RemovePlayer(bot->GetGUID(), true);
        }
    }

    botAI->GetAiObjectContext()->GetValue<uint32>("bg type")->Set(0);
    botAI->GetAiObjectContext()->GetValue<uint32>("arena type")->Set(0);

    return true;
}

bool BGStatusCheckAction::Execute(Event /*event*/)
{
    if (bot->IsBeingTeleported())
        return false;

    for (uint8 i = 0; i < PLAYER_MAX_BATTLEGROUND_QUEUES; ++i)
    {
        BattlegroundQueueTypeId queueTypeId = bot->GetBattlegroundQueueTypeId(i);
        if (queueTypeId == BATTLEGROUND_QUEUE_NONE)
            continue;

        BattlegroundQueue& bgQueue = sBattlegroundMgr->GetBattlegroundQueue(queueTypeId);
        GroupQueueInfo ginfo;
        if (!bgQueue.GetPlayerGroupInfoData(bot->GetGUID(), &ginfo))
            continue;

        if (ginfo.IsInvitedToBGInstanceGUID && !bot->InBattleground())
        {
            BattlegroundTypeId bgTypeId = BattlegroundMgr::BGTemplateId(queueTypeId);
            // Random BG / All Arenas / Rated BG create instances under a specific BG type, not their meta-type
            bool isMetaQueue = (bgTypeId == BATTLEGROUND_AA || bgTypeId == BATTLEGROUND_RB || bgTypeId == BATTLEGROUND_RATED_10_VS_10);
            Battleground* bg = sBattlegroundMgr->GetBattleground(
                ginfo.IsInvitedToBGInstanceGUID,
                isMetaQueue ? BATTLEGROUND_TYPE_NONE : bgTypeId);

            if (bg)
            {
                TC_LOG_INFO("playerbots", "Bot {} <{}>: Accepting BG invite",
                    bot->GetGUID().ToString().c_str(), bot->GetName().c_str());

                bot->SetBattlegroundEntryPoint();
                sBattlegroundMgr->SendToBattleground(bot, ginfo.IsInvitedToBGInstanceGUID, bgTypeId);
                return true;
            }
        }
    }

    return false;
}

bool BGStatusCheckAction::isUseful()
{
    return bot->InBattlegroundQueue();
}
