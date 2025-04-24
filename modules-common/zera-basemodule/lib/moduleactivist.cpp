#include "moduleactivist.h"

int cModuleActivist::m_instanceCount = 0;

cModuleActivist::cModuleActivist(QString notifyHeaderString) :
    m_notifyHeaderString(notifyHeaderString)
{
    m_instanceCount++;
}

cModuleActivist::~cModuleActivist()
{
    m_instanceCount--;
}

int cModuleActivist::getInstanceCount()
{
    return m_instanceCount;
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
