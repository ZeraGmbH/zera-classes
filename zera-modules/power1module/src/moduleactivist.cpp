#include "moduleactivist.h"

namespace POWER1MODULE
{

void cModuleActivist::activate()
{
    m_activationMachine.start();
}


void cModuleActivist::deactivate()
{
    m_deactivationMachine.start();
}

}
