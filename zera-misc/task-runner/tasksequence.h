#ifndef TASKSEQUENCE_H
#define TASKSEQUENCE_H

#include "taskcontainer.h"
#include <list>

class TaskSequence : public TaskContainer // for now abort on error
{
    Q_OBJECT
public:
    static std::unique_ptr<TaskContainer> create();
    void start() override;
    void addSubTask(TaskCompositePtr task) override;

private slots:
    void onFinishCurr(bool ok);
private:
    bool next();
    void setNext();
    void cleanup();
    bool m_started = false;
    std::list<TaskCompositePtr> m_tasks;
    TaskCompositePtr m_current;
    int m_taskId;
};

#endif // TASKSEQUENCE_H
