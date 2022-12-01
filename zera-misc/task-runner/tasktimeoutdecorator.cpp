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
    if(m_timeout)
        connect(&m_timer, &QTimer::timeout, this, &TaskTimeoutDecorator::onTimeout);
}

int TaskTimeoutDecorator::start()
{
    m_taskId = TaskComposite::getNextTaskId();
    m_timer.start(m_timeout);
    if(m_decoratedTask)
        m_decoratedTask->start();
    else
        emitFinish(true);
    return m_taskId;
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

void TaskTimeoutDecorator::emitFinish(bool ok)
{
    if(!m_emitted){
        m_decoratedTask = nullptr;
        emit sigFinish(ok, m_taskId);
        m_emitted = true;
    }
}
