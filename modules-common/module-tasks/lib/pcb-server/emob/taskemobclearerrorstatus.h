#ifndef TASKEMOBCLEARERRORSTATUS_H
#define TASKEMOBCLEARERRORSTATUS_H

#include <pcbinterface.h>
#include <taskservertransactiontemplate.h>

class TaskEmobClearErrorStatus : public TaskServerTransactionTemplate
{
public:
    static TaskTemplatePtr create(Zera::PcbInterfacePtr pcbInterface,
                                  QString channelMName,
                                  int timeout,
                                  std::function<void()> additionalErrorHandler = []{});
};

#endif // TASKEMOBCLEARERRORSTATUS_H
