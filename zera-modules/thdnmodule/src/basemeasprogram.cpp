#include "basemeasprogram.h"

namespace THDNMODULE
{

cBaseMeasProgram::cBaseMeasProgram(Zera::Proxy::cProxy* proxy, VeinPeer *peer)
    :m_pProxy(proxy), m_pPeer(peer)
{
}


cBaseMeasProgram::~cBaseMeasProgram()
{
}


void cBaseMeasProgram::monitorConnection()
{
    m_nConnectionCount--;
    if (m_nConnectionCount == 0)
        emit activationContinue();
}

}


