#ifndef TASKGENERATORDSPAMPLITUDESET_H
#define TASKGENERATORDSPAMPLITUDESET_H

#include <abstractserverInterface.h>
#include <tasktemplate.h>

class TaskGeneratorDspAmplitudeSet
{
public:
    static TaskTemplatePtr create(AbstractServerInterfacePtr interface,
                                  const QString &channelMName, float amplitude,
                                  std::function<void()> additionalErrorHandler = []{}, int timeout = TRANSACTION_TIMEOUT);
};

#endif // TASKGENERATORDSPAMPLITUDESET_H
