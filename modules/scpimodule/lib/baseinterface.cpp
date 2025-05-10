#include "baseinterface.h"
#include "scpimodule.h"

namespace SCPIMODULE
{

int cBaseInterface::m_interfaceCount = 0;

cBaseInterface::cBaseInterface(cSCPIModule* module, cSCPIInterface *iface) :
    m_pModule(module),
    m_pSCPIInterface(iface)
{
    m_interfaceCount++;
}

cBaseInterface::~cBaseInterface()
{
    m_interfaceCount--;
}

int cBaseInterface::getInstanceCount()
{
    return m_interfaceCount;
}

}
