#ifndef TASKGENERATORRANGEBYENUMSET_H
#define TASKGENERATORRANGEBYENUMSET_H

#include <abstractserverInterface.h>
#include <tasktemplate.h>

class TaskGeneratorRangeByEnumSet
{
public:
    static TaskTemplatePtr create(AbstractServerInterfacePtr interface,
                                  const QString &channelMName, int range,
                                  std::function<void()> additionalErrorHandler = []{}, int timeout = TRANSACTION_TIMEOUT);
};

#endif // TASKGENERATORRANGEBYENUMSET_H
