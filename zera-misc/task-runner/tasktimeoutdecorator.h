#ifndef TASKTIMEOUTDECORATOR_H
#define TASKTIMEOUTDECORATOR_H

#include "taskcomposit.h"

class TaskTimeoutDecorator : public TaskComposite
{
    Q_OBJECT
public:
    explicit TaskTimeoutDecorator(TaskComposite* decoratedTask, int timeout, QObject *parent=nullptr);
    void start() override;
};

#endif // TASKTIMEOUTDECORATOR_H
