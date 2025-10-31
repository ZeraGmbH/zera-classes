#include "taskemobreadconnectionstate.h"
#include "taskscpigetint.h"

TaskTemplatePtr TaskEmobReadConnectionState::create(Zera::PcbInterfacePtr pcbInterface,
                                                    std::shared_ptr<int> stateReceived,
                                                    int timeout,
                                                    std::function<void ()> additionalErrorHandler)
{
    return TaskScpiGetInt::create(pcbInterface,
                                  "SYST:EMOB:LOCKST?",
                                  stateReceived,
                                  timeout,
                                  additionalErrorHandler);
}
