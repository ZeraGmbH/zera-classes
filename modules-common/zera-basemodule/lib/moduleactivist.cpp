#include "moduleactivist.h"

cModuleActivist::cModuleActivist(QString moduleName) :
    m_moduleName(moduleName)
{
}

void cModuleActivist::activate()
{
    m_activationMachine.start();
}

void cModuleActivist::deactivate()
{
    m_deactivationMachine.start();
}

void cModuleActivist::notifyError(QVariant value)
{
    qWarning("Module %s reported error: %s", qPrintable(m_moduleName), qPrintable(value.toString()));
}

void cModuleActivist::notifyActivationError(QVariant value)
{
    notifyError(value);
    emit activationError();
}

void cModuleActivist::notifyDeactivationError(QVariant value)
{
    notifyError(value);
    emit deactivationError();
}

void cModuleActivist::notifyExecutionError(QVariant value)
{
    notifyError(value);
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
