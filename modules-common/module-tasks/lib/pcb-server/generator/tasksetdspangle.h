#ifndef TASKSETDSPANGLE_H
#define TASKSETDSPANGLE_H

#include "taskscpicmd.h"

class TaskSetDspAngle : public TaskScpiCmd
{
    Q_OBJECT
public:
    static TaskTemplatePtr create(AbstractServerInterfacePtr interface,
                                  const QString &channelMName, float angle,
                                  int timeout, std::function<void()> additionalErrorHandler = []{});
};

#endif // TASKSETDSPANGLE_H
