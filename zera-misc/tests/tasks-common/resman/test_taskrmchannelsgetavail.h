#ifndef TEST_TASKRMCHANNELSAVAILGET_H
#define TEST_TASKRMCHANNELSAVAILGET_H

#include <QObject>

class test_taskrmchannelsgetavail : public QObject
{
    Q_OBJECT
private slots:
    void getThreeChannels();
    void getThreeChannelsIgnoreMMode();
};

#endif // TEST_TASKRMCHANNELSAVAILGET_H
