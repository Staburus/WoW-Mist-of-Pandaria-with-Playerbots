#include "LfgTriggers.h"

#include "Playerbots.h"

bool LfgProposalActiveTrigger::IsActive()
{
    return AI_VALUE(uint32, "lfg proposal");
}
