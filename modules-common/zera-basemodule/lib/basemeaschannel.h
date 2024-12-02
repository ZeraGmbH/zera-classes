#ifndef BASEMEASCHANNEL_H
#define BASEMEASCHANNEL_H

#include "moduleactivist.h"
#include "networkconnectioninfo.h"
#include <pcbinterface.h>
#include <abstracttcpnetworkfactory.h>

class cBaseMeasChannel: public cModuleActivist
{
    Q_OBJECT

public:
    cBaseMeasChannel(NetworkConnectionInfo pcbsocket,
                     VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory,
                     QString channelName, quint8 chnnr,
                     QString moduleChannelInfo);

    quint8 getDSPChannelNr();
    quint16 getSampleRate();
    QString getName();
    QString getAlias();
    QString getUnit();

protected:
    NetworkConnectionInfo m_pcbNetworkInfo;
    VeinTcp::AbstractTcpNetworkFactoryPtr m_tcpNetworkFactory;
    QString m_sName; // the channel's system name
    quint8 m_nChannelNr; // the number of our channel for naming purpose

    Zera::PcbInterfacePtr m_pcbInterface;

    // the things we have to query from our pcb server
    quint8 m_nDspChannel; // the dsp's channel where samples come in
    quint16 m_nSampleRate; // the systems sample rate
    QString m_sAlias; // the channel' alias
    QString m_sUnit;
    QString m_sDescription; // we take description from resource manager
    QHash<quint32, int> m_MsgNrCmdList;

protected slots:
    virtual void catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer) = 0;
};

#endif // BASEMEASCHANNEL_H
