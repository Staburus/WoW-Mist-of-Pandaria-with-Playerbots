#ifndef _PLAYERBOT_BATTLEGROUNDSTRATEGY_H
#define _PLAYERBOT_BATTLEGROUNDSTRATEGY_H

#include "PassTroughStrategy.h"

class PlayerbotAI;

class BGStrategy : public PassTroughStrategy
{
public:
    BGStrategy(PlayerbotAI* botAI);
    std::string const getName() override { return "bg"; }

    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

#endif
