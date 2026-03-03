#ifndef TASKEMOBFLIPSWITCH_H
#define TASKEMOBFLIPSWITCH_H

#include <pcbinterface.h>
#include <taskservertransactiontemplate.h>

class TaskEmobFlipSwitch : public TaskServerTransactionTemplate
{
public:
    static TaskTemplatePtr create(Zera::PcbInterfacePtr pcbInterface,
                                  QString channelMName, bool onOff,
                                  int timeout,
                                  std::function<void()> additionalErrorHandler = []{});
};

#endif // TASKEMOBFLIPSWITCH_H
