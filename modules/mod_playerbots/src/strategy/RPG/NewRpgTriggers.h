#ifndef _PLAYERBOT_NEWRPGTRIGGERS_H
#define _PLAYERBOT_NEWRPGTRIGGERS_H

#include "NewRpgStrategy.h"
#include "Trigger.h"

class NewRpgStatusTrigger : public Trigger
{
public:
    NewRpgStatusTrigger(PlayerbotAI* botAI, std::string const& name, NewRpgStatus status)
        : Trigger(botAI, name), status(status)
    {
    }
    bool IsActive() override;

protected:
    NewRpgStatus status;
};

#endif
