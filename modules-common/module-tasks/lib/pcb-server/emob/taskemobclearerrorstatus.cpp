#include "taskemobclearerrorstatus.h"
#include "taskscpicmd.h"

TaskTemplatePtr TaskEmobClearErrorStatus::create(Zera::PcbInterfacePtr pcbInterface,
                                                 QString channelMName,
                                                 int timeout,
                                                 std::function<void ()> additionalErrorHandler)
{
    QString cmd = channelMName == "" ? "SYST:EMOB:CLEARERROR;" : QString("SYST:EMOB:CLEARERROR %1;").arg(channelMName);
    return TaskScpiCmd::create(pcbInterface,
                               cmd,
                               timeout,
                               additionalErrorHandler);

}
