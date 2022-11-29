#include "taskparallel.h"


std::unique_ptr<TaskParallel> TaskParallel::create()
{
    return std::make_unique<TaskParallel>();
}

void TaskParallel::addTask(TaskInterfacePtr task)
{

}

void TaskParallel::start()
{

}

TaskParallel::TaskParallel()
{

}
