#include "taskrangegetalias.h"
#include "taskscpigetstring.h"

TaskTemplatePtr TaskRangeGetAlias::create(const Zera::PcbInterfacePtr &pcbInterface,
                                          const QString &channelMName,
                                          const QString &rangeName,
                                          std::shared_ptr<QString> valueReceived,
                                          int timeout, std::function<void ()> additionalErrorHandler)
{
    return TaskScpiGetString::create(pcbInterface,
                                     QString("SENSE:%1:%2:ALIAS?").arg(channelMName, rangeName),
                                     valueReceived,
                                     timeout,
                                     additionalErrorHandler);
}
