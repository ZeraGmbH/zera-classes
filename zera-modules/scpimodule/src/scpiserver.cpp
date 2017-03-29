#include <QList>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QHostAddress>
#include <QTcpSocket>

#include <QtSerialPort/QSerialPort>
//#include <QtSerialPort/QSerialPortInfo>

#include "debug.h"
#include "errormessages.h"
#include "scpimoduleconfigdata.h"
#include "scpimodule.h"
#include "scpiserver.h"
#include "scpiethclient.h"
#include "scpiserialclient.h"
#include "scpiinterface.h"
#include "moduleinterface.h"
#include "interfaceinterface.h"
#include "statusinterface.h"
#include "ieee4882interface.h"
#include "scpicmdinfo.h"
#include "scpimoduleconfigdata.h"


namespace SCPIMODULE
{


cSCPIServer::cSCPIServer(cSCPIModule *module, cSCPIModuleConfigData &configData)
    : m_pModule(module), m_ConfigData(configData)
{
    m_bSerial = false;
    m_bActive = false;

    m_pSCPIInterface = new cSCPIInterface(m_ConfigData.m_sDeviceName); // our scpi interface with cmd interpreter

    m_pModuleInterface = new cModuleInterface(m_pModule, m_pSCPIInterface); // the modules interface
    m_pInterfaceInterface = new cInterfaceInterface(m_pModule, m_pSCPIInterface); // the interfaces interface
    m_pStatusInterface = new cStatusInterface(m_pModule, m_pSCPIInterface); // the scpi status interface
    m_pIEEE488Interface = new cIEEE4882Interface(m_pModule, m_pSCPIInterface); // the ieee448-2 interface

    m_pSerial = 0;
    m_pSerialClient = 0;

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
    if (m_pSerial)
        delete m_pSerial;
    if (m_pSerialClient)
        delete m_pSerialClient;
}


void cSCPIServer::generateInterface()
{
    // at the moment we have no interface
}


void cSCPIServer::deleteInterface()
{
    // so we have nothing to delete
}


cModuleInterface *cSCPIServer::getModuleInterface()
{
    return m_pModuleInterface;
}


void cSCPIServer::addSCPIClient()
{
    QTcpSocket* socket = m_pTcpServer->nextPendingConnection();
    cSCPIEthClient* client = new cSCPIEthClient(socket, m_pModule, m_ConfigData, m_pSCPIInterface); // each client our interface;
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

    if (!noError)
    {
        emit errMsg((tr(interfacejsonErrMsg)));
#ifdef DEBUG
        qDebug() << interfacejsonErrMsg;
#endif
    }

    noError = noError && m_pTcpServer->listen(QHostAddress(QHostAddress::AnyIPv4), m_ConfigData.m_InterfaceSocket.m_nPort);

    if(!noError)
    {
        emit errMsg((tr(interfaceETHErrMsg)));
#ifdef DEBUG
        qDebug() << interfaceETHErrMsg;
#endif
    }

    if (m_ConfigData.m_SerialDevice.m_nOn == 1)
    {
        connect(&m_SerialTestTimer, SIGNAL(timeout()), this, SLOT(testSerial()));
        m_SerialTestTimer.start(2000);
    }

    if (noError)
        emit activationContinue();
    else
        emit activationError();

}


void cSCPIServer::activationDone()
{
    m_bActive = true;
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
    if (m_bSerial)
        m_pSerial->close();

    emit deactivationContinue();
}


void cSCPIServer::deactivationDone()
{
    m_bActive = false;
    emit deactivated();
}


void cSCPIServer::testSerial()
{
    if (m_bActive)
    {
        QFile devFile;
        devFile.setFileName(m_ConfigData.m_SerialDevice.m_sDevice);
        if (devFile.exists())
        {
            if (!m_bSerial)
            {
    #ifdef DEBUG
            qDebug() << "serial client connected";
    #endif
                m_pSerial=new QSerialPort();
                m_pSerial->setPortName(m_ConfigData.m_SerialDevice.m_sDevice);
                if (m_pSerial->open(QIODevice::ReadWrite))
                {
                    m_pSerial->setBaudRate(m_ConfigData.m_SerialDevice.m_nBaud);
                    m_pSerial->setDataBits((QSerialPort::DataBits)m_ConfigData.m_SerialDevice.m_nDatabits);
                    m_pSerial->setStopBits((QSerialPort::StopBits)m_ConfigData.m_SerialDevice.m_nStopbits);
                    m_pSerial->setParity(QSerialPort::NoParity);
                    m_pSerial->setFlowControl(QSerialPort::NoFlowControl);

                    m_pSerialClient = new cSCPISerialClient(m_pSerial, m_pModule, m_ConfigData, m_pSCPIInterface);
                    m_SCPIClientList.append(m_pSerialClient);
                    if (m_SCPIClientList.count() == 1)
                        m_pSerialClient->setAuthorisation(true);

                    m_bSerial = true;

                }
                else
                {
                    delete m_pSerial;
                }
            }
        }
        else
        {
            if (m_bSerial) // in case we already had serial connection
            {
    #ifdef DEBUG
            qDebug() << "serial client disconnected";
    #endif
                m_bSerial = false;
                deleteSCPIClient(m_pSerialClient);
                delete m_pSerialClient;
                m_pSerial->close();
                delete m_pSerial;
                m_pSerialClient = 0;
                m_pSerial = 0;
            }
        }
    }
}

}
