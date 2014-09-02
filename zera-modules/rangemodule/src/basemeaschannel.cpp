#include "basemeaschannel.h"

namespace RANGEMODULE
{

cBaseMeasChannel::cBaseMeasChannel(Zera::Proxy::cProxy* proxy, VeinPeer *peer, cSocket* rmsocket, cSocket* pcbsocket, QString name, quint8 chnnr)
    :m_pProxy(proxy), m_pPeer(peer), m_pRMSocket(rmsocket), m_pPCBServerSocket(pcbsocket), m_sName(name), m_nChannelNr(chnnr)
{
}


quint8 cBaseMeasChannel::getDSPChannelNr()
{
    return m_nDspChannel;
}


quint16 cBaseMeasChannel::getSampleRate()
{
    return m_nSampleRate;
}


QString cBaseMeasChannel::getName()
{
    return m_sName;
}


QString cBaseMeasChannel::getAlias()
{
    return m_sAlias;
}


QString cBaseMeasChannel::getUnit()
{
    return m_sUnit;
}

}



