#ifndef TEST_TASKCHANGERANGEBYAMPLITUDE_H
#define TEST_TASKCHANGERANGEBYAMPLITUDE_H

#include <QObject>

class test_taskgeneratordspanglesetget : public QObject
{
    Q_OBJECT
private slots:
    void checkScpiSendReceive();
    void returnsNak();
    void timeoutAndErrFunc();
};

#endif // TEST_TASKCHANGERANGEBYAMPLITUDE_H
