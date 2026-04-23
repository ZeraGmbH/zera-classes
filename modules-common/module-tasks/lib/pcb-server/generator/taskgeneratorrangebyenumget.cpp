#include "taskgeneratorrangebyenumget.h"
#include <taskscpigetint.h>

TaskTemplatePtr TaskGeneratorRangeByEnumGet::create(AbstractServerInterfacePtr interface,
                                                    const QString &channelMName,
                                                    std::shared_ptr<int> rangeReceived,
                                                    std::function<void ()> additionalErrorHandler, int timeout)
{
    QString cmd = QString("GENERATOR:%1:RANGE?").arg(channelMName);
    return TaskScpiGetInt::create(interface,
                                  cmd,
                                  rangeReceived,
                                  timeout,
                                  additionalErrorHandler);
}
