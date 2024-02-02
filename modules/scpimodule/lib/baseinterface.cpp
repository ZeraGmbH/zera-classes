#include "baseinterface.h"
#include "scpimodule.h"

namespace SCPIMODULE
{

cBaseInterface::cBaseInterface(cSCPIModule* module, cSCPIInterface *iface)
    :m_pModule(module), m_pSCPIInterface(iface)
{
}

}
