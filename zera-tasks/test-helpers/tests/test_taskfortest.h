#ifndef TEST_TASKFORTEST_H
#define TEST_TASKFORTEST_H

#include <QObject>

class test_taskfortest : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void onePassImmediate();
    void onePassImmediateNew();

    void oneErrImmediate();
    void oneErrImmediateNew();

    void onePassDelayed();
    void onePassDelayedNew();

    void oneErrDelayed();
    void oneErrDelayedNew();

    void onePassCount();
    void onePassCountNew();

    void oneErrCount();
    void oneErrCountNew();

    void oneDtorCount();
    void oneDtorCountNew();

    void taskId();
    void taskIdNew();
};

#endif // TEST_TASKFORTEST_H
