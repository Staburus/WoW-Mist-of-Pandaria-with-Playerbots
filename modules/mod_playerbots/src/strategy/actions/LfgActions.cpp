#include "LfgActions.h"

#include "AiFactory.h"
#include "LFGMgr.h"
#include "Opcodes.h"
#include "PlayerbotAI.h"
#include "PlayerbotSpec.h"
#include "Playerbots.h"
#include "RandomPlayerbotMgr.h"
#include "SharedDefines.h"
#include "WorldPacket.h"

using namespace lfg;

bool ParseLfgProposalId(WorldPacket const& packet, uint32& proposalId)
{
    if (packet.empty())
        return false;

    WorldPacket p = packet;
    p.rpos(0);

    p.ReadBit();
    p.ReadBit();
    p.ReadBit();
    p.ReadBit();
    p.ReadBit();
    p.ReadBit();
    p.ReadBit();
    p.ReadBit();
    p.ReadBit();
    p.ReadBit();
    p.ReadBit();
    p.ReadBit();
    p.ReadBit();

    uint32 playerCount = p.ReadBits(21);
    for (uint32 i = 0; i < playerCount; ++i)
        for (int j = 0; j < 5; ++j)
            p.ReadBit();

    p.ReadBit();
    p.ReadBit();
    p.ReadBit();
    p.ReadBit();
    p.ReadBit();

    uint8 g2_1 = 0, g1_4 = 0, g2_4 = 0, g1_7 = 0, g1_2 = 0, g1_0 = 0, g2_6 = 0, g1_5 = 0, g1_3 = 0;
    p.ReadByteSeq(g2_1);
    p.ReadByteSeq(g1_4);
    p.ReadByteSeq(g2_4);
    p.ReadByteSeq(g1_7);
    p.ReadByteSeq(g1_2);
    p.ReadByteSeq(g1_0);

    uint32 dungeonEntry = 0;
    p >> dungeonEntry;
    uint8 state = 0;
    p >> state;
    uint32 clientQueueId = 0;
    p >> clientQueueId;
    p.ReadByteSeq(g2_6);
    p >> proposalId;

    return proposalId != 0;
}

bool LfgJoinAction::Execute(Event /*event*/)
{
    return JoinLFG();
}

uint32 LfgJoinAction::GetRoles()
{
    if (!sRandomPlayerbotMgr->IsRandomBot(bot))
    {
        if (PlayerBotSpec::IsTank(bot))
            return PLAYER_ROLE_TANK;
        if (PlayerBotSpec::IsHeal(bot))
            return PLAYER_ROLE_HEALER;
        return PLAYER_ROLE_DAMAGE;
    }

    Specializations spec = AiFactory::GetPlayerSpecTab(bot);
    switch (bot->GetClass())
    {
        case CLASS_DRUID:
            if (spec == Specializations::SPEC_DRUID_RESTORATION)
                return PLAYER_ROLE_HEALER;
            if (spec == Specializations::SPEC_DRUID_GUARDIAN)
                return PLAYER_ROLE_TANK;
            return PLAYER_ROLE_DAMAGE;
        case CLASS_PALADIN:
            if (spec == Specializations::SPEC_PALADIN_PROTECTION)
                return PLAYER_ROLE_TANK;
            if (spec == Specializations::SPEC_PALADIN_HOLY)
                return PLAYER_ROLE_HEALER;
            return PLAYER_ROLE_DAMAGE;
        case CLASS_PRIEST:
            if (spec != Specializations::SPEC_PRIEST_SHADOW)
                return PLAYER_ROLE_HEALER;
            return PLAYER_ROLE_DAMAGE;
        case CLASS_SHAMAN:
            if (spec == Specializations::SPEC_SHAMAN_RESTORATION)
                return PLAYER_ROLE_HEALER;
            return PLAYER_ROLE_DAMAGE;
        case CLASS_WARRIOR:
            if (spec == Specializations::SPEC_WARRIOR_PROTECTION)
                return PLAYER_ROLE_TANK;
            return PLAYER_ROLE_DAMAGE;
        case CLASS_DEATH_KNIGHT:
            if (spec == Specializations::SPEC_DEATH_KNIGHT_BLOOD)
                return PLAYER_ROLE_TANK;
            return PLAYER_ROLE_DAMAGE;
        case CLASS_MONK:
            if (spec == Specializations::SPEC_MONK_BREWMASTER)
                return PLAYER_ROLE_TANK;
            if (spec == Specializations::SPEC_MONK_MISTWEAVER)
                return PLAYER_ROLE_HEALER;
            return PLAYER_ROLE_DAMAGE;
        default:
            return PLAYER_ROLE_DAMAGE;
    }
}

bool LfgJoinAction::JoinLFG()
{
    if (sLFGMgr->GetActiveState(bot->GetGUID()) != LFG_STATE_NONE)
        return false;

    LfgDungeonSet list;
    std::vector<uint32> selected;

    std::vector<uint32> dungeons = sRandomPlayerbotMgr->LfgDungeons[bot->GetTeamId()];
    if (dungeons.empty())
    {
        static time_t lastLfgLog = 0;
        if (time(nullptr) > lastLfgLog + 60)
        {
            lastLfgLog = time(nullptr);
            TC_LOG_INFO("playerbots", "LfgJoinAction: LfgDungeons empty for team %u - no real player in LFG queue", bot->GetTeamId());
        }
        return false;
    }

    uint8 const botLevel = bot->GetLevel();
    for (uint32 dungeonId : dungeons)
    {
        LFGDungeonData const* dungeon = sLFGMgr->GetLFGDungeon(dungeonId);
        if (!dungeon || (dungeon->type != LFG_TYPE_RANDOM && dungeon->type != LFG_TYPE_DUNGEON &&
                         dungeon->type != LFG_TYPE_HEROIC && dungeon->type != LFG_TYPE_RAID))
            continue;

        if (dungeon->minlevel && (botLevel < dungeon->minlevel || botLevel > dungeon->maxlevel) ||
            (botLevel > dungeon->minlevel + 10 && dungeon->type == LFG_TYPE_DUNGEON))
            continue;

        selected.push_back(dungeon->Entry());
        list.insert(dungeon->Entry());
    }

    if (selected.empty() || list.empty())
        return false;

    uint32 roleMask = GetRoles();
    std::string const comment = std::to_string(PlayerbotAI::GetMixedGearScore(bot, true, false, 12));

    TC_LOG_INFO("playerbots", "Bot {} {}:{} <{}>: queues LFG as {} ({} dungeons)",
        bot->GetGUID().ToString().c_str(), bot->GetTeamId() == TEAM_ALLIANCE ? "A" : "H", bot->GetLevel(),
        bot->GetName().c_str(), roleMask, list.size());

  // MoP 5.4.8 CMSG_LFG_JOIN: unk8, 3x Needs, roles, bit-packed dungeon count/comment, dungeons, comment
    WorldPacket* data = new WorldPacket(CMSG_LFG_JOIN);
    *data << uint8(0);
    *data << uint32(0) << uint32(0) << uint32(0);
    *data << uint32(roleMask);
    data->WriteBits(list.size(), 22);
    data->WriteBits(comment.size(), 8);
    data->WriteBit(0);
    data->FlushBits();
    for (uint32 dungeon : list)
        *data << dungeon;
    data->WriteString(comment);
    bot->GetSession()->QueuePacket(data);

    return true;
}

bool LfgRoleCheckAction::Execute(Event /*event*/)
{
    Group* group = bot->GetGroup();
    if (!group)
        return false;

    uint32 newRoles = GetRoles();
    WorldPacket* packet = new WorldPacket(CMSG_LFG_SET_ROLES);
    *packet << uint32(newRoles);
    *packet << uint8(0);
    bot->GetSession()->QueuePacket(packet);

    TC_LOG_INFO("playerbots", "Bot {} {}:{} <{}>: LFG roles checked",
        bot->GetGUID().ToString().c_str(), bot->GetTeamId() == TEAM_ALLIANCE ? "A" : "H", bot->GetLevel(),
        bot->GetName().c_str());

    return true;
}

bool LfgAcceptAction::Execute(Event event)
{
    auto acceptProposal = [&](uint32 proposalId, bool accept)
    {
        sLFGMgr->UpdateProposal(proposalId, bot->GetGUID(), accept);
        botAI->GetAiObjectContext()->GetValue<uint32>("lfg proposal")->Set(0);

        if (accept)
        {
            bot->ClearUnitState(UNIT_STATE_ALL_STATE);
            if (sRandomPlayerbotMgr->IsRandomBot(bot) && !bot->GetGroup())
            {
                sRandomPlayerbotMgr->Refresh(bot);
                botAI->ResetStrategies();
            }
            botAI->Reset();
        }
    };

    uint32 id = AI_VALUE(uint32, "lfg proposal");
    if (id)
    {
        bool accept = !(bot->IsInCombat() || bot->isDead());
        TC_LOG_INFO("playerbots", "Bot {} <{}>: LFG proposal %u - %s (combat:%d dead:%d)",
            bot->GetGUID().ToString().c_str(), bot->GetName().c_str(), id,
            accept ? "ACCEPTING" : "DECLINING", bot->IsInCombat(), bot->isDead());
        acceptProposal(id, accept);
        return true;
    }

    WorldPacket p(event.getPacket());
    if (!p.empty())
    {
        uint32 proposalId = 0;
        if (ParseLfgProposalId(p, proposalId) && proposalId)
        {
            bool accept = !(bot->IsInCombat() || bot->isDead());
            TC_LOG_INFO("playerbots", "Bot {} <{}>: LFG proposal %u (packet) - %s",
                bot->GetGUID().ToString().c_str(), bot->GetName().c_str(), proposalId,
                accept ? "ACCEPTING" : "DECLINING");
            acceptProposal(proposalId, accept);
            return true;
        }
    }

    return false;
}

bool LfgLeaveAction::Execute(Event /*event*/)
{
    if (sLFGMgr->GetActiveState(bot->GetGUID()) > LFG_STATE_QUEUED)
        return false;

    uint32 queueId = sLFGMgr->GetActiveQueueId(bot->GetGUID());
    if (!queueId)
        return false;

    sLFGMgr->LeaveLfg(bot->GetGUID(), queueId);
    return true;
}

bool LfgLeaveAction::isUseful()
{
    return true;
}

bool LfgTeleportAction::Execute(Event event)
{
    bool out = false;

    WorldPacket p(event.getPacket());
    if (!p.empty())
    {
        p.rpos(0);
        out = p.ReadBit();
    }

    bot->ClearUnitState(UNIT_STATE_ALL_STATE);

    WorldPacket* packet = new WorldPacket(CMSG_LFG_TELEPORT);
    packet->WriteBit(out);
    packet->FlushBits();
    bot->GetSession()->QueuePacket(packet);

    return true;
}

bool LfgJoinAction::isUseful()
{
    if (!sPlayerbotAIConfig->randomBotJoinLfg)
        return false;

    if (bot->GetLevel() < 15)
        return false;

    if (GET_PLAYERBOT_AI(bot) && GET_PLAYERBOT_AI(bot)->IsRealPlayer())
        return false;

    if (bot->GetGroup() && bot->GetGroup()->GetLeaderGUID() != bot->GetGUID())
        return false;

    if (bot->IsBeingTeleported())
        return false;

    if (bot->InBattleground() || bot->InBattlegroundQueue())
        return false;

    if (bot->isDead())
        return false;

    if (!sRandomPlayerbotMgr->IsRandomBot(bot))
        return false;

    Map* map = bot->GetMap();
    if (map && map->Instanceable())
        return false;

    if (sLFGMgr->GetActiveState(bot->GetGUID()) != LFG_STATE_NONE)
        return false;

    return true;
}
