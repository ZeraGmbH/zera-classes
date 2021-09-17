#ifndef BASEMEASCHANNEL_H
#define BASEMEASCHANNEL_H

#include <QObject>
#include <QByteArray>
#include <QHash>
#include <QVariant>

#include "moduleactivist.h"
#include "socket.h"

class cSCPIConnection;
class QString;

namespace Zera {
namespace Proxy {
    class cProxy;
}
namespace  Server {
    class cRMInterface;
    class cPCBInterface;
}
}

class cBaseModule;

class cBaseMeasChannel: public cModuleActivist
{
    Q_OBJECT

public:
    cBaseMeasChannel(Zera::Proxy::cProxy* proxy, cSocket* rmsocket, cSocket* pcbsocket, QString name, quint8 chnnr);
    virtual ~cBaseMeasChannel(){}
    virtual void generateInterface() = 0; // here we export our interface (entities)

    quint8 getDSPChannelNr();
    quint16 getSampleRate();
    QString getName();
    QString getAlias();
    QString getUnit();

protected:
    Zera::Proxy::cProxy* m_pProxy;
    cSocket* m_pRMSocket; // the sockets we can connect to
    cSocket* m_pPCBServerSocket;
    QString m_sName; // the channel's system name
    quint8 m_nChannelNr; // the number of our channel for naming purpose

    Zera::Server::cRMInterface* m_pRMInterface;
    Zera::Server::cPCBInterface* m_pPCBInterface;

    // the things we have to query from our pcb server
    quint8 m_nDspChannel; // the dsp's channel where samples come in
    quint16 m_nSampleRate; // the systems sample rate
    QString m_sAlias; // the channel' alias
    QString m_sUnit;
    QString m_sDescription; // we take description from resource manager
    quint16 m_nPort; // and also the port adress of pcb server
    QHash<quint32, int> m_MsgNrCmdList;

protected slots:
    virtual void catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer) = 0;
};

#endif // BASEMEASCHANNEL_H
