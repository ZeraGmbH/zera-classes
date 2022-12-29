#ifndef TEST_TASKRMCHANNELSAVAILGET_H
#define TEST_TASKRMCHANNELSAVAILGET_H

#include <QObject>

class test_taskrmchannelsgetavail : public QObject
{
    Q_OBJECT
private slots:
    void checkScpiSend();
    void getThreeChannels();
    void getThreeChannelsIgnoreMMode();
    void timeoutAndErrFunc();
};

#endif // TEST_TASKRMCHANNELSAVAILGET_H
