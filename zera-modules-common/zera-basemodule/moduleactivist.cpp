#include "moduleactivist.h"

void cModuleActivist::activate()
{
    m_activationMachine.start();
}

void cModuleActivist::deactivate()
{
    m_deactivationMachine.start();
}

void cModuleActivist::notifyActivationError(QVariant value, int dest)
{
    emit errMsg(value, dest);
    emit activationError();
}

void cModuleActivist::notifyDeactivationError(QVariant value, int dest)
{
    emit errMsg(value, dest);
    emit deactivationError();
}

void cModuleActivist::notifyExecutionError(QVariant value, int dest)
{
    emit errMsg(value, dest);
    emit executionError();
}
