#ifndef TASKSEQUENCE_H
#define TASKSEQUENCE_H

#include "taskcomposit.h"
#include <list>

class TaskSequence : public TaskComposite // for now abort on error
{
    Q_OBJECT
public:
    static std::unique_ptr<TaskSequence> create();
    void appendTask(TaskInterfacePtr task);
    void start() override;

private slots:
    void onFinishCurr(bool ok);
private:
    bool next();
    void setNext();
    void cleanup();
    std::list<TaskInterfacePtr> m_tasks;
    TaskInterfacePtr m_current;
    int m_taskId;
};

typedef std::unique_ptr<TaskSequence> TaskSequencePtr;

#endif // TASKSEQUENCE_H
