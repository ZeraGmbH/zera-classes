#ifndef TASKCOMPOSIT_H
#define TASKCOMPOSIT_H

#include <QObject>
#include <memory>

class TaskComposite : public QObject
{
    Q_OBJECT
public:
    TaskComposite();
    virtual ~TaskComposite() = default;
    int getTaskId() const;

    virtual void start() = 0;
signals:
    void sigFinish(bool ok, int taskId);

private:
    static int getNextTaskId();
    int m_taskId = 0;
    static int m_taskIdForNext;
};

typedef std::unique_ptr<TaskComposite> TaskInterfacePtr;


#endif // TASKCOMPOSIT_H
