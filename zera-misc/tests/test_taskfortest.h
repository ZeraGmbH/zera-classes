#ifndef TEST_TASKFORTEST_H
#define TEST_TASKFORTEST_H

#include <QObject>

class test_taskfortest : public QObject
{
    Q_OBJECT
private slots:
    void onePassImmediate();
    void oneErrImmediate();
    void onePassDelayed();
    void oneErrDelayed();
};

#endif // TEST_TASKFORTEST_H
