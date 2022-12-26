#include "tasktimeoutdecorator.h"
#include "taskextraerrorhandler.h"

TaskCompositePtr TaskTimeoutDecorator::wrapTimeout(int timeout, TaskCompositePtr decoratedTask, std::function<void ()> additionalErrorHandler)
{
    return TaskExtraErrorHandler::create(
                std::make_unique<TaskTimeoutDecorator>(
                    std::move(decoratedTask),
                    timeout),
                additionalErrorHandler);
}

TaskTimeoutDecorator::TaskTimeoutDecorator(TaskCompositePtr decoratedTask, int timeout) :
    m_decoratedTask(std::move(decoratedTask)),
    m_timeoutMs(timeout)
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
    if(m_timeoutMs) {
        connect(&m_timer, &QTimer::timeout, this, &TaskTimeoutDecorator::onTimeout);
        m_timer.start(m_timeoutMs);
    }
    connect(m_decoratedTask.get(), &TaskComposite::sigFinish, this, &TaskTimeoutDecorator::onFinishDecorated);
    m_decoratedTask->start();
}

void TaskTimeoutDecorator::emitFinish(bool ok)
{
    m_decoratedTask = nullptr;
    m_timer.stop();
    finishTask(ok);
}
