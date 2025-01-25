#include "basemeaschannel.h"

cBaseMeasChannel::cBaseMeasChannel(NetworkConnectionInfo pcbsocket,
                                   VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory,
                                   ChannelRangeObserver::ChannelPtr channelObserver,
                                   QString moduleChannelInfo) :
    cModuleActivist(moduleChannelInfo),
    m_pcbNetworkInfo(pcbsocket),
    m_tcpNetworkFactory(tcpNetworkFactory),
    m_channelObserver(channelObserver)
{
}

quint8 cBaseMeasChannel::getDSPChannelNr() const
{
    return m_channelObserver->getDspChannel();
}

QString cBaseMeasChannel::getMName() const
{
    return m_channelObserver->getMName();
}

QString cBaseMeasChannel::getAlias()
{
    return m_channelObserver->getAlias();
}

QString cBaseMeasChannel::getUnit()
{
    return m_channelObserver->getUnit();
}
