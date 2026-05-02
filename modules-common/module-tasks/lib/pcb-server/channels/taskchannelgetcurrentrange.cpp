#include "taskchannelgetcurrentrange.h"
#include "taskscpigetstring.h"

TaskTemplatePtr TaskChannelGetCurrentRange::create(const AbstractServerInterfacePtr &pcbInterface,
                                                   const QString &channelName,
                                                   std::shared_ptr<QString> rangeReceived,
                                                   int timeout, std::function<void ()> additionalErrorHandler)
{
    return TaskScpiGetString::create(pcbInterface,
                                     QString("SENS:%1:RANG?").arg(channelName),
                                     rangeReceived,
                                     timeout,
                                     additionalErrorHandler);
}
