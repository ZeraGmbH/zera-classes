#include "test_singleshottimer.h"
#include "singleshottimerqt.h"
#include "timertestdefaults.h"
#include <QTest>

QTEST_MAIN(test_singleshottimer)

static bool isExpireTimeWithinLimits(int measuredTime, int expectedTime)
{
    int minAllowed = expectedTime;
    int maxAllowed = expectedTime * 1.5;
    bool ok = measuredTime>=minAllowed && measuredTime<=maxAllowed;
    if(!ok) {
        qWarning("Measured: %i / Expected: %i", measuredTime, expectedTime);
        qWarning("Min allowed: %i / Max allowed: %i", minAllowed, maxAllowed);
    }
    return ok;
}

void test_singleshottimer::init()
{
    m_expireCount = 0;
    m_expireTime = 0;
    m_elapsedTimer = std::make_unique<QElapsedTimer>();
    TimerRunnerForTest::reset();
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
    connect(timer, &SingleShotTimerQt::sigExpired, [&]{
        m_expireCount++;
        m_expireTime = TimerRunnerForTest::getInstance()->getCurrentTimeMs();
    });
}

void test_singleshottimer::signalOnExpireTiming()
{
    SingleShotTimerQt timer(DEFAULT_EXPIRE);
    timer.setHighAccuracy(true);
    inspectTimerByDelay(&timer);

    timer.start();
    QTest::qWait(DEFAULT_EXPIRE_WAIT);

    QCOMPARE(m_expireCount, 1);
    QVERIFY(isExpireTimeWithinLimits(m_expireTime, DEFAULT_EXPIRE)); // fuzzy
}

void test_singleshottimer::signalOnExpireTimingTest()
{
    SingleShotTimerTest timer(DEFAULT_EXPIRE);
    inspectTimerByRunner(&timer);

    timer.start();
    TimerRunnerForTest::getInstance()->processTimers(DEFAULT_EXPIRE_WAIT);

    QCOMPARE(m_expireCount, 1);
    QCOMPARE(m_expireTime, DEFAULT_EXPIRE); // on point
}

void test_singleshottimer::restartTiming()
{
    SingleShotTimerQt timer(DEFAULT_EXPIRE);
    timer.setHighAccuracy(true);
    inspectTimerByDelay(&timer);

    timer.start();
    QTest::qWait(DEFAULT_EXPIRE/2);
    timer.start();
    QTest::qWait(DEFAULT_EXPIRE_WAIT);

    QCOMPARE(m_expireCount, 1);
    QVERIFY(isExpireTimeWithinLimits(m_expireTime, DEFAULT_EXPIRE*1.5)); // fuzzy
}

void test_singleshottimer::restartTimingTest()
{
    SingleShotTimerTest timer(DEFAULT_EXPIRE);
    inspectTimerByRunner(&timer);

    timer.start();
    TimerRunnerForTest::getInstance()->processTimers(DEFAULT_EXPIRE/2);
    timer.start();
    TimerRunnerForTest::getInstance()->processTimers(DEFAULT_EXPIRE_WAIT);

    QCOMPARE(m_expireCount, 1);
    QCOMPARE(m_expireTime, DEFAULT_EXPIRE*1.5); // on point
}

void test_singleshottimer::stopWhilePending()
{
    SingleShotTimerQt timer(DEFAULT_EXPIRE);
    timer.setHighAccuracy(true);
    inspectTimerByDelay(&timer);

    timer.start();
    QTest::qWait(DEFAULT_EXPIRE/2);
    timer.stop();
    QTest::qWait(DEFAULT_EXPIRE_WAIT);

    QCOMPARE(m_expireCount, 0);
    QCOMPARE(m_expireTime, 0);
}

void test_singleshottimer::stopWhilePendingTest()
{
    SingleShotTimerTest timer(DEFAULT_EXPIRE);
    inspectTimerByRunner(&timer);

    timer.start();
    TimerRunnerForTest::getInstance()->processTimers(DEFAULT_EXPIRE/2);
    timer.stop();
    TimerRunnerForTest::getInstance()->processTimers(DEFAULT_EXPIRE_WAIT);

    QCOMPARE(m_expireCount, 0);
    QCOMPARE(m_expireTime, 0);
}

void test_singleshottimer::queuedConnectetionsOnExpire()
{
    SingleShotTimerQt timer(DEFAULT_EXPIRE);
    timer.setHighAccuracy(true);
    inspectTimerByDelay(&timer);
    TestEventLoop evTest(&timer);
    int expireReceived = 0;
    connect(&evTest, &TestEventLoop::sigExpireReceived, [&]{
        expireReceived++;
    });

    timer.start();
    QTest::qWait(DEFAULT_EXPIRE_WAIT);

    QCOMPARE(expireReceived, 1);
}

void test_singleshottimer::queuedConnectetionsOnExpireTest()
{
    SingleShotTimerTest timer(DEFAULT_EXPIRE);
    inspectTimerByRunner(&timer);
    TestEventLoop evTest(&timer);
    int expireReceived = 0;
    connect(&evTest, &TestEventLoop::sigExpireReceived, [&]{
        expireReceived++;
    });

    timer.start();
    TimerRunnerForTest::getInstance()->processTimers(DEFAULT_EXPIRE_WAIT);

    QCOMPARE(expireReceived, 1);
}

void test_singleshottimer::infiniteExpire()
{
    SingleShotTimerQt timer(EXPIRE_INFINITE);
    timer.setHighAccuracy(true);
    inspectTimerByDelay(&timer);

    timer.start();
    QTest::qWait(DEFAULT_EXPIRE_WAIT);

    QCOMPARE(m_expireCount, 0);
    QCOMPARE(m_expireTime, 0);
}

void test_singleshottimer::infiniteExpireTest()
{
    SingleShotTimerTest timer(EXPIRE_INFINITE);
    inspectTimerByRunner(&timer);

    timer.start();
    TimerRunnerForTest::getInstance()->processTimers(DEFAULT_EXPIRE_WAIT);

    QCOMPARE(m_expireCount, 0);
    QCOMPARE(m_expireTime, 0);
}
