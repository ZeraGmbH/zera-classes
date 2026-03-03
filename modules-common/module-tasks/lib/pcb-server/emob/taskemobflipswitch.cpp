#include "taskemobflipswitch.h"
#include "taskscpicmd.h"

TaskTemplatePtr TaskEmobFlipSwitch::create(Zera::PcbInterfacePtr pcbInterface,
                                           QString channelMName, bool onOff,
                                           int timeout,
                                           std::function<void ()> additionalErrorHandler)
{
    QString cmd;
    if(onOff)
        cmd = channelMName == "" ? "SYST:EMOB:ONSWITCH;" : QString("SYST:EMOB:ONSWITCH %1;").arg(channelMName);
    else
        cmd = channelMName == "" ? "SYST:EMOB:OFFSWITCH;" : QString("SYST:EMOB:OFFSWITCH %1;").arg(channelMName);
    return TaskScpiCmd::create(pcbInterface,
                               cmd,
                               timeout,
                               additionalErrorHandler);
}
