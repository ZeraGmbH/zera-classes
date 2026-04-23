#ifndef TASKGENERATORDSPANGLEGET_H
#define TASKGENERATORDSPANGLEGET_H

#include <abstractserverInterface.h>
#include <tasktemplate.h>

class TaskGeneratorDspAngleGet
{
public:
    static TaskTemplatePtr create(AbstractServerInterfacePtr interface,
                                  const QString &channelMName,
                                  std::shared_ptr<double> angleReceived,
                                  std::function<void()> additionalErrorHandler = []{}, int timeout = TRANSACTION_TIMEOUT);
};

#endif // TASKGENERATORDSPANGLEGET_H
