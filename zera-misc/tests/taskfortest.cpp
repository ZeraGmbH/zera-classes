#include "taskfortest.h"
#include <QTest>

TaskForTest::TaskForTest(int delayMs, bool finishError) :
    m_delayMs(delayMs),
    m_finishError(finishError)
{
    connect(&m_delayTimer, &QTimer::timeout, this, &TaskForTest::doEmit);
}

void TaskForTest::start()
{
    QVERIFY(!m_started);
    m_started = true;
    if(m_delayMs <= 0)
        doEmit();
    else
        m_delayTimer.start(m_delayMs);
}

void TaskForTest::doEmit()
{
    if(!m_finishError)
        emit finishOk();
    else
        emit finishErr();
}
