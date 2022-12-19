#include "tasktesthelper.h"

TaskTestHelper::TaskTestHelper(TaskComposite *taskUnderTest)
{
    connect(taskUnderTest, &TaskComposite::sigFinish, this, &TaskTestHelper::onTaskFinish);
    m_elapsedTimer.start();
}

void TaskTestHelper::onTaskFinish(bool ok, int taskId)
{
    if(ok)
        m_okCount++;
    else
        m_errCount++;
    m_signalDelay = m_elapsedTimer.elapsed();
    m_taskIdsReceived.append(taskId);
}
