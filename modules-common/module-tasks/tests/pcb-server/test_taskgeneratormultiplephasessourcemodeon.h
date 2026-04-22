#ifndef TEST_TASKGENERATORMULTIPLEPHASESSOURCEMODEON_H
#define TEST_TASKGENERATORMULTIPLEPHASESSOURCEMODEON_H

#include <QObject>

class test_taskgeneratormultiplephasessourcemodeon : public QObject
{
    Q_OBJECT
private slots:
    void checkScpiSend();
    void returnsNak();
    void timeoutAndErrFunc();
};

#endif // TEST_TASKGENERATORMULTIPLEPHASESSOURCEMODEON_H
