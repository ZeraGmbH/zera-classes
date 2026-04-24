#ifndef TEST_TASKGENERATORMULTIPLEPHASESSWITCHONSETGET_H
#define TEST_TASKGENERATORMULTIPLEPHASESSWITCHONSETGET_H

#include <QObject>

class test_taskgeneratormultiplephasesswitchonsetget : public QObject
{
    Q_OBJECT
private slots:
    void checkScpiSendReceive();
    void returnsNak();
    void timeoutAndErrFunc();
};

#endif // TEST_TASKGENERATORMULTIPLEPHASESSWITCHONSETGET_H
