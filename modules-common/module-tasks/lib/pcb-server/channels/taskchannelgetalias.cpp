#include "taskchannelgetalias.h"
#include "taskscpigetstring.h"

TaskTemplatePtr TaskChannelGetAlias::create(const AbstractServerInterfacePtr &pcbInterface,
                                            const QString &channelName,
                                            std::shared_ptr<QString> aliasReceived,
                                            int timeout, std::function<void ()> additionalErrorHandler)
{
    return TaskScpiGetString::create(pcbInterface,
                                     QString("SENS:%1:ALI?").arg(channelName),
                                     aliasReceived,
                                     timeout,
                                     additionalErrorHandler);
}
