#ifndef TEST_TASKGENERATORDSPAMPLITUDESETGET_H
#define TEST_TASKGENERATORDSPAMPLITUDESETGET_H

#include <QObject>

class test_taskgeneratordspamplitudesetget : public QObject
{
    Q_OBJECT
private slots:
    void checkScpiSendReceive();
    void returnsNak();
    void timeoutAndErrFunc();
};

#endif // TEST_TASKGENERATORDSPAMPLITUDESETGET_H
