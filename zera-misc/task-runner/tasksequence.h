#ifndef TASKSEQUENCE_H
#define TASKSEQUENCE_H

#include "taskcomposit.h"

class TaskSequence : public TaskComposite // for now abort on error
{
    Q_OBJECT
public:
    void addTask(TaskComposite* task);
    void start() override;
};

#endif // TASKSEQUENCE_H
