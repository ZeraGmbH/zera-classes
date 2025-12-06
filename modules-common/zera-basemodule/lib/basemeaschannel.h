#ifndef BASEMEASCHANNEL_H
#define BASEMEASCHANNEL_H

#include "moduleactivist.h"
#include "cro_channel.h"
#include "networkconnectioninfo.h"
#include <pcbinterface.h>
#include <abstracttcpnetworkfactory.h>

class cBaseMeasChannel: public cModuleActivist
{
    Q_OBJECT

public:
    cBaseMeasChannel(NetworkConnectionInfo pcbsocket,
                     VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory,
                     ChannelRangeObserver::ChannelPtr channelObserver,
                     QString moduleChannelInfo);

    quint8 getDSPChannelNr() const;
    QString getMName() const;
    QString getAlias();
    QString getUnit();
    ChannelRangeObserver::ChannelPtr getChannelObserver();

protected:
    NetworkConnectionInfo m_pcbNetworkInfo;
    VeinTcp::AbstractTcpNetworkFactoryPtr m_tcpNetworkFactory;
    ChannelRangeObserver::ChannelPtr m_channelObserver;

    Zera::PcbInterfacePtr m_pcbInterface;

    QHash<quint32, int> m_MsgNrCmdList;

protected slots:
    virtual void catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer) = 0;
};

#endif // BASEMEASCHANNEL_H
