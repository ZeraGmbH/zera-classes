#ifndef TASKSETPLLCHANNEL_H
#define TASKSETPLLCHANNEL_H

#include <taskservertransactiontemplate.h>
#include <pcbinterface.h>

class TaskSetPllChannel : public TaskServerTransactionTemplate
{
    Q_OBJECT
public:
    static TaskTemplatePtr create(Zera::PcbInterfacePtr pcbInterface,
                                  const QString &pllChannelMName,
                                  int timeout, std::function<void()> additionalErrorHandler = []{});
};

#endif // TASKSETPLLCHANNEL_H
