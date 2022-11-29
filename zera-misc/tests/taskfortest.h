#ifndef TASKFORTEST_H
#define TASKFORTEST_H

#include "taskcomposit.h"
#include <QTimer>

class TaskForTest : public TaskComposite
{
public:
    static TaskInterfacePtr create(int delayMs, bool finishError);
    TaskForTest(int delayMs, bool finishError);
    void start() override;
private slots:
    void doEmit();
private:
    int m_delayMs;
    bool m_finishError;
    QTimer m_delayTimer;
    bool m_started = false;
};

#endif // TASKFORTEST_H
