#ifndef TEST_TASKPARALLEL_H
#define TEST_TASKPARALLEL_H

#include <QObject>

class test_taskparallel : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void startEmpty();
    void startErrorTask();
    void startPassImmediateDelayed();
    void startThreeImmediateMiddleFail();
    void startThreeImmediateAllOk();
    void startThreeDelayedMiddleFail();
    void startThreeDelayedAllOk();
    void taskId();
};

#endif // TEST_TASKPARALLEL_H
