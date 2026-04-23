#ifndef TASKCHANGERANGEBYENUM_H
#define TASKCHANGERANGEBYENUM_H

#include "taskscpicmd.h"

class TaskChangeRangeByEnum : public TaskScpiCmd
{
    Q_OBJECT
public:
    static TaskTemplatePtr create(AbstractServerInterfacePtr interface,
                                  const QString &channelMName, int range,
                                  std::function<void()> additionalErrorHandler = []{}, int timeout = TRANSACTION_TIMEOUT);
};

#endif // TASKCHANGERANGEBYENUM_H
