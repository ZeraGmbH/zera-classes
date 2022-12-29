#ifndef TEST_TASKRMSENDIDENT_H
#define TEST_TASKRMSENDIDENT_H

#include <QObject>

class test_taskrmsendident : public QObject
{
    Q_OBJECT
private slots:
    void checkSend();
    void timeoutAndErrFunc();
};

#endif // TEST_TASKRMSENDIDENT_H
