#ifndef TASKCHANGERANGEBYENUM_H
#define TASKCHANGERANGEBYENUM_H

#include <abstractserverInterface.h>
#include <tasktemplate.h>

class TaskChangeRangeByEnum
{
public:
    static TaskTemplatePtr create(AbstractServerInterfacePtr interface,
                                  const QString &channelMName, int range,
                                  std::function<void()> additionalErrorHandler = []{}, int timeout = TRANSACTION_TIMEOUT);
};

#endif // TASKCHANGERANGEBYENUM_H
