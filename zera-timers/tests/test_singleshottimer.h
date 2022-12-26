#ifndef TEST_SINGLESHOTTIMER_H
#define TEST_SINGLESHOTTIMER_H

#include "zeratimertemplate.h"
#include "timerrunnerfortest.h"
#include "singleshottimertest.h"
#include <QObject>
#include <QElapsedTimer>
#include <memory>

// This test compares behavior of SingleShotTimerQt with SingleShotTimerTest
// with the following target: By using only SingleShotTimerTest in tests timing
// measurements are done here once and nowhere else because of
// * timing measurements are slow
// * timing measurement are fragile
// Here we make fragility less an issue by running timers with high accuracy

class test_singleshottimer : public QObject
{
    Q_OBJECT
private slots:
    void init();

    void signalOnExpireTiming();
    void signalOnExpireTimingTest();

    void restartTiming();
    void restartTimingTest();

    void stopWhilePending();
    void stopWhilePendingTest();

    void queuedConnectetionsOnExpire();
    void queuedConnectetionsOnExpireTest();

private:
    void inspectTimerByDelay(ZeraTimerTemplate* timer);
    void inspectTimerByRunner(SingleShotTimerTest *timer);
    int m_expireCount;
    int m_expireTime;
    std::unique_ptr<QElapsedTimer> m_elapsedTimer;
    std::unique_ptr<TimerRunnerForTest> m_elapsedTestTimerRunner;
};


class TestEventLoop : public QObject
{
    Q_OBJECT
public:
    TestEventLoop(ZeraTimerTemplate* timer) {
        connect(timer, &ZeraTimerTemplate::sigExpired,
                this, &TestEventLoop::sigExpireReceived,
                Qt::QueuedConnection);
    };
signals:
    void sigExpireReceived();
};

#endif // TEST_SINGLESHOTTIMER_H
