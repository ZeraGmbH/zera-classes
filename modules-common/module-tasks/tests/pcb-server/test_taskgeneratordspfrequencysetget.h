#ifndef TEST_TASKGENERATORDSPFREQUENCYSETGET_H
#define TEST_TASKGENERATORDSPFREQUENCYSETGET_H

#include <QObject>

class test_taskgeneratordspfrequencysetget : public QObject
{
    Q_OBJECT
private slots:
    void checkScpiSendReceive();
    void returnsNak();
    void timeoutAndErrFunc();
};

#endif // TEST_TASKGENERATORDSPFREQUENCYSETGET_H
