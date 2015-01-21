#ifndef REFERENCEMODULEINIT_H
#define REFERENCEMODULEINIT_H

#include <QObject>
#include <QList>
#include <QStateMachine>
#include <QState>
#include <QFinalState>

#include "moduleactivist.h"

class VeinPeer;

namespace Zera
{
namespace Proxy
{
    class cProxy;
    class cProxyClient;
}
namespace  Server {
    class cRMInterface;
    class cPCBInterface;
}
}


class QStateMachine;
class QState;
class QFinalState;


namespace REFERENCEMODULE
{

namespace REFMODINIT
{

enum referencemoduleinitCmds
{
    sendrmident,
    readresourcetypes,
    readresource,
    readresourceinfo,
    claimresource,
    setreferencemode,

    setacmode,
    freeresource
};

}

class cReferenceModule;
class cReferenceModuleConfigData;


class cReferenceModuleInit: public cModuleActivist
{
    Q_OBJECT

public:
    cReferenceModuleInit(cReferenceModule* module, Zera::Proxy::cProxy* proxy, VeinPeer* peer, cReferenceModuleConfigData& configData);
    virtual ~cReferenceModuleInit();
    virtual void generateInterface(); // here we export our interface (entities)
    virtual void deleteInterface(); // we delete interface in case of reconfiguration


protected slots:
    virtual void catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer);

private:
    cReferenceModule* m_pModule; // the module we live in
    Zera::Proxy::cProxy* m_pProxy; // the proxy where we can get our connections
    VeinPeer* m_pPeer; // the peer where we set our entities
    cReferenceModuleConfigData& m_ConfigData;

    QHash<quint32, int> m_MsgNrCmdList;

    Zera::Server::cPCBInterface *m_pPCBInterface;
    Zera::Proxy::cProxyClient *m_pPCBClient;
    Zera::Server::cRMInterface* m_pRMInterface;
    Zera::Proxy::cProxyClient* m_pRMClient;

    QString m_sDescription;
    quint16 m_nPort; // the port adress of pcb server

    // statemachine for activating gets the following states
    QState m_resourceManagerConnectState;
    QState m_IdentifyState;
    QState m_readResourceTypesState; // we ask for a list of all resources
    QState m_readResourceState; // we look for our resource needed
    QState m_readResourceInfoState; // we look for resource specification
    QState m_claimResourceState;
    QState m_pcbserverConnectionState; // we try to get a connection to our pcb server
    QState m_setReferenceModeState;
    QFinalState m_activationDoneState;

    // statemachine for deactivating
    QState m_setACModeState;
    QState m_freeResourceState;
    QFinalState m_deactivationDoneState;

private slots:
    void resourceManagerConnect();
    void sendRMIdent();
    void readResourceTypes();
    void readResource();
    void readResourceInfo();
    void claimResource();
    void pcbserverConnect();
    void setReferenceMode();
    void activationDone();

    void setACMode();
    void freeResource();
    void deactivationDone();

    void catchChannelReply(quint32 msgnr);
};

}


#endif // REFERENCEMODULEINIT_H
