#ifndef TEST_TASKGENERATORRANGEBYAMPLITUDESET_H
#define TEST_TASKGENERATORRANGEBYAMPLITUDESET_H

#include <QObject>

class test_taskgeneratorrangebyamplitudeset : public QObject
{
    Q_OBJECT
private slots:
    void checkScpiSend();
    void returnsNak();
    void timeoutAndErrFunc();
};

#endif // TEST_TASKGENERATORRANGEBYAMPLITUDESET_H
