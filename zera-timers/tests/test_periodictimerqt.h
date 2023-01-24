#ifndef TEST_PERIODICTIMERQT_H
#define TEST_PERIODICTIMERQT_H

#include "zeratimertemplate.h"
#include <QElapsedTimer>
#include <QList>
#include <memory>

class test_periodictimerqt : public QObject
{
    Q_OBJECT
private slots:
    void init();

    void oneInterval();
    void oneIntervalTest();

    void threeInterval();
    void threeIntervalTest();

private:
    void inspectTimerByDelay(ZeraTimerTemplate* timer);
    void inspectTimerByRunner(ZeraTimerTemplate *timer);
    QList<int> m_expireTimes;
    std::unique_ptr<QElapsedTimer> m_elapsedTimer;
};

#endif // TEST_PERIODICTIMERQT_H
