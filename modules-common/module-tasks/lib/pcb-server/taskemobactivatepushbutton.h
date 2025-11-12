#ifndef TASKEMOBACTIVATEPUSHBUTTON_H
#define TASKEMOBACTIVATEPUSHBUTTON_H

#include <pcbinterface.h>
#include <taskservertransactiontemplate.h>

class TaskEmobActivatePushButton : public TaskServerTransactionTemplate
{
public:
    static TaskTemplatePtr create(Zera::PcbInterfacePtr pcbInterface,
                                  int timeout,
                                  std::function<void()> additionalErrorHandler = []{});
};

#endif // TASKEMOBACTIVATEPUSHBUTTON_H
