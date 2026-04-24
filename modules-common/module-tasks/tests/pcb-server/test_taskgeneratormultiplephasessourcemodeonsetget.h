#ifndef TEST_TASKGENERATORMULTIPLEPHASESSOURCEMODEONSETGET_H
#define TEST_TASKGENERATORMULTIPLEPHASESSOURCEMODEONSETGET_H

#include <QObject>

class test_taskgeneratormultiplephasessourcemodeonsetget : public QObject
{
    Q_OBJECT
private slots:
    void checkScpiSend();
    void returnsNak();
    void timeoutAndErrFunc();
};

#endif // TEST_TASKGENERATORMULTIPLEPHASESSOURCEMODEONSETGET_H
