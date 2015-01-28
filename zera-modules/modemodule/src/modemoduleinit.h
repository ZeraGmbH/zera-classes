#ifndef MODEMODULEINIT_H
#define MODEMODULEINIT_H

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


namespace MODEMODULE
{

namespace MODEMODINIT
{

enum modemoduleinitCmds
{
    sendrmident,
    readresourcetypes,
    readresource,
    readresourceinfo,
    claimresource,
    setmode,
    freeresource
};

}

class cModeModule;
class cModeModuleConfigData;


class cModeModuleInit: public cModuleActivist
{
    Q_OBJECT

public:
    cModeModuleInit(cModeModule* module, Zera::Proxy::cProxy* proxy, VeinPeer* peer, cModeModuleConfigData& configData);
    virtual ~cModeModuleInit();
    virtual void generateInterface(); // here we export our interface (entities)
    virtual void deleteInterface(); // we delete interface in case of reconfiguration


protected slots:
    virtual void catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer);

private:
    cModeModule* m_pModule; // the module we live in
    Zera::Proxy::cProxy* m_pProxy; // the proxy where we can get our connections
    VeinPeer* m_pPeer; // the peer where we set our entities
    cModeModuleConfigData& m_ConfigData;

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
    QState m_setModeState;
    QFinalState m_activationDoneState;

    // statemachine for deactivating
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
    void setMode();
    void activationDone();

    void freeResource();
    void deactivationDone();

    void catchChannelReply(quint32 msgnr);
};

}


#endif // MODEMODULEINIT_H
