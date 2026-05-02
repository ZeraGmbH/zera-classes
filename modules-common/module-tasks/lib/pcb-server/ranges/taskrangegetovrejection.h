#ifndef TASKRANGEGETOVREJECTION_H
#define TASKRANGEGETOVREJECTION_H

#include <pcbinterface.h>
#include <tasktemplate.h>

class TaskRangeGetOvRejection
{
public:
    static TaskTemplatePtr create(const Zera::PcbInterfacePtr &pcbInterface,
                                  const QString &channelMName, const QString &rangeName,
                                  std::shared_ptr<double> valueReceived,
                                  int timeout, std::function<void()> additionalErrorHandler = []{});
};

#endif // TASKRANGEGETOVREJECTION_H
