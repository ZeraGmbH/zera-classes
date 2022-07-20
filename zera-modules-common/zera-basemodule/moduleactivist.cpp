#include "moduleactivist.h"

void cModuleActivist::activate()
{
    m_activationMachine.start();
}

void cModuleActivist::deactivate()
{
    m_deactivationMachine.start();
}
