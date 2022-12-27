#ifndef TEST_TASKFORTEST_H
#define TEST_TASKFORTEST_H

#include <QObject>

class test_taskfortest : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void onePassImmediateNew();

    void oneErrImmediateNew();

    void onePassDelayedNew();

    void oneErrDelayedNew();

    void onePassCountNew();

    void oneErrCountNew();

    void oneDtorCountNew();

    void taskIdNew();
};

#endif // TEST_TASKFORTEST_H
