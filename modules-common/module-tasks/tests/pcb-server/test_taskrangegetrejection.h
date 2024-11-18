#ifndef TEST_TASKCHANNELGETOVREJECTION_H
#define TEST_TASKCHANNELGETOVREJECTION_H

#include <QObject>

class test_taskrangegetrejection : public QObject
{
    Q_OBJECT
private slots:
    void checkScpiSend();
    void returnsRejectionProperly();
    void timeoutAndErrFunc();
};

#endif // TEST_TASKCHANNELGETOVREJECTION_H
