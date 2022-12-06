#include "taskcomposit.h"

TaskComposite::TaskComposite()
{
    m_taskId = getNextTaskId();
}

int TaskComposite::getTaskId() const
{
    return m_taskId;
}

void TaskComposite::finishTask(bool ok)
{
    emit sigFinish(ok, m_taskId);
}

int TaskComposite::m_taskIdForNext = 0;

int TaskComposite::getNextTaskId()
{
    return ++m_taskIdForNext;
}

