#ifndef TASKCHANGERANGEBYAMPLITUDE_H
#define TASKCHANGERANGEBYAMPLITUDE_H

#include <abstractserverInterface.h>
#include <tasktemplate.h>

class TaskChangeRangeByAmplitude
{
public:
    static TaskTemplatePtr create(AbstractServerInterfacePtr interface,
                                  const QString &channelMName, float amplitude,
                                  std::function<void()> additionalErrorHandler = []{}, int timeout = TRANSACTION_TIMEOUT);
};

#endif // TASKCHANGERANGEBYAMPLITUDE_H
