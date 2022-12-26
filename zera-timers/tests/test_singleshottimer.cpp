#include "test_singleshottimer.h"
#include "singleshottimerqt.h"
#include <QTest>

QTEST_MAIN(test_singleshottimer)

constexpr int defaultExpireMs = 10;

void test_singleshottimer::init()
{
    m_expireCount = 0;
    m_expireTime = 0;
    m_elapsedTimer = std::make_unique<QElapsedTimer>();
    m_elapsedTestTimerRunner = std::make_unique<TimerRunnerForTest>();
}

void test_singleshottimer::inspectTimerByDelay(ZeraTimerTemplate *timer)
{
    m_elapsedTimer->start();
    connect(timer, &SingleShotTimerQt::sigExpired, [&]{
        m_expireCount++;
        m_expireTime = m_elapsedTimer->elapsed();
    });
}

void test_singleshottimer::inspectTimerByRunner(SingleShotTimerTest *timer)
{
    timer->setRunner(m_elapsedTestTimerRunner.get());
    connect(timer, &SingleShotTimerQt::sigExpired, [&]{
        m_expireCount++;
        m_expireTime = m_elapsedTestTimerRunner->getCurrentTimeMs();
    });
}

void test_singleshottimer::signalOnExpireTiming()
{
    SingleShotTimerQt timer(defaultExpireMs);
    timer.setHighAccuracy(true);
    inspectTimerByDelay(&timer);
    timer.start();

    QTest::qWait(defaultExpireMs+1);

    QCOMPARE(m_expireCount, 1);
    QCOMPARE(m_expireTime, defaultExpireMs);
}

void test_singleshottimer::signalOnExpireTimingTest()
{
    SingleShotTimerTest timer(defaultExpireMs);
    inspectTimerByRunner(&timer);
    timer.start();

    m_elapsedTestTimerRunner->processTimers(defaultExpireMs+1);
    QCOMPARE(m_expireCount, 1);
    QCOMPARE(m_expireTime, defaultExpireMs);
}

void test_singleshottimer::restartTiming()
{
    SingleShotTimerQt timer(defaultExpireMs);
    timer.setHighAccuracy(true);
    inspectTimerByDelay(&timer);
    timer.start();

    QTest::qWait(defaultExpireMs/2);
    timer.start();
    QTest::qWait(defaultExpireMs+1);

    QCOMPARE(m_expireCount, 1);
    QCOMPARE(m_expireTime, defaultExpireMs*1.5);
}

void test_singleshottimer::restartTimingTest()
{
    SingleShotTimerTest timer(defaultExpireMs);
    inspectTimerByRunner(&timer);
    timer.start();

    m_elapsedTestTimerRunner->processTimers(defaultExpireMs/2);
    timer.start();
    m_elapsedTestTimerRunner->processTimers(defaultExpireMs+1);

    QCOMPARE(m_expireCount, 1);
    QCOMPARE(m_expireTime, defaultExpireMs*1.5);
}

void test_singleshottimer::stopWhilePending()
{
    SingleShotTimerQt timer(defaultExpireMs);
    timer.setHighAccuracy(true);
    inspectTimerByDelay(&timer);
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

