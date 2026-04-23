#ifndef TASKGENERATORDSPFREQUENCYGET_H
#define TASKGENERATORDSPFREQUENCYGET_H

#include <abstractserverInterface.h>
#include <tasktemplate.h>

class TaskGeneratorDspFrequencyGet
{
public:
    static TaskTemplatePtr create(AbstractServerInterfacePtr interface,
                                  const QString &channelMName,
                                  std::shared_ptr<double> frequencyReceived,
                                  std::function<void()> additionalErrorHandler = []{}, int timeout = TRANSACTION_TIMEOUT);
};

#endif // TASKGENERATORDSPFREQUENCYGET_H
