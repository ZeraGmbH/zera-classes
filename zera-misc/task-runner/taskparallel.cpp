#include "taskparallel.h"


std::unique_ptr<TaskParallel> TaskParallel::create()
{
    return std::make_unique<TaskParallel>();
}

TaskParallel::TaskParallel()
{
}

void TaskParallel::addTask(TaskInterfacePtr task)
{
    m_task = std::move(task);
}

int TaskParallel::start()
{
    m_taskId = TaskComposite::getNextTaskId();
    if(m_task) {
        connect(m_task.get(), &TaskComposite::sigFinish, this, &TaskParallel::onFinishTask);
        m_task->start();
    }
    else
        emit sigFinish(true, m_taskId);
    return m_taskId;
}

void TaskParallel::onFinishTask(bool ok)
{
    m_task = nullptr;
    emit sigFinish(ok, m_taskId);
}

