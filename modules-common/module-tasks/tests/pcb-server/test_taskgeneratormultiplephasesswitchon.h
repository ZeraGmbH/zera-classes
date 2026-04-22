#ifndef TEST_TASKGENERATORMULTIPLEPHASESSWITCHON_H
#define TEST_TASKGENERATORMULTIPLEPHASESSWITCHON_H

#include <QObject>

class test_taskgeneratormultiplephasesswitchon : public QObject
{
    Q_OBJECT
private slots:
    void checkScpiSend();
    void returnsNak();
    void timeoutAndErrFunc();
};

#endif // TEST_TASKGENERATORMULTIPLEPHASESSWITCHON_H
