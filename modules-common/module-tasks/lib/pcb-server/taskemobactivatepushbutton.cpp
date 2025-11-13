#include "taskemobactivatepushbutton.h"
#include "taskscpicmd.h"

TaskTemplatePtr TaskEmobActivatePushButton::create(Zera::PcbInterfacePtr pcbInterface,
                                                   QString channelMName,
                                                   int timeout,
                                                   std::function<void ()> additionalErrorHandler)
{
    QString cmd = channelMName == "" ? "SYST:EMOB:PBPR;" : QString("SYST:EMOB:PBPR %1;").arg(channelMName);
    return TaskScpiCmd::create(pcbInterface,
                        cmd,
                        timeout,
                        additionalErrorHandler);
}
