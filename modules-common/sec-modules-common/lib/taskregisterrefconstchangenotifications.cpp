#include "taskregisterrefconstchangenotifications.h"
#include "taskregisternotifier.h"

void TaskRegisterRefConstChangeNotifications::startRegistrationTasks(Zera::PcbInterfacePtr pcbInterface, QList<PowerRefInput> refInputs)
{
    connect(&m_registerTasks, &TaskTemplate::sigFinish,
            this, &TaskRegisterRefConstChangeNotifications::onRegistrationFinish);
    for(const auto &refInput : refInputs) {
        m_registerTasks.addSub(TaskRegisterNotifier::create(
                                   pcbInterface,
                                   QString("SOURCE:%1:CONSTANT?").arg(refInput.refInputName),
                                   refInput.refInputInterruptNotifyId,
                                   REGISTER_TIMEOUT_MS));
    }
    m_registerTasks.start();
}

void TaskRegisterRefConstChangeNotifications::onRegistrationFinish(bool ok)
{
    if(ok)
        emit sigRegistrationOk();
    else
        emit sigRegistrationFail();
}
