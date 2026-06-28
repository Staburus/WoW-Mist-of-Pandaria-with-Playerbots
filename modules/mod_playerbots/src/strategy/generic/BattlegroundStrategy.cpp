#include "BattlegroundStrategy.h"

#include "Playerbots.h"

BGStrategy::BGStrategy(PlayerbotAI* botAI) : PassTroughStrategy(botAI) {}

void BGStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode("often", NextAction::array(0, new NextAction("bg join", relevance), nullptr)));
    triggers.push_back(new TriggerNode("bg invite active", NextAction::array(0, new NextAction("bg status check", relevance), nullptr)));
}
