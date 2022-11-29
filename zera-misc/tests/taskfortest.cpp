#include "taskfortest.h"
#include <QTest>

TaskInterfacePtr TaskForTest::create(int delayMs, bool finishError)
{
    return std::make_unique<TaskForTest>(delayMs, finishError);
}

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
    emit sigFinish(!m_finishError);
}
