#include "taskcomposit.h"

TaskComposite::TaskComposite()
{
    m_taskId = getNextTaskId();
}

void TaskComposite::setAdditionalErrorHandler(std::function<void ()> additionalErrorHandler)
{
    m_additionalErrorHandler = additionalErrorHandler;
}

int TaskComposite::getTaskId() const
{
    return m_taskId;
}

void TaskComposite::finishTask(bool ok)
{
    if(!ok)
        m_additionalErrorHandler();
    emit sigFinish(ok, m_taskId);
}

int TaskComposite::m_taskIdForNext = 0;

int TaskComposite::getNextTaskId()
{
    return ++m_taskIdForNext;
}

