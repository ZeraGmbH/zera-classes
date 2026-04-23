#ifndef TASKGENERATORRANGEBYENUMGET_H
#define TASKGENERATORRANGEBYENUMGET_H

#include <abstractserverInterface.h>
#include <tasktemplate.h>

class TaskGeneratorRangeByEnumGet
{
public:
    static TaskTemplatePtr create(AbstractServerInterfacePtr interface,
                                  const QString &channelMName,
                                  std::shared_ptr<int> rangeReceived,
                                  std::function<void()> additionalErrorHandler = []{}, int timeout = TRANSACTION_TIMEOUT);
};

#endif // TASKGENERATORRANGEBYENUMGET_H
