#ifndef TEST_TASKGENERATORMULTIPLEPHASESSOURCEMODEONSET_H
#define TEST_TASKGENERATORMULTIPLEPHASESSOURCEMODEONSET_H

#include <QObject>

class test_taskgeneratormultiplephasessourcemodeonset : public QObject
{
    Q_OBJECT
private slots:
    void checkScpiSend();
    void returnsNak();
    void timeoutAndErrFunc();
};

#endif // TEST_TASKGENERATORMULTIPLEPHASESSOURCEMODEONSET_H
