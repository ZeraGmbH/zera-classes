#include "taskimmediatelambda.h"

TaskCompositePtr TaskImmediateLambda::create(std::function<bool ()> startFunc)
{
    return std::make_unique<TaskImmediateLambda>(startFunc);
}

TaskImmediateLambda::TaskImmediateLambda(std::function<bool ()> startFunc) :
    m_startFunc(startFunc)
{
}

void TaskImmediateLambda::start()
{
    finishTask(m_startFunc());
}
