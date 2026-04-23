#ifndef TASKSETDSPAMPLITUDE_H
#define TASKSETDSPAMPLITUDE_H

#include <abstractserverInterface.h>
#include <tasktemplate.h>

class TaskSetDspAmplitude
{
public:
    static TaskTemplatePtr create(AbstractServerInterfacePtr interface,
                                  const QString &channelMName, float amplitude,
                                  std::function<void()> additionalErrorHandler = []{}, int timeout = TRANSACTION_TIMEOUT);
};

#endif // TASKSETDSPAMPLITUDE_H
