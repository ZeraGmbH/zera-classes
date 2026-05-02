#ifndef TASKRANGEGETREJECTION_H
#define TASKRANGEGETREJECTION_H

#include <pcbinterface.h>
#include <tasktemplate.h>

class TaskRangeGetRejection
{
public:
    static TaskTemplatePtr create(const Zera::PcbInterfacePtr &pcbInterface,
                                  const QString &channelMName, const QString &rangeName,
                                  std::shared_ptr<double> valueReceived,
                                  int timeout, std::function<void()> additionalErrorHandler = []{});
};

#endif // TASKRANGEGETREJECTION_H
