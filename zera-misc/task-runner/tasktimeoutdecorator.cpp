#include "tasktimeoutdecorator.h"

std::unique_ptr<TaskTimeoutDecorator> TaskTimeoutDecorator::create(TaskInterfacePtr decoratedTask, int timeout)
{
    return std::make_unique<TaskTimeoutDecorator>(std::move(decoratedTask), timeout);
}

TaskTimeoutDecorator::TaskTimeoutDecorator(TaskInterfacePtr decoratedTask, int timeout)
{

}

void TaskTimeoutDecorator::start()
{
}
