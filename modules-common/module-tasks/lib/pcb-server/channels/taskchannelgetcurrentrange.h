#ifndef TASKCHANNELGETCURRENTRANGE_H
#define TASKCHANNELGETCURRENTRANGE_H

#include <taskservertransactiontemplate.h>

class TaskChannelGetCurrentRange : public TaskServerTransactionTemplate
{
    Q_OBJECT
public:
    static TaskTemplatePtr create(const AbstractServerInterfacePtr &pcbInterface,
                                  const QString &channelName,
                                  std::shared_ptr<QString> rangeReceived,
                                  int timeout, std::function<void()> additionalErrorHandler = []{});
};

#endif // TASKCHANNELGETCURRENTRANGE_H
