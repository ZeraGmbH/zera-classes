#ifndef TASKFORTEST_H
#define TASKFORTEST_H

#include "taskcomposit.h"
#include "testdefaults.h"
#include <QTimer>

class TaskForTest : public TaskComposite
{
public:
    static TaskCompositePtr create(int delayMs, bool finishOk);
    TaskForTest(int delayMs, bool finishOk);
    ~TaskForTest() override;
    void start() override;
    static void resetCounters();
    static int okCount() { return m_finishOkCount; }
    static int errCount() { return m_finishErrCount; }
    static int dtorCount() { return m_dtorCount; }
private slots:
    void doEmit();
private:
    int m_delayMs;
    bool m_finishOk;
    QTimer m_delayTimer;
    bool m_started = false;
    static int m_finishOkCount;
    static int m_finishErrCount;
    static int m_dtorCount;
};

#endif // TASKFORTEST_H
