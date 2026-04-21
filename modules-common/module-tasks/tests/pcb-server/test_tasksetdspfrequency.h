#ifndef TEST_TASKSETDSPFREQUENCY_H
#define TEST_TASKSETDSPFREQUENCY_H

#include <QObject>

class test_tasksetdspfrequency : public QObject
{
    Q_OBJECT
private slots:
    void checkScpiSend();
    void returnsNak();
    void timeoutAndErrFunc();
};

#endif // TEST_TASKSETDSPFREQUENCY_H
