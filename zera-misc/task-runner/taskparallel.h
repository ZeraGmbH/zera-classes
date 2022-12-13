#ifndef TASKPARALLEL_H
#define TASKPARALLEL_H

#include "taskcontainer.h"
#include <list>
#include <unordered_map>

class TaskParallel : public TaskContainer
{
    Q_OBJECT
public:
    static std::unique_ptr<TaskContainer> create();
    void start() override;
    void addSub(TaskCompositePtr task) override;
private slots:
    void onFinishTask(bool ok, int taskId);
private:
    void startTasksDirectConnectionSafe();

    std::list<TaskCompositePtr> m_addedTasks;
    std::unordered_map<int, TaskCompositePtr> m_startedTasks;
    bool m_allOk = false;
};

#endif // TASKPARALLEL_H
