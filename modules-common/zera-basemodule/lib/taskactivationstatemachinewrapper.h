#ifndef TASKACTIVATIONSTATEMACHINEWRAPPER_H
#define TASKACTIVATIONSTATEMACHINEWRAPPER_H

#include <tasktemplate.h>
#include <QStateMachine>

class TaskActivationStateMachineWrapper;
typedef std::unique_ptr<TaskActivationStateMachineWrapper> TaskActivationStateMachineWrapperPtr;

class TaskActivationStateMachineWrapper : public TaskTemplate
{
    Q_OBJECT
public:
    static TaskActivationStateMachineWrapperPtr create(QStateMachine *wrappedStateMachine); // add timout decorator??
    TaskActivationStateMachineWrapper(QStateMachine *wrappedStateMachine);
    void start() override;
public slots:
    void onActivationFinished();
private:
    QStateMachine *m_wrappedStateMachine;
};


#endif // TASKACTIVATIONSTATEMACHINEWRAPPER_H
