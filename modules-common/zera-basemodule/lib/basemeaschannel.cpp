#include "basemeaschannel.h"

cBaseMeasChannel::cBaseMeasChannel(NetworkConnectionInfo pcbsocket,
                                   VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory,
                                   QString channelName, quint8 chnnr,
                                   QString moduleChannelInfo) :
    cModuleActivist(moduleChannelInfo),
    m_pcbNetworkInfo(pcbsocket),
    m_tcpNetworkFactory(tcpNetworkFactory),
    m_sName(channelName),
    m_nChannelNr(chnnr)
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

