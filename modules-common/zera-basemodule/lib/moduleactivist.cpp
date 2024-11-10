#include "moduleactivist.h"

void cModuleActivist::activate()
{
    m_activationMachine.start();
}

void cModuleActivist::deactivate()
{
    m_deactivationMachine.start();
}

void cModuleActivist::notifyActivationError(QVariant value)
{
    emit errMsg(value);
    emit activationError();
}

void cModuleActivist::notifyDeactivationError(QVariant value)
{
    emit errMsg(value);
    emit deactivationError();
}

void cModuleActivist::notifyExecutionError(QVariant value)
{
    emit errMsg(value);
    emit executionError();
}

bool cModuleActivist::handleFinishCallback(int cmdNumber, quint8 reply, QVariant answer)
{
    auto iter = m_cmdFinishCallbacks.constFind(cmdNumber);
    if(iter != m_cmdFinishCallbacks.constEnd()) {
        iter.value()(reply, answer);
    }
    return false;
}
