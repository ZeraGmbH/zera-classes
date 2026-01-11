#include "tasksetpllchannel.h"
#include "taskscpicmd.h"

TaskTemplatePtr TaskSetPllChannel::create(Zera::PcbInterfacePtr pcbInterface,
                                          const QString &pllChannelMName,
                                          int timeout,
                                          std::function<void ()> additionalErrorHandler)
{
    return TaskScpiCmd::create(pcbInterface,
                               QString("SAMPLE:S0:PLL %1;").arg(pllChannelMName),
                               timeout,
                               additionalErrorHandler);
}
