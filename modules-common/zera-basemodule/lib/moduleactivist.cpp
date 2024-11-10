#include "moduleactivist.h"

cModuleActivist::cModuleActivist(QString notifyHeaderString) :
    m_notifyHeaderString(notifyHeaderString)
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
    qWarning("%s reported error: %s", qPrintable(m_notifyHeaderString), qPrintable(value.toString()));
}

bool cModuleActivist::handleFinishCallback(int cmdNumber, quint8 reply, QVariant answer)
{
    auto iter = m_cmdFinishCallbacks.constFind(cmdNumber);
    if(iter != m_cmdFinishCallbacks.constEnd()) {
        iter.value()(reply, answer);
    }
    return false;
}
