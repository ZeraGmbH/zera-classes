#include "taskfortest.h"
#include <singleshottimertest.h>

TaskCompositePtr TaskForTest::create(int delayMs, bool finishOk)
{
    return std::make_unique<TaskForTest>(delayMs, finishOk);
}

int TaskForTest::m_finishOkCount = 0;
int TaskForTest::m_finishErrCount = 0;
int TaskForTest::m_dtorCount = 0;

TaskForTest::TaskForTest(int delayMs, bool finishOk) :
    m_delayMs(delayMs),
    m_delayTimer(std::make_unique<SingleShotTimerTest>(delayMs)),
    m_finishOk(finishOk)
{
    connect(m_delayTimer.get(), &ZeraTimerTemplate::sigExpired,
            this, &TaskForTest::doEmit);
}

TaskForTest::~TaskForTest()
{
    m_dtorCount++;
}

void TaskForTest::start()
{
    if(m_started)
        qFatal("Started Twice");
    m_started = true;
    if(m_delayMs <= 0)
        doEmit();
    else
        m_delayTimer->start();
}

void TaskForTest::resetCounters()
{
    m_finishOkCount = 0;
    m_finishErrCount = 0;
    m_dtorCount = 0;

}

void TaskForTest::doEmit()
{
    if(m_finishOk)
        m_finishOkCount++;
    else
        m_finishErrCount++;
    finishTask(m_finishOk);
}
