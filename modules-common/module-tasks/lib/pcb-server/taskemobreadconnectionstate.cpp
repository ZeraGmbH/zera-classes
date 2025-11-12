#include "taskemobreadconnectionstate.h"
#include "taskscpigetint.h"

TaskTemplatePtr TaskEmobReadConnectionState::create(Zera::PcbInterfacePtr pcbInterface,
                                                    std::shared_ptr<int> stateReceived,
                                                    QString channelMName,
                                                    int timeout,
                                                    std::function<void ()> additionalErrorHandler)
{
    QString cmd = channelMName == "" ? "SYST:EMOB:LOCKST?" : QString("SYST:EMOB:LOCKST? %1;").arg(channelMName);
    return TaskScpiGetInt::create(pcbInterface,
                                  cmd,
                                  stateReceived,
                                  timeout,
                                  additionalErrorHandler);
}
