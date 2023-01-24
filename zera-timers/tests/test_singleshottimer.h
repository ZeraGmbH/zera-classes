#ifndef TEST_SINGLESHOTTIMER_H
#define TEST_SINGLESHOTTIMER_H

#include "timerrunnerfortest.h"
#include "singleshottimerqt.h"
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

    void queuedConnectionsOnExpire();
    void queuedConnectionsOnExpireTest();

    void nestedStart();
    void nestedStartTest();

    void nestedStartQueued();
    void nestedStartQueuedTest();

    void infiniteExpire();
    void infiniteExpireTest();

private:
    void inspectTimerByDelay(SingleShotTimerQt *timer);
    void inspectTimerByRunner(SingleShotTimerTest *timer);
    int m_expireCount;
    int m_expireTime;
    std::unique_ptr<QElapsedTimer> m_elapsedTimer;
};

class EventLoopWrapper : public QObject
{
    Q_OBJECT
public:
    EventLoopWrapper() {
        connect(this, &EventLoopWrapper::sigStart,
                this, &EventLoopWrapper::sigReceiveEventLoop,
                Qt::QueuedConnection);
    }
    void start() {
        emit sigStart();
    }
signals:
    void sigStart();
    void sigReceiveEventLoop();
};

class TimerEventLoopWrapper : public QObject
{
    Q_OBJECT
public:
    TimerEventLoopWrapper(ZeraTimerTemplate* timer) {
        connect(timer, &ZeraTimerTemplate::sigExpired,
                this, &TimerEventLoopWrapper::sigExpireReceived,
                Qt::QueuedConnection);
    }
signals:
    void sigExpireReceived();
};

#endif // TEST_SINGLESHOTTIMER_H
