#ifndef _PLAYERBOT_LFGTRIGGERS_H
#define _PLAYERBOT_LFGTRIGGERS_H

#include "Trigger.h"

class PlayerbotAI;

class LfgProposalActiveTrigger : public Trigger
{
public:
    LfgProposalActiveTrigger(PlayerbotAI* botAI) : Trigger(botAI, "lfg proposal active", 20 * 2000) {}

    bool IsActive() override;
};

#endif
