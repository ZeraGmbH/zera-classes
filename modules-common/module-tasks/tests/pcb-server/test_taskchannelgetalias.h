#ifndef TEST_TASKCHANNELGETALIAS_H
#define TEST_TASKCHANNELGETALIAS_H

#include <QObject>

class test_taskchannelgetalias : public QObject
{
    Q_OBJECT
private slots:
    void checkScpiSend();
    void returnsAliasProperly();
    void timeoutAndErrFunc();
};

#endif // TEST_TASKCHANNELGETALIAS_H
