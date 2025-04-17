#include "taskchannelgetunit.h"
#include "taskscpigetstring.h"

TaskTemplatePtr TaskChannelGetUnit::create(Zera::PcbInterfacePtr pcbInterface,
                                           QString channelName,
                                           std::shared_ptr<QString> valueReceived,
                                           int timeout, std::function<void ()> additionalErrorHandler)
{
    return TaskScpiGetString::create(pcbInterface,
                                     QString("SENSE:%1:UNIT?").arg(channelName),
                                     valueReceived,
                                     timeout,
                                     additionalErrorHandler);
}
