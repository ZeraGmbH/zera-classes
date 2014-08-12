#include "moduleactivist.h"

namespace RANGEMODULE
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
