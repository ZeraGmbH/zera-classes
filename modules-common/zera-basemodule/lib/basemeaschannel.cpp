#include "basemeaschannel.h"

cBaseMeasChannel::cBaseMeasChannel(NetworkConnectionInfo pcbsocket,
                                   VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory,
                                   ChannelRangeObserver::ChannelPtr channelObserver,
                                   quint8 chnnr,
                                   QString moduleChannelInfo) :
    cModuleActivist(moduleChannelInfo),
    m_pcbNetworkInfo(pcbsocket),
    m_tcpNetworkFactory(tcpNetworkFactory),
    m_channelObserver(channelObserver),
    m_nChannelNr(chnnr)
{
}

quint8 cBaseMeasChannel::getDSPChannelNr()
{
    return m_channelObserver->m_dspChannel;
}

QString cBaseMeasChannel::getMName()
{
    return m_channelObserver->getMName();
}

QString cBaseMeasChannel::getAlias()
{
    return m_sAlias;
}

QString cBaseMeasChannel::getUnit()
{
    return m_sUnit;
}

