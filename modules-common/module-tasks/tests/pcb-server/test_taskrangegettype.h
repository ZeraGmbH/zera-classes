#ifndef TEST_TASKRANGEGETTYPE_H
#define TEST_TASKRANGEGETTYPE_H

#include <QObject>

class test_taskrangegettype : public QObject
{
    Q_OBJECT
private slots:
    void checkScpiSend();
    void returnsTypeProperly();
    void timeoutAndErrFunc();
};

#endif // TEST_TASKRANGEGETTYPE_H
