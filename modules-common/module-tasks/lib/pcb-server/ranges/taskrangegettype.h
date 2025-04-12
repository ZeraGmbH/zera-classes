#ifndef TASKRANGEGETTYPE_H
#define TASKRANGEGETTYPE_H

#include <pcbinterface.h>
#include <tasktemplate.h>

class TaskRangeGetType
{
public:
    static TaskTemplatePtr create(Zera::PcbInterfacePtr pcbInterface,
                                  QString channelMName, QString rangeName,
                                  std::shared_ptr<int> valueReceived,
                                  int timeout, std::function<void()> additionalErrorHandler = []{});
};

#endif // TASKRANGEGETTYPE_H
