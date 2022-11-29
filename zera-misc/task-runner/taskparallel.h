#ifndef TASKPARALLEL_H
#define TASKPARALLEL_H

#include "taskcomposit.h"

class TaskParallel : public TaskComposite
{
    Q_OBJECT
public:
    static std::unique_ptr<TaskParallel> create();
    TaskParallel();
    void addTask(TaskInterfacePtr task);
    void start() override;
};

typedef std::unique_ptr<TaskParallel> TaskParallelPtr;

#endif // TASKPARALLEL_H
