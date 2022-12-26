#include "tasktesthelper.h"
#include <timerrunnerfortest.h>

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
    m_signalDelayMs = m_elapsedTimer.elapsed();
    m_taskIdsReceived.append(taskId);
}

TaskTestHelperNew::TaskTestHelperNew(TaskComposite *taskUnderTest)
{
    connect(taskUnderTest, &TaskComposite::sigFinish, this, &TaskTestHelperNew::onTaskFinish);
}

void TaskTestHelperNew::onTaskFinish(bool ok, int taskId)
{
    if(ok)
        m_okCount++;
    else
        m_errCount++;
    m_signalDelayMs = TimerRunnerForTest::getInstance()->getCurrentTimeMs();
    m_taskIdsReceived.append(taskId);
}
