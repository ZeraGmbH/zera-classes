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

void test_singleshottimer::queuedConnectionsOnExpire()
{
    SingleShotTimerQt timer(DEFAULT_EXPIRE);
    timer.setHighAccuracy(true);
    inspectTimerByDelay(&timer);
    TimerEventLoopWrapper evTest(&timer);
    int expireReceived = 0;
    connect(&evTest, &TimerEventLoopWrapper::sigExpireReceived, [&]{
        expireReceived++;
    });

    timer.start();
    QTest::qWait(DEFAULT_EXPIRE_WAIT);

    QCOMPARE(expireReceived, 1);
}

void test_singleshottimer::queuedConnectionsOnExpireTest()
{
    SingleShotTimerTest timer(DEFAULT_EXPIRE);
    inspectTimerByRunner(&timer);
    TimerEventLoopWrapper evTest(&timer);
    int expireReceived = 0;
    connect(&evTest, &TimerEventLoopWrapper::sigExpireReceived, [&]{
        expireReceived++;
    });

    timer.start();
    TimerRunnerForTest::getInstance()->processTimers(DEFAULT_EXPIRE_WAIT);

    QCOMPARE(expireReceived, 1);
}

void test_singleshottimer::nestedStart()
{
    SingleShotTimerQt timer1(DEFAULT_EXPIRE/2);
    timer1.setHighAccuracy(true);
    inspectTimerByDelay(&timer1);
    SingleShotTimerQt timer2(DEFAULT_EXPIRE/2);
    timer2.setHighAccuracy(true);
    inspectTimerByDelay(&timer2);
    connect(&timer1, &ZeraTimerTemplate::sigExpired, [&]{
        timer2.start();
    });

    timer1.start();
    QTest::qWait(DEFAULT_EXPIRE_WAIT);

    QCOMPARE(m_expireCount, 2);
}

void test_singleshottimer::nestedStartTest()
{
    SingleShotTimerTest timer1(DEFAULT_EXPIRE/2);
    inspectTimerByDelay(&timer1);
    SingleShotTimerTest timer2(DEFAULT_EXPIRE/2);
    inspectTimerByDelay(&timer2);
    connect(&timer1, &ZeraTimerTemplate::sigExpired, [&]{
        timer2.start();
    });

    timer1.start();
    TimerRunnerForTest::getInstance()->processTimers(DEFAULT_EXPIRE_WAIT);

    QCOMPARE(m_expireCount, 2);
}

void test_singleshottimer::nestedStartQueued()
{
    SingleShotTimerQt timer1(DEFAULT_EXPIRE/2);
    timer1.setHighAccuracy(true);
    inspectTimerByDelay(&timer1);
    EventLoopWrapper evLoop1;
    connect(&evLoop1, &EventLoopWrapper::sigReceiveEventLoop, [&]{
        timer1.start();
    });
    SingleShotTimerQt timer2(DEFAULT_EXPIRE/2);
    timer2.setHighAccuracy(true);
    inspectTimerByDelay(&timer2);
    EventLoopWrapper evLoop2;
    connect(&evLoop2, &EventLoopWrapper::sigReceiveEventLoop, [&]{
        timer2.start();
    });
    connect(&timer1, &ZeraTimerTemplate::sigExpired, [&]{
        evLoop2.start();
    });

    evLoop1.start();
    QTest::qWait(DEFAULT_EXPIRE_WAIT);

    QCOMPARE(m_expireCount, 2);
}

void test_singleshottimer::nestedStartQueuedTest()
{
    SingleShotTimerTest timer1(DEFAULT_EXPIRE/2);
    inspectTimerByDelay(&timer1);
    EventLoopWrapper evLoop1;
    connect(&evLoop1, &EventLoopWrapper::sigReceiveEventLoop, [&]{
        timer1.start();
    });
    SingleShotTimerTest timer2(DEFAULT_EXPIRE/2);
    inspectTimerByDelay(&timer2);
    EventLoopWrapper evLoop2;
    connect(&evLoop2, &EventLoopWrapper::sigReceiveEventLoop, [&]{
        timer2.start();
    });
    connect(&timer1, &ZeraTimerTemplate::sigExpired, [&]{
        evLoop2.start();
    });

    evLoop1.start();
    TimerRunnerForTest::getInstance()->processTimers(DEFAULT_EXPIRE_WAIT);

    QCOMPARE(m_expireCount, 2);
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
