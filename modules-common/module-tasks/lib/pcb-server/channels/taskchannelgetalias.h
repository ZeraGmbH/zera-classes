#ifndef TASKCHANNELGETALIAS_H
#define TASKCHANNELGETALIAS_H

#include "taskservertransactiontemplate.h"

class TaskChannelGetAlias : public TaskServerTransactionTemplate
{
    Q_OBJECT
public:
    static TaskTemplatePtr create(const AbstractServerInterfacePtr &pcbInterface,
                                  const QString &channelName,
                                  std::shared_ptr<QString> aliasReceived,
                                  int timeout, std::function<void()> additionalErrorHandler = []{});
};

#endif // TASKCHANNELGETALIAS_H
