#include "tasksequence.h"

std::unique_ptr<TaskContainer> TaskSequence::create()
{
    return std::make_unique<TaskSequence>();
}

void TaskSequence::start()
{
    if(!m_started) {
        m_started = true;
        if(next())
            m_current->start();
        else
            finishTask(true);
    }
}

void TaskSequence::addSub(TaskCompositePtr task)
{
    m_tasks.push_front(std::move(task));
}

void TaskSequence::onFinishCurr(bool ok)
{
    if(next() && ok)
        m_current->start();
    else {
        cleanup();
        finishTask(ok);
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
    m_started = false;
    m_current = nullptr;
    m_tasks.clear();
}
