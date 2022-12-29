#ifndef TEST_TASKCHANNELREGISTERNOTIFIER_H
#define TEST_TASKCHANNELREGISTERNOTIFIER_H

#include <QObject>

class test_taskchannelregisternotifier : public QObject
{
    Q_OBJECT
private slots:
    void checkScpiSend();
    void timeoutAndErrFunc();
};

#endif // TEST_TASKCHANNELREGISTERNOTIFIER_H
