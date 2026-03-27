#ifndef TASKEMOBREADPRUEFGROESSENSTATUS_H
#define TASKEMOBREADPRUEFGROESSENSTATUS_H

#include <pcbinterface.h>
#include <taskservertransactiontemplate.h>

class TaskEmobReadPruefgroessenStatus : public TaskServerTransactionTemplate
{
public:
    static TaskTemplatePtr create(Zera::PcbInterfacePtr pcbInterface,
                                  std::shared_ptr<int> emobPruefgroessenStatus,
                                  QString channelMName,
                                  int timeout,
                                  std::function<void()> additionalErrorHandler = []{});
};

#endif // TASKEMOBREADPRUEFGROESSENSTATUS_H
