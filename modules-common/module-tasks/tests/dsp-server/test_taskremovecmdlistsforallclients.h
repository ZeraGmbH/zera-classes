#ifndef TEST_TASKREMOVECMDLISTSFORALLCLIENTS_H
#define TEST_TASKREMOVECMDLISTSFORALLCLIENTS_H

#include <QObject>

class test_taskremovecmdlistsforallclients : public QObject
{
    Q_OBJECT
private slots:
    void checkScpiSend();
    void timeoutAndErrFunc();
};

#endif // TEST_TASKREMOVECMDLISTSFORALLCLIENTS_H
