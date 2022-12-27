#include "tasktesthelper.h"
#include <timerrunnerfortest.h>

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
