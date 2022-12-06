#ifndef TASKFORTEST_H
#define TASKFORTEST_H

#include "taskcomposit.h"
#include <QTimer>

class TaskForTest : public TaskComposite
{
public:
    static TaskInterfacePtr create(int delayMs, bool finishOk, std::function<void()> additionalErrorHandler = []{});
    TaskForTest(int delayMs, bool finishOk, std::function<void ()> additionalErrorHandler = []{});
    ~TaskForTest() override;
    void start() override;
    static void resetCounters();
    static int getOkCount() { return m_finishOkCount; }
    static int getErrCount() { return m_finishErrCount; }
    static int getDtorCount() { return m_dtorCount; }
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
