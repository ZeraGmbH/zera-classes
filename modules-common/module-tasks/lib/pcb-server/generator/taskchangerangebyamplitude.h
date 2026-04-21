#ifndef TASKCHANGERANGEBYAMPLITUDE_H
#define TASKCHANGERANGEBYAMPLITUDE_H

#include "taskscpicmd.h"

class TaskChangeRangeByAmplitude : public TaskScpiCmd
{
    Q_OBJECT
public:
    static TaskTemplatePtr create(AbstractServerInterfacePtr interface,
                                  const QString &channelMName, float amplitude,
                                  int timeout, std::function<void()> additionalErrorHandler = []{});
};

#endif // TASKCHANGERANGEBYAMPLITUDE_H
