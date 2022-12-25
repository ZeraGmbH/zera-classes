#include "test_singleshottimer.h"
#include "singleshottimerqt.h"
#include "singleshottimertest.h"
#include "timerrunnerfortest.h"
#include <QTest>

QTEST_MAIN(test_singleshottimer)

constexpr int defaultExpireMs = 10;

void test_singleshottimer::init()
{
    m_expireCount = 0;
    m_expireTime = 0;
    m_elapsedTimer = std::make_unique<QElapsedTimer>();
}

void test_singleshottimer::signalOnExpireTiming()
{
    SingleShotTimerQt timer(defaultExpireMs);
    timer.setHighAccuracy(true);
    inspectTimer(&timer);
    timer.start();

    QTest::qWait(defaultExpireMs+1);

    QCOMPARE(m_expireCount, 1);
    QCOMPARE(m_expireTime, defaultExpireMs);
}

void test_singleshottimer::signalOnExpireTimingTest()
{
    SingleShotTimerTest timer(defaultExpireMs);
    inspectTimer(&timer);
    timer.start();

    TimerRunnerForTest::getInstance()->processTimers(defaultExpireMs+1);
    QCOMPARE(m_expireCount, 1);
    QCOMPARE(m_expireTime, defaultExpireMs);
}

void test_singleshottimer::restartTiming()
{
    SingleShotTimerQt timer(defaultExpireMs);
    timer.setHighAccuracy(true);
    inspectTimer(&timer);
    timer.start();

    QTest::qWait(defaultExpireMs/2);
    timer.start();
    QTest::qWait(defaultExpireMs+1);

    QCOMPARE(m_expireCount, 1);
    QCOMPARE(m_expireTime, defaultExpireMs*1.5);
}

void test_singleshottimer::restartTimingTest()
{

}

void test_singleshottimer::stopWhilePending()
{
    SingleShotTimerQt timer(defaultExpireMs);
    timer.setHighAccuracy(true);
    inspectTimer(&timer);
    timer.start();

    QTest::qWait(defaultExpireMs/2);
    timer.stop();
    QTest::qWait(defaultExpireMs+1);

    QCOMPARE(m_expireCount, 0);
    QCOMPARE(m_expireTime, 0);

}

void test_singleshottimer::stopWhilePendingTest()
{

}

void test_singleshottimer::queuedConnectetionsOnExpire()
{

}

void test_singleshottimer::queuedConnectetionsOnExpireTest()
{

}

void test_singleshottimer::inspectTimer(ZeraTimerTemplate *timer)
{
    m_elapsedTimer->start();
    connect(timer, &SingleShotTimerQt::sigExpired, [&]{
        m_expireCount++;
        m_expireTime = m_elapsedTimer->elapsed();
    });
}
