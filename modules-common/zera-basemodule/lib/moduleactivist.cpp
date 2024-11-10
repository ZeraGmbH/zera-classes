#include "moduleactivist.h"

cModuleActivist::cModuleActivist(QString notifyInfo) :
    m_notifyInfo(notifyInfo)
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
    qWarning("%s reported error: %s", qPrintable(m_notifyInfo), qPrintable(value.toString()));
}

void cModuleActivist::notifyActivationError(QVariant value)
{
    notifyError(value);
    emit activationError();
}

bool cModuleActivist::handleFinishCallback(int cmdNumber, quint8 reply, QVariant answer)
{
    auto iter = m_cmdFinishCallbacks.constFind(cmdNumber);
    if(iter != m_cmdFinishCallbacks.constEnd()) {
        iter.value()(reply, answer);
    }
    return false;
}
