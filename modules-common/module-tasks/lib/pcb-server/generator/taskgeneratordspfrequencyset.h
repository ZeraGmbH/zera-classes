#ifndef TASKGENERATORDSPFREQUENCYSET_H
#define TASKGENERATORDSPFREQUENCYSET_H

#include <abstractserverInterface.h>
#include <tasktemplate.h>

class TaskGeneratorDspFrequencySet
{
public:
    static TaskTemplatePtr create(AbstractServerInterfacePtr interface,
                                  const QString &channelMName, float frequency,
                                  std::function<void()> additionalErrorHandler = []{}, int timeout = TRANSACTION_TIMEOUT);
};

#endif // TASKGENERATORDSPFREQUENCYSET_H
