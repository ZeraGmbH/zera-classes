#ifndef TEST_TASKCHANNELGETAVAIL_H
#define TEST_TASKCHANNELGETAVAIL_H

#include <QObject>

class test_taskchannelgetavail : public QObject
{
    Q_OBJECT
private slots:
    void checkScpiSend();
    void returnsChannelListProperly();
    void timeoutAndErrFunc();
};

#endif // TEST_TASKCHANNELGETAVAIL_H
