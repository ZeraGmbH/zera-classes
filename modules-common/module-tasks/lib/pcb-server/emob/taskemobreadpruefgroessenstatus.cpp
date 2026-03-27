#include "taskemobreadpruefgroessenstatus.h"
#include "taskscpigetint.h"

TaskTemplatePtr TaskEmobReadPruefgroessenStatus::create(Zera::PcbInterfacePtr pcbInterface, std::shared_ptr<int> emobPruefgroessenStatus,
                                                        QString channelMName,
                                                        int timeout,
                                                        std::function<void ()> additionalErrorHandler)
{
    QString cmd = channelMName == "" ? "SYST:EMOB:PRUEFGROESSENSTATUS?" : QString("SYST:EMOB:PRUEFGROESSENSTATUS? %1;").arg(channelMName);
    return TaskScpiGetInt::create(pcbInterface,
                                  cmd,
                                  emobPruefgroessenStatus,
                                  timeout,
                                  additionalErrorHandler);
}
