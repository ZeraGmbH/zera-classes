#ifndef TASKSEQUENCE_H
#define TASKSEQUENCE_H

#include "taskcontainer.h"
#include <list>

class TaskSequence : public TaskContainer // for now abort on error
{
    Q_OBJECT
public:
    static std::unique_ptr<TaskSequence> create();
    void start() override;
    void addSubTask(TaskCompositePtr task) override;

private slots:
    void onFinishCurr(bool ok);
private:
    bool next();
    void setNext();
    void cleanup();
    std::list<TaskCompositePtr> m_tasks;
    TaskCompositePtr m_current;
    int m_taskId;
};

typedef std::unique_ptr<TaskSequence> TaskSequencePtr;

#endif // TASKSEQUENCE_H
