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
#include "scpicmdinfo.h"
#include "scpimoduleconfigdata.h"

namespace SCPIMODULE
{


cSCPIServer::cSCPIServer(cSCPIModule *module, VeinPeer *peer, cSCPIModuleConfigData &configData)
    : m_pModule(module), m_pPeer(peer), m_ConfigData(configData)
{
    m_pSCPIInterface = new cSCPIInterface(m_ConfigData.m_sDeviceName); // our scpi cmd interface

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
    cSCPIClient* client = new cSCPIClient(socket, m_pSCPIInterface); // each client gets his own interface;
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
    // before we can call listen we must set up a valid interface clients can connect to
    cSCPICmdInfo scpiCmdInfo;
    bool error;

    error = false;
    QList<VeinPeer *> peerList;
    VeinHub *hub = m_pPeer->getHub();
    peerList = hub->listPeers();

    for (int i = 0; i < peerList.count(); i++)
    {
        QJsonDocument jsonDoc; // we parse over all moduleinterface entities
        VeinEntity* entity = peerList.at(i)->getEntityByName(QString("INF_ModuleInterface"));

        if (entity != 0) // modulemangers and interfaces do not export INF_ModuleInterface
        {
            jsonDoc = QJsonDocument::fromBinaryData(entity->getValue().toByteArray());

            if ( !jsonDoc.isNull() && jsonDoc.isObject() )
            {
                VeinPeer* peer;
                QJsonObject jsonObj;
#ifdef DEBUG
                qDebug() << jsonDoc;
#endif
                jsonObj = jsonDoc.object();
                scpiCmdInfo.scpiModuleName = jsonObj["SCPIModuleName"].toString();
                peer = hub->getPeerByName(jsonObj["VeinPeer"].toString());
                scpiCmdInfo.peer = peer;

                QJsonArray jsonEntityArr = jsonObj["Entities"].toArray();

                for (int j = 0; j < jsonEntityArr.count(); j++)
                {
                    QJsonObject jsonEntityObj;
                    jsonEntityObj = jsonEntityArr[j].toObject();
                    QJsonValue jsonVal;
                    jsonVal = jsonEntityObj["Name"];
                    QString s = jsonVal.toString();
                    scpiCmdInfo.entity = peer->getEntityByName(s);
                    //scpiCmdInfo.entity = peer->getEntityByName(jsonEntityObj["Name"].toString());
                    scpiCmdInfo.scpiModel = jsonEntityObj["SCPI"].toArray()[0].toString();
                    scpiCmdInfo.cmdNode = jsonEntityObj["SCPI"].toArray()[1].toString();
                    scpiCmdInfo.type = jsonEntityObj["SCPI"].toArray()[2].toString();
                    scpiCmdInfo.unit = jsonEntityObj["SCPI"].toArray()[3].toString();

                    if (scpiCmdInfo.type != "0") // we have to add this entity to our interface
                        m_pSCPIInterface->addSCPICommand(scpiCmdInfo);

                }
            }
            else
            {
                //error = true; temp. we set no error
                //break;
            }
        }
    }

    error |= !m_pTcpServer->listen(QHostAddress(QHostAddress::AnyIPv4), m_ConfigData.m_InterfaceSocket.m_nPort);

    if (error)
    {
        emit errMsg((tr(interfacejsonErrMsg)));
#ifdef DEBUG
        qDebug() << interfacejsonErrMsg;
#endif
        emit activationError();
    }
    else

        emit activationContinue();
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
