#include <QList>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QHostAddress>
#include <QTcpSocket>

#include <veinhub.h>
#include <veinpeer.h>
#include <veinentity.h>

#include "debug.h"
#include "errormessages.h"
#include "scpimodule.h"
#include "scpiserver.h"
#include "scpiclient.h"
#include "scpiinterface.h"
#include "moduleinterface.h"
#include "interfaceinterface.h"
#include "statusinterface.h"
#include "ieee4882interface.h"
#include "scpicmdinfo.h"
#include "scpimoduleconfigdata.h"


namespace SCPIMODULE
{


cSCPIServer::cSCPIServer(cSCPIModule *module, VeinPeer *peer, cSCPIModuleConfigData &configData)
    : m_pModule(module), m_pPeer(peer), m_ConfigData(configData)
{
    m_pSCPIInterface = new cSCPIInterface(m_ConfigData.m_sDeviceName); // our scpi interface with cmd interpreter
    m_pModuleInterface = new cModuleInterface(m_pPeer, m_pSCPIInterface); // the modules interface
    m_pInterfaceInterface = new cInterfaceInterface(m_pPeer, m_pSCPIInterface); // the interfaces interface
    m_pStatusInterface = new cStatusInterface(m_pPeer, m_pSCPIInterface);
    m_pIEEE488Interface = new cIEEE4882Interface(m_pPeer, m_pSCPIInterface);

    m_pTcpServer = new QTcpServer();
    m_pTcpServer->setMaxPendingConnections(m_ConfigData.m_nClients);
    connect(m_pTcpServer, SIGNAL(newConnection()), this, SLOT(addSCPIClient()));
    connect(m_pTcpServer, SIGNAL(acceptError(QAbstractSocket::SocketError)), this, SLOT(TCPError(QAbstractSocket::SocketError)));

    m_setupTCPServerState.addTransition(this, SIGNAL(activationContinue()), &m_activationDoneState);
    m_activationMachine.addState(&m_setupTCPServerState);
    m_activationMachine.addState(&m_activationDoneState);
    m_activationMachine.setInitialState(&m_setupTCPServerState);
    connect(&m_setupTCPServerState, SIGNAL(entered()), SLOT(setupTCPServer()));
    connect(&m_activationDoneState, SIGNAL(entered()), SLOT(activationDone()));

    m_shutdownTCPServerState.addTransition(this, SIGNAL(deactivationContinue()), &m_deactivationDoneState);
    m_deactivationMachine.addState(&m_shutdownTCPServerState);
    m_deactivationMachine.addState(&m_deactivationDoneState);
    m_deactivationMachine.setInitialState(&m_shutdownTCPServerState);
    connect(&m_shutdownTCPServerState, SIGNAL(entered()), SLOT(shutdownTCPServer()));
    connect(&m_deactivationDoneState, SIGNAL(entered()), SLOT(deactivationDone()));

}


cSCPIServer::~cSCPIServer()
{
    delete m_pSCPIInterface;
    delete m_pTcpServer;
}


void cSCPIServer::generateInterface()
{
    // at the moment we have no interface
}


void cSCPIServer::deleteInterface()
{
    // so we have nothing to delete
}


void cSCPIServer::exportInterface(QJsonArray &)
{
    // and nothing to export
}


void cSCPIServer::addSCPIClient()
{
    QTcpSocket* socket = m_pTcpServer->nextPendingConnection();
    cSCPIClient* client = new cSCPIClient(socket, m_pSCPIInterface); // each client our interface;
    connect(client, SIGNAL(destroyed(QObject*)), this, SLOT(deleteSCPIClient(QObject*)));
    m_SCPIClientList.append(client);
    if (m_SCPIClientList.count() == 1)
        client->setAuthorisation(true);

}


void cSCPIServer::deleteSCPIClient(QObject *obj)
{
    m_SCPIClientList.removeAll(static_cast<cSCPIClient*>(obj));
    if (m_SCPIClientList.count() > 0)
        m_SCPIClientList.at(0)->setAuthorisation(true);
}


void cSCPIServer::TCPError(QAbstractSocket::SocketError)
{

}


void cSCPIServer::setupTCPServer()
{
    bool noError;

    // before we can call listen we must set up a valid interface that clients can connect to

    noError = m_pModuleInterface->setupInterface();
    noError = noError && m_pInterfaceInterface->setupInterface();
    noError = noError && m_pStatusInterface->setupInterface();
    noError = noError && m_pIEEE488Interface->setupInterface();

    noError = noError && m_pTcpServer->listen(QHostAddress(QHostAddress::AnyIPv4), m_ConfigData.m_InterfaceSocket.m_nPort);

    if (noError)
    {
        emit activationContinue();
    }
    else
    {
        emit errMsg((tr(interfacejsonErrMsg)));
#ifdef DEBUG
        qDebug() << interfacejsonErrMsg;
#endif
        emit activationError();
    }

}


void cSCPIServer::activationDone()
{
    emit activated();
}


void cSCPIServer::shutdownTCPServer()
{
    cSCPIClient* client;

    for (int i = 0; i < m_SCPIClientList.count(); i++)
    {
        client = m_SCPIClientList.at(i);
        delete client;
    }

    m_pTcpServer->close();

    emit deactivationContinue();
}


void cSCPIServer::deactivationDone()
{
    emit deactivated();
}

}
