#include "basemeasprogram.h"


cBaseMeasProgram::cBaseMeasProgram(Zera::Proxy::cProxy* proxy, VeinPeer *peer, Zera::Server::cDSPInterface* iface, cSocket* rmsocket, QStringList chnlist)
    :m_pProxy(proxy), m_pPeer(peer), m_pDSPIFace(iface), m_pRMSocket(rmsocket), m_ChannelList(chnlist)
{
}


cBaseMeasProgram::~cBaseMeasProgram()
{
}




