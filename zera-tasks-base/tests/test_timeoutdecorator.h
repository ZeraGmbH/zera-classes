#ifndef TEST_TIMEOUTDECORATOR_H
#define TEST_TIMEOUTDECORATOR_H

#include <QObject>

class test_timeoutdecorator : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void startEmpty();
    void startEmptyCheckDelayed();
    void immediateOk();
    void immediateError();
    void immediateErrorWithErrFunc();
    void infiniteTask();
    void delayedOk();
    void delayEqualsTimeout();
    void taskId();
};

#endif // TEST_TIMEOUTDECORATOR_H
