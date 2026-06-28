#ifndef _PLAYERBOT_LFGTRIGGERS_H
#define _PLAYERBOT_LFGTRIGGERS_H

#include "Trigger.h"

class PlayerbotAI;

class LfgProposalActiveTrigger : public Trigger
{
public:
    LfgProposalActiveTrigger(PlayerbotAI* botAI) : Trigger(botAI, "lfg proposal active", 3 * 1000) {}

    bool IsActive() override;
};

class LfgJoinTrigger : public Trigger
{
public:
    LfgJoinTrigger(PlayerbotAI* botAI) : Trigger(botAI, "lfg join timer", 2) {}

    bool IsActive() override { return true; }
};

#endif
