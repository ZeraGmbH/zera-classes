#ifndef TASKRANGEGETADCREJECTION_H
#define TASKRANGEGETADCREJECTION_H

#include <pcbinterface.h>
#include <tasktemplate.h>

class TaskRangeGetAdcRejection
{
public:
    static TaskTemplatePtr create(Zera::PcbInterfacePtr pcbInterface,
                                  QString channelMName, QString rangeName,
                                  std::shared_ptr<double> valueReceived,
                                  int timeout, std::function<void()> additionalErrorHandler = []{});
};

#endif // TASKRANGEGETADCREJECTION_H
