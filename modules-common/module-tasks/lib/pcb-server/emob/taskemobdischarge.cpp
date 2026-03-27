#include "taskemobdischarge.h"
#include "taskscpicmd.h"

TaskTemplatePtr TaskEmobDischarge::create(Zera::PcbInterfacePtr pcbInterface,
                                           QString channelMName, bool onOff,
                                           int timeout,
                                           std::function<void ()> additionalErrorHandler)
{
    QString cmd;
    if(onOff)
        cmd = channelMName == "" ? "SYST:EMOB:ONDISCHARGE;" : QString("SYST:EMOB:ONDISCHARGE %1;").arg(channelMName);
    else
        cmd = channelMName == "" ? "SYST:EMOB:OFFDISCHARGE;" : QString("SYST:EMOB:OFFDISCHARGE %1;").arg(channelMName);
    return TaskScpiCmd::create(pcbInterface,
                               cmd,
                               timeout,
                               additionalErrorHandler);
}
