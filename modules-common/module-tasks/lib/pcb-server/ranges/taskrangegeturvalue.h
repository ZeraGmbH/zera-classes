#ifndef TASKRANGEGETURVALUE_H
#define TASKRANGEGETURVALUE_H

#include <pcbinterface.h>
#include <tasktemplate.h>

class TaskRangeGetUrValue
{
public:
    static TaskTemplatePtr create(Zera::PcbInterfacePtr pcbInterface,
                                  QString channelMName, QString rangeName,
                                  std::shared_ptr<double> valueReceived,
                                  int timeout, std::function<void()> additionalErrorHandler = []{});
};

#endif // TASKRANGEGETURVALUE_H
