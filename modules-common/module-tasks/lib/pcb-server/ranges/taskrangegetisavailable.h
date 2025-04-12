#ifndef TASKRANGEGETISAVAILABLE_H
#define TASKRANGEGETISAVAILABLE_H

#include <pcbinterface.h>
#include <tasktemplate.h>

class TaskRangeGetIsAvailable
{
public:
    static TaskTemplatePtr create(Zera::PcbInterfacePtr pcbInterface,
                                  QString channelMName, QString rangeName,
                                  std::shared_ptr<bool> valueReceived,
                                  int timeout, std::function<void ()> additionalErrorHandler = []{});
};

#endif // TASKRANGEGETISAVAILABLE_H
