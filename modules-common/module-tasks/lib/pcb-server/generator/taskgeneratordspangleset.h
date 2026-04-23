#ifndef TASKGENERATORDSPANGLESET_H
#define TASKGENERATORDSPANGLESET_H

#include <abstractserverInterface.h>
#include <tasktemplate.h>

class TaskGeneratorDspAngleSet
{
public:
    static TaskTemplatePtr create(AbstractServerInterfacePtr interface,
                                  const QString &channelMName, float angle,
                                  std::function<void()> additionalErrorHandler = []{}, int timeout = TRANSACTION_TIMEOUT);
};

#endif // TASKGENERATORDSPANGLESET_H
