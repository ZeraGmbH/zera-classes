#include "taskchangerangebyenum.h"

TaskTemplatePtr TaskChangeRangeByEnum::create(AbstractServerInterfacePtr interface,
                                              const QString &channelMName, int range,
                                              int timeout, std::function<void ()> additionalErrorHandler)
{
    QString cmd = QString("GENERATOR:%1:RANGE %2;").arg(channelMName).arg(range);
    return TaskScpiCmd::create(interface,
                               cmd,
                               timeout,
                               additionalErrorHandler);
}
