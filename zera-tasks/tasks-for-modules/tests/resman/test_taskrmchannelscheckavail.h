#ifndef TEST_TASKRMCHANNELSCHECKAVAIL_H
#define TEST_TASKRMCHANNELSCHECKAVAIL_H

#include <QObject>

class test_taskrmchannelscheckavail : public QObject
{
    Q_OBJECT
private slots:
    void okOnExpectedEqualGet();
    void okOnExpectedPartOfGet();
    void errOnExpectedNotPartOfGet();
    void timeoutAndErrFunc();
};

#endif // TEST_TASKRMCHANNELSCHECKAVAIL_H
