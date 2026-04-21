#ifndef TEST_TASKCHANGERANGEBYAMPLITUDE_H
#define TEST_TASKCHANGERANGEBYAMPLITUDE_H

#include <QObject>

class test_taskchangerangebyamplitude : public QObject
{
    Q_OBJECT
private slots:
    void checkScpiSend();
    void returnsAck();
    void returnsNak();
    void timeoutAndErrFunc();
};

#endif // TEST_TASKCHANGERANGEBYAMPLITUDE_H
