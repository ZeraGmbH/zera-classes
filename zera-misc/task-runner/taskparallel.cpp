#include "taskparallel.h"
#include <QList>

std::unique_ptr<TaskParallel> TaskParallel::create()
{
    return std::make_unique<TaskParallel>();
}

void TaskParallel::addTask(TaskInterfacePtr task)
{
    m_addedTasks.push_back(std::move(task));
}

void TaskParallel::start()
{
    if(!m_addedTasks.empty())
        startTasksDirectConnectionSafe();
    else
        finishTask(true);
}

void TaskParallel::onFinishTask(bool ok, int taskId)
{
    if(m_startedTasks.erase(taskId)) {
        m_allOk = m_allOk && ok;
        if(m_startedTasks.empty())
            finishTask(m_allOk);
    }
}

void TaskParallel::startTasksDirectConnectionSafe()
{
    m_allOk = true;
    QList<int> taskIdsToStart;
    while(!m_addedTasks.empty()) {
        auto task = std::move(m_addedTasks.back());
        m_addedTasks.pop_back();
        connect(task.get(), &TaskComposite::sigFinish, this, &TaskParallel::onFinishTask);
        int taskId = task->getTaskId();
        taskIdsToStart.push_back(taskId);
        m_startedTasks[taskId] = std::move(task);
    }
    for(const int &taskId : taskIdsToStart)
        m_startedTasks[taskId]->start();
}
