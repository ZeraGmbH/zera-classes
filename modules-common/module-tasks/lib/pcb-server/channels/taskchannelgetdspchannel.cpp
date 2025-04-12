#include "taskchannelgetdspchannel.h"
#include "taskgetscpiint.h"

TaskTemplatePtr TaskChannelGetDspChannel::create(Zera::PcbInterfacePtr pcbInterface,
                                                 QString channelName,
                                                 std::shared_ptr<int> valueReceived,
                                                 int timeout, std::function<void()> additionalErrorHandler)
{
    return TaskGetScpiInt::create(pcbInterface,
                                  QString("SENSE:%1:DSPCHANNEL?").arg(channelName),
                                  valueReceived,
                                  timeout,
                                  additionalErrorHandler);
}
