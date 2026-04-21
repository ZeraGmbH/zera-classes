#ifndef TEST_TASKCHANGERANGEBYENUM_H
#define TEST_TASKCHANGERANGEBYENUM_H

#include <QObject>

class test_taskchangerangebyenum : public QObject
{
    Q_OBJECT
private slots:
    void checkScpiSend();
    void returnsNak();
    void timeoutAndErrFunc();
};

#endif // TEST_TASKCHANGERANGEBYENUM_H
