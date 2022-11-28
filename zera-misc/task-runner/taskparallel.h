#ifndef TASKPARALLEL_H
#define TASKPARALLEL_H

#include "taskcomposit.h"

class TaskParallel : public TaskComposite
{
    Q_OBJECT
public:
    void addTask(TaskComposite* task);
    void start() override;
};

#endif // TASKPARALLEL_H
