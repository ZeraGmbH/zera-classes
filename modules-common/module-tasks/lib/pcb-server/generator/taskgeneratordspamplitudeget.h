#ifndef TASKGENERATORDSPAMPLITUDEGET_H
#define TASKGENERATORDSPAMPLITUDEGET_H

#include <abstractserverInterface.h>
#include <tasktemplate.h>

class TaskGeneratorDspAmplitudeGet
{
public:
    static TaskTemplatePtr create(AbstractServerInterfacePtr interface,
                                  const QString &channelMName,
                                  std::shared_ptr<double> amplitudeReceived,
                                  std::function<void()> additionalErrorHandler = []{}, int timeout = TRANSACTION_TIMEOUT);
};

#endif // TASKGENERATORDSPAMPLITUDEGET_H
