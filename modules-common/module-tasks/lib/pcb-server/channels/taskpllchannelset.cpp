#include "taskpllchannelset.h"
#include "taskscpicmd.h"

TaskTemplatePtr TaskPllChannelSet::create(AbstractServerInterfacePtr pcbInterface,
                                          const QString &pllChannelMName,
                                          int timeout,
                                          std::function<void ()> additionalErrorHandler)
{
    return TaskScpiCmd::create(pcbInterface,
                               QString("SAMPLE:S0:PLL %1;").arg(pllChannelMName),
                               timeout,
                               additionalErrorHandler);
}
