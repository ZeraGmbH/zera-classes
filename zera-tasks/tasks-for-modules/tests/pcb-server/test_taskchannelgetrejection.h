#ifndef TEST_TASKCHANNELGETOVREJECTION_H
#define TEST_TASKCHANNELGETOVREJECTION_H

#include <QObject>

class test_taskchannelgetrejection : public QObject
{
    Q_OBJECT
private slots:
    void checkScpiSend();
    void returnsOvrRejectionProperly();
    void timeoutAndErrFunc();
};

#endif // TEST_TASKCHANNELGETOVREJECTION_H
