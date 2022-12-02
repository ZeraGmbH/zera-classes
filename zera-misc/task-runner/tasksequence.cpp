#include "tasksequence.h"

std::unique_ptr<TaskSequence> TaskSequence::create()
{
    return std::make_unique<TaskSequence>();
}

void TaskSequence::appendTask(TaskInterfacePtr task)
{
    m_tasks.push_front(std::move(task));
}

void TaskSequence::start()
{
    if(next())
        m_current->start();
    else
        emit sigFinish(true, m_taskId);
}

void TaskSequence::onFinishCurr(bool ok)
{
    if(next() && ok)
        m_current->start();
    else {
        cleanup();
        emit sigFinish(ok, m_taskId);
    }
}

bool TaskSequence::next()
{
    if(!m_tasks.empty())
        setNext();
    else
        m_current = nullptr;
    return m_current != nullptr;
}

void TaskSequence::setNext()
{
    m_current = std::move(m_tasks.back());
    m_tasks.pop_back();
    connect(m_current.get(), &TaskComposite::sigFinish, this, &TaskSequence::onFinishCurr);
}

void TaskSequence::cleanup()
{
    m_current = nullptr;
    m_tasks.clear();
}
