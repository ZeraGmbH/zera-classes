#ifndef TEST_TASKGETSAMPLERATE_H
#define TEST_TASKGETSAMPLERATE_H

#include <QObject>

class test_taskgetsamplerate : public QObject
{
    Q_OBJECT
private slots:
    void checkScpiSend();
    void returnsSampleRateProperly();
    void timeoutAndErrFunc();
};

#endif // TEST_TASKGETSAMPLERATE_H
