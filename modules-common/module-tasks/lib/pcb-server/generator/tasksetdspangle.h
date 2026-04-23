#ifndef TASKSETDSPANGLE_H
#define TASKSETDSPANGLE_H

#include <abstractserverInterface.h>
#include <tasktemplate.h>

class TaskSetDspAngle
{
public:
    static TaskTemplatePtr create(AbstractServerInterfacePtr interface,
                                  const QString &channelMName, float angle,
                                  std::function<void()> additionalErrorHandler = []{}, int timeout = TRANSACTION_TIMEOUT);
};

#endif // TASKSETDSPANGLE_H
