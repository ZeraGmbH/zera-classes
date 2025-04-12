#include "taskchannelgetunit.h"
#include "taskgetscpistring.h"

TaskTemplatePtr TaskChannelGetUnit::create(Zera::PcbInterfacePtr pcbInterface,
                                           QString channelName,
                                           std::shared_ptr<QString> valueReceived,
                                           int timeout, std::function<void ()> additionalErrorHandler)
{
    return TaskGetScpiString::create(pcbInterface,
                                     QString("SENSE:%1:UNIT?").arg(channelName),
                                     valueReceived,
                                     timeout,
                                     additionalErrorHandler);
}
