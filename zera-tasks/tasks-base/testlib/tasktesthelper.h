#ifndef TASKTESTHELPER_H
#define TASKTESTHELPER_H

#include <taskcomposit.h>
#include <QObject>
#include <QElapsedTimer>
#include <QList>

class TaskTestHelper : public QObject
{
    Q_OBJECT
public:
    explicit TaskTestHelper(TaskComposite *taskUnderTest);
    int okCount() const { return m_okCount; }
    int errCount()  const { return m_errCount; }
    int signalDelayMs() const { return m_signalDelayMs; }
    int lastTaskIdReceived() const { return m_taskIdsReceived.last(); }
private slots:
    void onTaskFinish(bool ok, int taskId);
private:
    int m_okCount = 0;
    int m_errCount = 0;
    int m_signalDelayMs = 0;
    QList<int> m_taskIdsReceived;
};

#endif // TASKTESTHELPER_H
