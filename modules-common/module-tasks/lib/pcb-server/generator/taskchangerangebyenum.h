#ifndef TASKCHANGERANGEBYENUM_H
#define TASKCHANGERANGEBYENUM_H

#include "taskscpicmd.h"

class TaskChangeRangeByEnum : public TaskScpiCmd
{
    Q_OBJECT
public:
    static TaskTemplatePtr create(AbstractServerInterfacePtr interface,
                                  const QString &channelMName, int range,
                                  int timeout, std::function<void()> additionalErrorHandler = []{});
};

#endif // TASKCHANGERANGEBYENUM_H
