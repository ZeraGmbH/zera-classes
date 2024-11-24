#ifndef TEST_TASKRANGEGETISAVAILABLE_H
#define TEST_TASKRANGEGETISAVAILABLE_H

#include <QObject>

class test_taskrangegetisavailable : public QObject
{
    Q_OBJECT
private slots:
    void checkScpiSend();
    void returnsIsAvailProperly();
    void timeoutAndErrFunc();
};

#endif // TEST_TASKRANGEGETISAVAILABLE_H
