#ifndef TEST_TASKGENERATORRANGEBYENUMSETGET_H
#define TEST_TASKGENERATORRANGEBYENUMSETGET_H

#include <QObject>

class test_taskgeneratorrangebyenumsetget : public QObject
{
    Q_OBJECT
private slots:
    void checkScpiSendReceive();
    void returnsNak();
    void timeoutAndErrFunc();
};

#endif // TEST_TASKGENERATORRANGEBYENUMSETGET_H
