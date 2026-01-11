#ifndef TASKPLLCHANNELGET_H
#define TASKPLLCHANNELGET_H

#include <taskservertransactiontemplate.h>
#include <abstractserverInterface.h>

class TaskPllChannelGet : public TaskServerTransactionTemplate
{
    Q_OBJECT
public:
    static TaskTemplatePtr create(AbstractServerInterfacePtr pcbInterface,
                                  std::shared_ptr<QString> &pllChannelMNameReceived,
                                  int timeout, std::function<void()> additionalErrorHandler = []{});
};

#endif // TASKPLLCHANNELGET_H
