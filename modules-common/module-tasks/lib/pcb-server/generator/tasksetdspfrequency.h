#ifndef TASKSETDSPFREQUENCY_H
#define TASKSETDSPFREQUENCY_H

#include "taskscpicmd.h"

class TaskSetDspFrequency : public TaskScpiCmd
{
    Q_OBJECT
public:
    static TaskTemplatePtr create(AbstractServerInterfacePtr interface,
                                  const QString &channelMName, float frequency,
                                  int timeout = TRANSACTION_TIMEOUT, std::function<void()> additionalErrorHandler = []{});
};

#endif // TASKSETDSPFREQUENCY_H
