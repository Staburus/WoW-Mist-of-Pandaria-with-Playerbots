#ifndef _PLAYERBOT_BATTLEGROUNDJOINACTION_H
#define _PLAYERBOT_BATTLEGROUNDJOINACTION_H

#include "Action.h"

class PlayerbotAI;

class BGJoinAction : public Action
{
public:
    BGJoinAction(PlayerbotAI* botAI, std::string const name = "bg join") : Action(botAI, name) {}

    bool Execute(Event event) override;
    bool isUseful() override;

protected:
    bool canJoinBg(BattlegroundQueueTypeId queueTypeId, BattlegroundBracketId bracketId);
    bool shouldJoinBg(BattlegroundQueueTypeId queueTypeId, BattlegroundBracketId bracketId);
    bool JoinQueue(uint32 type);
    bool GatherArenaGroup(uint8 arenaType);
    bool GatherRatedBgGroup();

    std::vector<uint32> bgList;
};

class BGLeaveAction : public Action
{
public:
    BGLeaveAction(PlayerbotAI* botAI) : Action(botAI, "bg leave") {}

    bool Execute(Event event) override;
};

class BGStatusCheckAction : public Action
{
public:
    BGStatusCheckAction(PlayerbotAI* botAI) : Action(botAI, "bg status check") {}

    bool Execute(Event event) override;
    bool isUseful() override;
};

#endif
