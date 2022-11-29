#include "tasktimeoutdecorator.h"

std::unique_ptr<TaskTimeoutDecorator> TaskTimeoutDecorator::create(TaskInterfacePtr decoratedTask, int timeout)
{
    return std::make_unique<TaskTimeoutDecorator>(std::move(decoratedTask), timeout);
}

TaskTimeoutDecorator::TaskTimeoutDecorator(TaskInterfacePtr decoratedTask, int timeout) :
    m_decoratedTask(std::move(decoratedTask)),
    m_timeout(timeout)
{
    if(m_decoratedTask)
        connect(m_decoratedTask.get(), &TaskComposite::sigFinish, this, &TaskTimeoutDecorator::onFinishDecorated);
}

void TaskTimeoutDecorator::start()
{
    if(m_decoratedTask)
        m_decoratedTask->start();
    else
        emit sigFinish(true);
}

void TaskTimeoutDecorator::onFinishDecorated(bool ok)
{
    m_decoratedTask = nullptr;
    emit sigFinish(ok);
}
