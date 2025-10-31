#ifndef TEST_TASKEMOBREADCONNECTIONSTATE_H
#define TEST_TASKEMOBREADCONNECTIONSTATE_H

#include <QObject>

class test_taskemobreadconnectionstate : public QObject
{
    Q_OBJECT
private slots:
    void checkScpiSend();
    void returnsConnectionStateProperly();
    void timeoutAndErrFunc();
};

#endif // TEST_TASKEMOBREADCONNECTIONSTATE_H
