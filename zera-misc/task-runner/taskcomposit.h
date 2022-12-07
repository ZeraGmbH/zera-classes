#ifndef TASKCOMPOSIT_H
#define TASKCOMPOSIT_H

#include <QObject>
#include <memory>
#include <functional>

class TaskComposite : public QObject
{
    Q_OBJECT
public:
    TaskComposite(std::function<void()> additionalErrorHandler = []{});
    virtual ~TaskComposite() = default;
    virtual void start() = 0;
    int getTaskId() const;
signals:
    void sigFinish(bool ok, int taskId);

protected:
    void finishTask(bool ok);
private:
    static int getNextTaskId();
    int m_taskId = 0;
    std::function<void()> m_additionalErrorHandler = []{};
    static int m_taskIdForNext;
};

typedef std::unique_ptr<TaskComposite> TaskCompositePtr;


#endif // TASKCOMPOSIT_H
