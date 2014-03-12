#ifndef BASEMEASCHANNEL_H
#define BASEMEASCHANNEL_H

#include <QObject>
#include <QByteArray>
#include <QHash>
#include <QVariant>

#include "moduleacitvist.h"
#include "socket.h"

class cSCPIConnection;
class cBaseModule;
class QString;
class VeinEntity;
class VeinPeer;

namespace Zera {
namespace Proxy {
    class cProxy;
}
namespace  Server {
    class cRMInterface;
    class cPCBInterface;
}
}


class cBaseMeasChannel: public cModuleActivist
{
    Q_OBJECT

public:
    cBaseMeasChannel(Zera::Proxy::cProxy* proxy, VeinPeer* peer, cSocket* rmsocket, cSocket* pcbsocket, QString name, quint8 chnnr);
    virtual ~cBaseMeasChannel(){}
    quint8 getDSPChannelNr();
    QString getName();
    QString getAlias();

public slots:
    virtual void activate() = 0; // here we query our properties and activate ourself
    virtual void deactivate() = 0; // what do you think ? yes you're right

protected:
    Zera::Proxy::cProxy* m_pProxy;
    VeinPeer* m_pPeer; // the peer where we set our entities
    cSocket* m_pRMSocket; // the sockets we can connect to
    cSocket* m_pPCBServerSocket;
    QString m_sName; // the channel's system name
    quint8 m_nChannelNr; // the number of our channel for naming purpose

    Zera::Server::cRMInterface* m_pRMInterface;
    Zera::Server::cPCBInterface* m_pPCBInterface;

    // the things we have to query from our pcb server
    quint8 m_nDspChannel; // the dsp's channel where samples come in
    QString m_sAlias; // the channel' alias
    QString m_sDescription; // we take description from resource manager
    quint16 m_nPort; // and also the port adress of pcb server

    QHash<quint32, int> m_MsgNrCmdList;
    virtual void generateInterface() = 0; // here we export our interface (entities)
    virtual void deleteInterface() = 0; // we delete interface in case of reconfiguration

protected slots:
    virtual void catchInterfaceAnswer(quint32 msgnr, QVariant answer) = 0;
};

#endif // BASEMEASCHANNEL_H
