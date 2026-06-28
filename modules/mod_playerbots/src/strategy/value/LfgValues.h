#ifndef _PLAYERBOT_LFGVALUES_H
#define _PLAYERBOT_LFGVALUES_H

#include "Value.h"

class PlayerbotAI;

class LfgProposalValue : public ManualSetValue<uint32>
{
public:
    LfgProposalValue(PlayerbotAI* botAI) : ManualSetValue<uint32>(botAI, 0, "lfg proposal") {}
};

#endif
