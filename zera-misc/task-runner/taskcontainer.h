#ifndef TASKCONTAINER_H
#define TASKCONTAINER_H

#include "taskcomposit.h"

class TaskContainer : public TaskComposite
{
public:
    virtual void addSubTask(TaskCompositePtr task) = 0;
};

#endif // TASKCONTAINER_H
