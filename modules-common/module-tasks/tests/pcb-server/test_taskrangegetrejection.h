#ifndef TEST_TASKRANGEGETOVREJECTION_H
#define TEST_TASKRANGEGETOVREJECTION_H

#include <QObject>

class test_taskrangegetrejection : public QObject
{
    Q_OBJECT
private slots:
    void checkScpiSend();
    void returnsRejectionProperly();
    void timeoutAndErrFunc();
};

#endif // TEST_TASKRANGEGETOVREJECTION_H
