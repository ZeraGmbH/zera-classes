#ifndef TASKGETSAMPLERATE_H
#define TASKGETSAMPLERATE_H

#include "taskservertransactiontemplate.h"
#include <pcbinterface.h>

class TaskGetSampleRate : public TaskServerTransactionTemplate
{
    Q_OBJECT
public:
    static TaskTemplatePtr create(Zera::PcbInterfacePtr pcbInterface,
                                  std::shared_ptr<int> valueReceived,
                                  int timeout, std::function<void()> additionalErrorHandler = []{});
};

#endif // TASKGETSAMPLERATE_H
