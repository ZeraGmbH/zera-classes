#ifndef TASKEMOBDISCHARGE_H
#define TASKEMOBDISCHARGE_H

#include <pcbinterface.h>
#include <taskservertransactiontemplate.h>

class TaskEmobDischarge : public TaskServerTransactionTemplate
{
public:
    static TaskTemplatePtr create(Zera::PcbInterfacePtr pcbInterface,
                                  QString channelMName, bool onOff,
                                  int timeout,
                                  std::function<void()> additionalErrorHandler = []{});
};

#endif // TASKEMOBDISCHARGE_H
