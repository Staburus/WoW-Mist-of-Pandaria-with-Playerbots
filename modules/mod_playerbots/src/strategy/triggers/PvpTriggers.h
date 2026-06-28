#ifndef _PLAYERBOT_PVPTRIGGERS_H
#define _PLAYERBOT_PVPTRIGGERS_H

#include "GenericTriggers.h"

class PlayerbotAI;

class BgInviteActiveTrigger : public Trigger
{
public:
    BgInviteActiveTrigger(PlayerbotAI* botAI) : Trigger(botAI, "bg invite active", 2) {}

    bool IsActive() override;
};

class BgActiveTrigger : public Trigger
{
public:
    BgActiveTrigger(PlayerbotAI* botAI) : Trigger(botAI, "bg active", 1) {}

    bool IsActive() override;
};

class BgWaitingTrigger : public Trigger
{
public:
    BgWaitingTrigger(PlayerbotAI* botAI) : Trigger(botAI, "bg waiting", 1) {}

    bool IsActive() override;
};

class PlayerWantsInBattlegroundTrigger : public Trigger
{
public:
    PlayerWantsInBattlegroundTrigger(PlayerbotAI* botAI) : Trigger(botAI, "player wants in bg", 5) {}

    bool IsActive() override;
};

#endif
