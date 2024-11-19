#ifndef TEST_TASKRANGEGETOVREJECTION_H
#define TEST_TASKRANGEGETOVREJECTION_H

#include <QObject>

class test_taskrangegetovrejection : public QObject
{
    Q_OBJECT
private slots:
    void checkScpiSend();
    void returnsOvrRejectionProperly();
    void timeoutAndErrFunc();
};

#endif // TEST_TASKRANGEGETOVREJECTION_H
