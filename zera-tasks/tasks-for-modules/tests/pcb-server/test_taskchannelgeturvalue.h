#ifndef TEST_TASKCHANNELGETURVALUE_H
#define TEST_TASKCHANNELGETURVALUE_H

#include <QObject>

class test_taskchannelgeturvalue : public QObject
{
    Q_OBJECT
private slots:
    void checkScpiSend();
    void returnsUrValueProperly();
    void timeoutAndErrFunc();
};

#endif // TEST_TASKCHANNELGETURVALUE_H
