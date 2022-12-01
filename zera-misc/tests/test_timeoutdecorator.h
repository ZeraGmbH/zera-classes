#ifndef TEST_TIMEOUTDECORATOR_H
#define TEST_TIMEOUTDECORATOR_H

#include <QObject>

class test_timeoutdecorator : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void startEmpty();
    void oneOkWithoutDelay();
    void oneErrWithoutDelay();
    void oneOkWithDelayAndInfiniteTimeout();
    void noTimeoutOnEarlyOk();
    void delayEqualsTimeout();
};

#endif // TEST_TIMEOUTDECORATOR_H
