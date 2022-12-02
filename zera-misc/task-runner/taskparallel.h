#ifndef TASKPARALLEL_H
#define TASKPARALLEL_H

#include "taskcomposit.h"
#include <list>
#include <unordered_map>

class TaskParallel : public TaskComposite
{
    Q_OBJECT
public:
    static std::unique_ptr<TaskParallel> create();
    void addTask(TaskInterfacePtr task);
    void start() override;
private slots:
    void onFinishTask(bool ok, int taskId);
private:
    void startTasksDirectConnectionSafe();

    std::list<TaskInterfacePtr> m_addedTasks;
    std::unordered_map<int, TaskInterfacePtr> m_startedTasks;
    bool m_allOk = false;
};

typedef std::unique_ptr<TaskParallel> TaskParallelPtr;

#endif // TASKPARALLEL_H
