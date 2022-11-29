#include "tasksequence.h"

std::unique_ptr<TaskSequence> TaskSequence::create()
{
    return std::make_unique<TaskSequence>();
}

TaskSequence::TaskSequence()
{
}

void TaskSequence::addTask(TaskInterfacePtr task)
{
    m_tasks.push_front(std::move(task));
}

void TaskSequence::start()
{
    if(next())
        m_current->start();
}

void TaskSequence::onFinishCurr(bool ok)
{
    emit sigFinish(ok);
}

bool TaskSequence::next()
{
    if(!m_tasks.empty()) {
        m_current = std::move(m_tasks.back());
        m_tasks.pop_back();
        connectCurrent();
    }
    else
        m_current = nullptr;
    return m_current != nullptr;
}

void TaskSequence::connectCurrent()
{
    connect(m_current.get(), &TaskComposite::sigFinish, this, &TaskSequence::onFinishCurr);
}
