#ifndef TASKEMOBREADERRORSTATUS_H
#define TASKEMOBREADERRORSTATUS_H

#include <pcbinterface.h>
#include <taskservertransactiontemplate.h>

class TaskEmobReadErrorStatus : public TaskServerTransactionTemplate
{
public:
    static TaskTemplatePtr create(Zera::PcbInterfacePtr pcbInterface,
                                  std::shared_ptr<int> errorStatus,
                                  QString channelMName,
                                  int timeout,
                                  std::function<void()> additionalErrorHandler = []{});
};

#endif // TASKEMOBREADERRORSTATUS_H
