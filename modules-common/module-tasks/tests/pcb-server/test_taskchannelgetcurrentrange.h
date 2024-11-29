#ifndef TEST_TASKCHANNELGETCURRENTRANGE_H
#define TEST_TASKCHANNELGETCURRENTRANGE_H

#include <QObject>

class test_taskchannelgetcurrentrange : public QObject
{
    Q_OBJECT
private slots:
    void checkScpiSend();
    void returnsCurrentRangeProperly();
    void timeoutAndErrFunc();
};

#endif // TEST_TASKCHANNELGETCURRENTRANGE_H
