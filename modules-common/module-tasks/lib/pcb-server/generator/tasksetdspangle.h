#ifndef TASKSETDSPANGLE_H
#define TASKSETDSPANGLE_H

#include "taskscpicmd.h"

class TaskSetDspAngle : public TaskScpiCmd
{
    Q_OBJECT
public:
    static TaskTemplatePtr create(AbstractServerInterfacePtr interface,
                                  const QString &channelMName, float angle,
                                  std::function<void()> additionalErrorHandler = []{}, int timeout = TRANSACTION_TIMEOUT);
};

#endif // TASKSETDSPANGLE_H
