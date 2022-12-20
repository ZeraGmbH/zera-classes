#ifndef TEST_TASKRMGETCHANNELSAVAIL_H
#define TEST_TASKRMGETCHANNELSAVAIL_H

#include <QObject>

class test_taskrmgetchannelsavail : public QObject
{
    Q_OBJECT
private slots:
    void getThreeChannels();
    void getThreeChannelsIgnoreMMode();
    void noChannelsOnNack();
    void okReceiveOnAck();
    void errReceiveOnNack();
    void noReceiveOnOther();
    void errReceiveOnTcpError();
};

#endif // TEST_TASKRMGETCHANNELSAVAIL_H
