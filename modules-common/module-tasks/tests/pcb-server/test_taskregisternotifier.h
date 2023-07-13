#ifndef TEST_TASKREGISTERNOTIFIER_H
#define TEST_TASKREGISTERNOTIFIER_H

#include <QObject>

class test_taskregisternotifier : public QObject
{
    Q_OBJECT
private slots:
    void checkScpiSend();
    void timeoutAndErrFunc();
};

#endif // TEST_TASKREGISTERNOTIFIER_H
