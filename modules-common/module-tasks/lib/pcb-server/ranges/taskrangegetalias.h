#ifndef TASKRANGEGETALIAS_H
#define TASKRANGEGETALIAS_H

#include <pcbinterface.h>
#include <tasktemplate.h>

class TaskRangeGetAlias
{
public:
    static TaskTemplatePtr create(const Zera::PcbInterfacePtr &pcbInterface,
                                  const QString &channelMName, const QString &rangeName,
                                  std::shared_ptr<QString> valueReceived,
                                  int timeout, std::function<void()> additionalErrorHandler = []{});
};

#endif // TASKRANGEGETALIAS_H
