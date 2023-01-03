#include "tasktesthelper.h"
#include <timerrunnerfortest.h>

TaskTestHelper::TaskTestHelper(TaskComposite *taskUnderTest)
{
    connect(taskUnderTest, &TaskComposite::sigFinish, this, &TaskTestHelper::onTaskFinish);
}

void TaskTestHelper::onTaskFinish(bool ok, int taskId)
{
    if(ok)
        m_okCount++;
    else
        m_errCount++;
    m_signalDelayMs = TimerRunnerForTest::getInstance()->getCurrentTimeMs();
    m_taskIdsReceived.append(taskId);
}
