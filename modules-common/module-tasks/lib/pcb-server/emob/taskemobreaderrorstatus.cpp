#include "taskemobreaderrorstatus.h"
#include "taskscpigetint.h"

TaskTemplatePtr TaskEmobReadErrorStatus::create(Zera::PcbInterfacePtr pcbInterface, std::shared_ptr<int> emobErrorStatus,
                                                QString channelMName,
                                                int timeout,
                                                std::function<void ()> additionalErrorHandler)
{
    QString cmd = channelMName == "" ? "SYST:EMOB:ERROR?" : QString("SYST:EMOB:ERROR? %1;").arg(channelMName);
    return TaskScpiGetInt::create(pcbInterface,
                                  cmd,
                                  emobErrorStatus,
                                  timeout,
                                  additionalErrorHandler);
}
