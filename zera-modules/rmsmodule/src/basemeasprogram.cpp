#include "basemeasprogram.h"

namespace RMSMODULE
{

cBaseMeas2Program::cBaseMeas2Program(Zera::Proxy::cProxy* proxy, VeinPeer *peer, Zera::Server::cDSPInterface* iface)
    :m_pProxy(proxy), m_pPeer(peer), m_pDSPIFace(iface)
{
}


cBaseMeas2Program::~cBaseMeas2Program()
{
}


void cBaseMeas2Program::monitorConnection()
{
    m_nConnectionCount--;
    if (m_nConnectionCount == 0)
        emit activationContinue();
}

}


