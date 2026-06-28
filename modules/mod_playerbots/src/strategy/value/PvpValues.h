#ifndef _PLAYERBOT_PVPVALUES_H
#define _PLAYERBOT_PVPVALUES_H

#include "Value.h"

class PlayerbotAI;

class BgTypeValue : public ManualSetValue<uint32>
{
public:
    BgTypeValue(PlayerbotAI* botAI) : ManualSetValue<uint32>(botAI, 0, "bg type") {}
};

class ArenaTypeValue : public ManualSetValue<uint32>
{
public:
    ArenaTypeValue(PlayerbotAI* botAI) : ManualSetValue<uint32>(botAI, 0, "arena type") {}
};

class BgRoleValue : public ManualSetValue<uint32>
{
public:
    BgRoleValue(PlayerbotAI* botAI) : ManualSetValue<uint32>(botAI, 0, "bg role") {}
};

#endif
