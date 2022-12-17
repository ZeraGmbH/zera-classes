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
    int signalDelay() const { return m_signalDelay; }
    int lastTaskIdReceived() const { return m_taskIdsReceived.last(); }
private slots:
    void onTaskFinish(bool ok, int taskId);
private:
    int m_okCount = 0;
    int m_errCount = 0;
    int m_signalDelay = 0;
    QElapsedTimer m_elapsedTimer;
    QList<int> m_taskIdsReceived;
};

#endif // TASKTESTHELPER_H
