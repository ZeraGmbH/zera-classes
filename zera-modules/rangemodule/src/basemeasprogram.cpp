#include "basemeasprogram.h"


cBaseMeasProgram::cBaseMeasProgram(VeinPeer *peer, Zera::Server::cDSPInterface* iface, cSocket* rmsocket, QStringList chnlist)
    :m_pPeer(peer), m_pDSPIFace(iface), m_pRMSocket(rmsocket), m_ChannelList(chnlist)
{
}


cBaseMeasProgram::~cBaseMeasProgram()
{
}




