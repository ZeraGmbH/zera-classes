#ifndef TASKSEQUENCE_H
#define TASKSEQUENCE_H

#include "taskcomposit.h"
#include <list>

class TaskSequence : public TaskComposite // for now abort on error
{
    Q_OBJECT
public:
    static std::unique_ptr<TaskSequence> create();
    TaskSequence();
    void addTask(TaskInterfacePtr task);
    int start() override;

private slots:
    void onFinishCurr(bool ok);
private:
    bool next();
    void setNext();
    void connectCurrent();
    void cleanup();
    std::list<TaskInterfacePtr> m_tasks;
    TaskInterfacePtr m_current;
    int m_taskId;
};

typedef std::unique_ptr<TaskSequence> TaskSequencePtr;

#endif // TASKSEQUENCE_H
