#include "tasktimeoutdecorator.h"

std::unique_ptr<TaskTimeoutDecorator> TaskTimeoutDecorator::create(TaskInterfacePtr decoratedTask, int timeout)
{
    return std::make_unique<TaskTimeoutDecorator>(std::move(decoratedTask), timeout);
}

TaskTimeoutDecorator::TaskTimeoutDecorator(TaskInterfacePtr decoratedTask, int timeout) :
    m_decoratedTask(std::move(decoratedTask)),
    m_timeout(timeout)
{
}

void TaskTimeoutDecorator::start()
{
    if(m_decoratedTask)
        startDecoratedTask();
    else
        emitFinish(true);
}

void TaskTimeoutDecorator::onFinishDecorated(bool ok)
{
    m_timer.stop();
    emitFinish(ok);
}

void TaskTimeoutDecorator::onTimeout()
{
    emitFinish(false);
}

void TaskTimeoutDecorator::startDecoratedTask()
{
    if(m_timeout) {
        connect(&m_timer, &QTimer::timeout, this, &TaskTimeoutDecorator::onTimeout);
        m_timer.start(m_timeout);
    }
    connect(m_decoratedTask.get(), &TaskComposite::sigFinish, this, &TaskTimeoutDecorator::onFinishDecorated);
    m_decoratedTask->start();
}

void TaskTimeoutDecorator::emitFinish(bool ok)
{
    m_decoratedTask = nullptr;
    m_timer.stop();
    emit sigFinish(ok, getTaskId());
}
