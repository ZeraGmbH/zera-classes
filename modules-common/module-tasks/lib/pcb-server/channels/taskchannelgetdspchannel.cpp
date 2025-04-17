#include "taskchannelgetdspchannel.h"
#include "taskscpigetint.h"

TaskTemplatePtr TaskChannelGetDspChannel::create(Zera::PcbInterfacePtr pcbInterface,
                                                 QString channelName,
                                                 std::shared_ptr<int> valueReceived,
                                                 int timeout, std::function<void()> additionalErrorHandler)
{
    return TaskScpiGetInt::create(pcbInterface,
                                  QString("SENSE:%1:DSPCHANNEL?").arg(channelName),
                                  valueReceived,
                                  timeout,
                                  additionalErrorHandler);
}
