#ifndef TASKIMMEDIATELAMBDA_H
#define TASKIMMEDIATELAMBDA_H

#include "taskcomposit.h"
#include <functional>

class TaskImmediateLambda : public TaskComposite
{
    Q_OBJECT
public:
    static TaskCompositePtr create(std::function<bool ()> startFunc);
    TaskImmediateLambda(std::function<bool()> startFunc);
    void start() override;
private:
    std::function<bool()> m_startFunc;
};

#endif // TASKIMMEDIATELAMBDA_H
