#ifndef TASKCONTAINER_H
#define TASKCONTAINER_H

#include "taskcomposit.h"

class TaskContainer : public TaskComposite
{
public:
    virtual void addSub(TaskCompositePtr task) = 0;
};

typedef std::unique_ptr<TaskContainer> TaskContainerPtr;

#endif // TASKCONTAINER_H
