#include "taskactivationstatemachinewrapper.h"

TaskActivationStateMachineWrapperPtr TaskActivationStateMachineWrapper::create(QStateMachine *wrappedStateMachine)
{
    return std::make_unique<TaskActivationStateMachineWrapper>(wrappedStateMachine);
}

TaskActivationStateMachineWrapper::TaskActivationStateMachineWrapper(QStateMachine *wrappedStateMachine) :
    m_wrappedStateMachine(wrappedStateMachine)
{
}

void TaskActivationStateMachineWrapper::start()
{
    m_wrappedStateMachine->start();
}

void TaskActivationStateMachineWrapper::onActivationFinished()
{
    finishTaskQueued(true);
}
