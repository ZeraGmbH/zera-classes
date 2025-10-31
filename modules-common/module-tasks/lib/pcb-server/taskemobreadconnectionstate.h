#ifndef TASKEMOBREADCONNECTIONSTATE_H
#define TASKEMOBREADCONNECTIONSTATE_H

#include <taskservertransactiontemplate.h>
#include <pcbinterface.h>

class TaskEmobReadConnectionState : public TaskServerTransactionTemplate
{
public:
    static TaskTemplatePtr create(Zera::PcbInterfacePtr pcbInterface,
                                  std::shared_ptr<int> stateReceived,
                                  int timeout, std::function<void()> additionalErrorHandler = []{});
};

#endif // TASKEMOBREADCONNECTIONSTATE_H
