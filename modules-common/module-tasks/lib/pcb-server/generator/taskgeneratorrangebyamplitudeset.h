#ifndef TASKGENERATORRANGEBYAMPLITUDESET_H
#define TASKGENERATORRANGEBYAMPLITUDESET_H

#include <abstractserverInterface.h>
#include <tasktemplate.h>

class TaskGeneratorRangeByAmplitudeSet
{
public:
    static TaskTemplatePtr create(AbstractServerInterfacePtr interface,
                                  const QString &channelMName, float amplitude,
                                  std::function<void()> additionalErrorHandler = []{}, int timeout = TRANSACTION_TIMEOUT);
};

#endif // TASKGENERATORRANGEBYAMPLITUDESET_H
