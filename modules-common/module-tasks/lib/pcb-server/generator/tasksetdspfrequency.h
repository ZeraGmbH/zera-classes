#ifndef TASKSETDSPFREQUENCY_H
#define TASKSETDSPFREQUENCY_H

#include <abstractserverInterface.h>
#include <tasktemplate.h>

class TaskSetDspFrequency
{
public:
    static TaskTemplatePtr create(AbstractServerInterfacePtr interface,
                                  const QString &channelMName, float frequency,
                                  std::function<void()> additionalErrorHandler = []{}, int timeout = TRANSACTION_TIMEOUT);
};

#endif // TASKSETDSPFREQUENCY_H
