#ifndef _PLAYERBOT_LFGSTRATEGY_H
#define _PLAYERBOT_LFGSTRATEGY_H

#include "PassTroughStrategy.h"

class PlayerbotAI;

class LfgStrategy : public PassTroughStrategy
{
public:
    LfgStrategy(PlayerbotAI* botAI);

    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    std::string const getName() override { return "lfg"; }
};

#endif
