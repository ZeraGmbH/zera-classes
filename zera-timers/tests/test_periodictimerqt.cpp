#include "timerrunnerfortest.h"
#include "periodictimertest.h"
#include "test_periodictimerqt.h"
#include <QTest>

QTEST_MAIN(test_periodictimerqt)

void test_periodictimerqt::init()
{
    m_expireTimes.clear();
    m_elapsedTimer = std::make_unique<QElapsedTimer>();
    TimerRunnerForTest::reset();
}

void test_periodictimerqt::inspectTimerByDelay(ZeraTimerTemplate *timer)
{
    m_elapsedTimer->start();
    connect(timer, &ZeraTimerTemplate::sigExpired, [&]{
        m_expireTimes.append(m_elapsedTimer->elapsed());
    });
}

void test_periodictimerqt::inspectTimerByRunner(ZeraTimerTemplate *timer)
{
    connect(timer, &ZeraTimerTemplate::sigExpired, [&]{
        m_expireTimes.append(TimerRunnerForTest::getInstance()->getCurrentTimeMs());
    });
}

void test_periodictimerqt::oneIntervalTest()
{
    PeriodicTimerTest timer(DEFAULT_EXPIRE);
    inspectTimerByRunner(&timer);

    timer.start();
    TimerRunnerForTest::getInstance()->processTimers(DEFAULT_EXPIRE + DEFAULT_PERIODIC_EXTRA_WAIT);

    QCOMPARE(m_expireTimes.size(), 1);
    QCOMPARE(m_expireTimes.at(0), DEFAULT_EXPIRE); // on point
}

void test_periodictimerqt::threeIntervalTest()
{
    PeriodicTimerTest timer(DEFAULT_EXPIRE);
    inspectTimerByRunner(&timer);

    timer.start();
    TimerRunnerForTest::getInstance()->processTimers(DEFAULT_EXPIRE*3 + DEFAULT_PERIODIC_EXTRA_WAIT);

    QCOMPARE(m_expireTimes.size(), 3);
    QCOMPARE(m_expireTimes.at(0), DEFAULT_EXPIRE); // on point
    QCOMPARE(m_expireTimes.at(1), DEFAULT_EXPIRE*2);
    QCOMPARE(m_expireTimes.at(2), DEFAULT_EXPIRE*3);
}
