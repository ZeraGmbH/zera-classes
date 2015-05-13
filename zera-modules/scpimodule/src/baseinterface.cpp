#include "baseinterface.h"

namespace SCPIMODULE
{

cBaseInterface::cBaseInterface(VeinPeer *peer, cSCPIInterface *iface)
    :m_pPeer(peer), m_pSCPIInterface(iface)
{
}

}
