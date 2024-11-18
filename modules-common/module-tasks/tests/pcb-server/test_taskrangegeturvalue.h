#ifndef TEST_TASKRANGEGETURVALUE_H
#define TEST_TASKRANGEGETURVALUE_H

#include <QObject>

class test_taskrangegeturvalue : public QObject
{
    Q_OBJECT
private slots:
    void checkScpiSend();
    void returnsUrValueProperly();
    void timeoutAndErrFunc();
};

#endif // TEST_TASKRANGEGETURVALUE_H
