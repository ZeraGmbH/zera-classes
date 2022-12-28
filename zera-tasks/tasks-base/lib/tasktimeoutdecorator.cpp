#include "tasktimeoutdecorator.h"
#include "taskextraerrorhandler.h"
#include "singleshottimerqt.h"

TaskCompositePtr TaskTimeoutDecorator::wrapTimeout(int timeout, TaskCompositePtr decoratedTask,
                                                   std::function<void ()> additionalErrorHandler)
{
    return TaskExtraErrorHandler::create(
                std::make_unique<TaskTimeoutDecorator>(
                    SingleShotTimerQt::create(timeout),
                    std::move(decoratedTask)),
                additionalErrorHandler);
}

TaskCompositePtr TaskTimeoutDecorator::wrapTimeout(ZeraTimerTemplatePtr timer, TaskCompositePtr decoratedTask,
                                                   std::function<void ()> additionalErrorHandler)
{
    return TaskExtraErrorHandler::create(
                std::make_unique<TaskTimeoutDecorator>(
                    std::move(timer),
                    std::move(decoratedTask)),
                additionalErrorHandler);
}

TaskTimeoutDecorator::TaskTimeoutDecorator(ZeraTimerTemplatePtr timer, TaskCompositePtr decoratedTask) :
    m_decoratedTask(std::move(decoratedTask)),
    m_timer(std::move(timer))
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
    m_timer->stop();
    emitFinish(ok);
}

void TaskTimeoutDecorator::onTimeout()
{
    emitFinish(false);
}

void TaskTimeoutDecorator::startDecoratedTask()
{
    connect(m_timer.get(), &ZeraTimerTemplate::sigExpired,
            this, &TaskTimeoutDecorator::onTimeout);
    m_timer->start();
    connect(m_decoratedTask.get(), &TaskComposite::sigFinish, this, &TaskTimeoutDecorator::onFinishDecorated);
    m_decoratedTask->start();
}

void TaskTimeoutDecorator::emitFinish(bool ok)
{
    m_decoratedTask = nullptr;
    m_timer->stop();
    finishTask(ok);
}
