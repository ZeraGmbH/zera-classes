#ifndef TASKPLLCHANNELSET_H
#define TASKPLLCHANNELSET_H

#include <taskservertransactiontemplate.h>
#include <abstractserverInterface.h>

class TaskPllChannelSet : public TaskServerTransactionTemplate
{
    Q_OBJECT
public:
    static TaskTemplatePtr create(AbstractServerInterfacePtr pcbInterface,
                                  const QString &pllChannelMName,
                                  int timeout, std::function<void()> additionalErrorHandler = []{});
};

#endif // TASKPLLCHANNELSET_H
