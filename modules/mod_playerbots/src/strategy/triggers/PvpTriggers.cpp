#include "PvpTriggers.h"

#include "Battleground.h"
#include "BattlegroundMgr.h"
#include "BattlegroundQueue.h"
#include "PlayerbotAI.h"
#include "Playerbots.h"
#include "RandomPlayerbotMgr.h"

bool BgInviteActiveTrigger::IsActive()
{
    if (!bot->InBattlegroundQueue())
        return false;

    for (uint8 i = 0; i < PLAYER_MAX_BATTLEGROUND_QUEUES; ++i)
    {
        BattlegroundQueueTypeId queueTypeId = bot->GetBattlegroundQueueTypeId(i);
        if (queueTypeId == BATTLEGROUND_QUEUE_NONE)
            continue;

        BattlegroundQueue& bgQueue = sBattlegroundMgr->GetBattlegroundQueue(queueTypeId);
        GroupQueueInfo ginfo;
        if (bgQueue.GetPlayerGroupInfoData(bot->GetGUID(), &ginfo))
        {
            if (ginfo.IsInvitedToBGInstanceGUID && ginfo.RemoveInviteTime > 0)
                return true;
        }
    }

    return false;
}

bool BgActiveTrigger::IsActive()
{
    if (!bot->InBattleground())
        return false;

    Battleground* bg = bot->GetBattleground();
    if (!bg)
        return false;

    return bg->GetStatus() == STATUS_IN_PROGRESS;
}

bool BgWaitingTrigger::IsActive()
{
    if (!bot->InBattleground())
        return false;

    Battleground* bg = bot->GetBattleground();
    if (!bg)
        return false;

    return bg->GetStatus() == STATUS_WAIT_JOIN;
}

bool PlayerWantsInBattlegroundTrigger::IsActive()
{
    if (!sPlayerbotAIConfig->randomBotJoinBG)
        return false;

    if (bot->InBattleground() || bot->InBattlegroundQueue())
        return false;

    if (!sRandomPlayerbotMgr->IsRandomBot(bot))
        return false;

    return true;
}
