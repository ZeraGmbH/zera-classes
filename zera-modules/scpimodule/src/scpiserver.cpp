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

#include <veinmoduleparameter.h>
#include <veinmoduleactvalue.h>
#include <modulevalidator.h>
#include <boolvalidator.h>

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

constexpr int serialPollTimerPeriod = 1000;

cSCPIServer::cSCPIServer(cSCPIModule *module, cSCPIModuleConfigData &configData)
    : m_pModule(module), m_ConfigData(configData)
{
    m_bSerialScpiActive = false;
    m_bActive = false;

    m_pSCPIInterface = new cSCPIInterface(m_ConfigData.m_sDeviceName); // our scpi interface with cmd interpreter

    m_pModuleInterface = new cModuleInterface(m_pModule, m_pSCPIInterface); // the modules interface
    m_pInterfaceInterface = new cInterfaceInterface(m_pModule, m_pSCPIInterface); // the interfaces interface
    m_pStatusInterface = new cStatusInterface(m_pModule, m_pSCPIInterface); // the scpi status interface
    m_pIEEE488Interface = new cIEEE4882Interface(m_pModule, m_pSCPIInterface); // the ieee448-2 interface

    m_pSerialPort = nullptr;
    m_pSerialClient = nullptr;

    m_pTcpServer = new QTcpServer();
    m_pTcpServer->setMaxPendingConnections(m_ConfigData.m_nClients);
    connect(m_pTcpServer, &QTcpServer::newConnection, this, &cSCPIServer::addSCPIClient);
    connect(m_pTcpServer, &QTcpServer::acceptError, this, &cSCPIServer::TCPError);

    m_setupTCPServerState.addTransition(this, &cSCPIServer::activationContinue, &m_activationDoneState);
    m_activationMachine.addState(&m_setupTCPServerState);
    m_activationMachine.addState(&m_activationDoneState);
    m_activationMachine.setInitialState(&m_setupTCPServerState);
    connect(&m_setupTCPServerState, &QState::entered, this, &cSCPIServer::setupTCPServer);
    connect(&m_activationDoneState, &QState::entered, this, &cSCPIServer::activationDone);

    m_shutdownTCPServerState.addTransition(this, &cSCPIServer::deactivationContinue, &m_deactivationDoneState);
    m_deactivationMachine.addState(&m_shutdownTCPServerState);
    m_deactivationMachine.addState(&m_deactivationDoneState);
    m_deactivationMachine.setInitialState(&m_shutdownTCPServerState);
    connect(&m_shutdownTCPServerState, &QState::entered, this, &cSCPIServer::shutdownTCPServer);
    connect(&m_deactivationDoneState, &QState::entered, this, &cSCPIServer::deactivationDone);
}


cSCPIServer::~cSCPIServer()
{
    delete m_pSCPIInterface;
    delete m_pTcpServer;
    if (m_pSerialPort) {
        delete m_pSerialPort;
    }
}


void cSCPIServer::generateInterface()
{
    QString key;
    m_pVeinParamSerialOn = new cVeinModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                                       key = QString("PAR_SerialScpiActive"),
                                                       QString("Enable/disable serial SCPI"),
                                                       QVariant(m_bSerialScpiActive));
    m_pVeinParamSerialOn->setValidator(new cBoolValidator());
    connect(m_pVeinParamSerialOn, &cVeinModuleParameter::sigValueChanged, this, &cSCPIServer::newSerialOn);
    m_pModule->veinModuleParameterHash[key] = m_pVeinParamSerialOn; // auto delete / meta-data / scpi


    m_pVeinSerialScpiDevFileName = new cVeinModuleActvalue(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                                    QString("ACT_SerialScpiDeviceFile"),
                                                    QString("Device file name for serial SCPI"),
                                                    QVariant(m_ConfigData.m_SerialDevice.m_sDevice) );
    m_pModule->veinModuleActvalueList.append(m_pVeinSerialScpiDevFileName); // auto delete / meta-data / scpi
}


cModuleInterface *cSCPIServer::getModuleInterface()
{
    return m_pModuleInterface;
}

void cSCPIServer::createSerialScpi()
{
    if (!m_bSerialScpiActive) {
        m_pSerialPort = new QSerialPort();
        m_pSerialPort->setPortName(m_ConfigData.m_SerialDevice.m_sDevice);
        if (m_pSerialPort->open(QIODevice::ReadWrite)) {
            m_pSerialPort->setBaudRate(m_ConfigData.m_SerialDevice.m_nBaud);
            m_pSerialPort->setDataBits((QSerialPort::DataBits)m_ConfigData.m_SerialDevice.m_nDatabits);
            m_pSerialPort->setStopBits((QSerialPort::StopBits)m_ConfigData.m_SerialDevice.m_nStopbits);
            m_pSerialPort->setParity(QSerialPort::NoParity);
            m_pSerialPort->setFlowControl(QSerialPort::NoFlowControl);

            m_pSerialClient = new cSCPISerialClient(m_pSerialPort, m_pModule, m_ConfigData, m_pSCPIInterface);
            m_SCPIClientList.append(m_pSerialClient);
            if (m_SCPIClientList.count() == 1) {
                m_pSerialClient->setAuthorisation(true);
            }
            m_bSerialScpiActive = true;
        }
        else {
            delete m_pSerialPort;
        }
    }
    m_pVeinParamSerialOn->setValue(m_bSerialScpiActive);
}

void cSCPIServer::destroySerialScpi()
{
    if (m_bSerialScpiActive) {
        m_bSerialScpiActive = false;
        deleteSCPIClient(m_pSerialClient);
        delete m_pSerialClient;
        m_pSerialPort->close();
        delete m_pSerialPort;
        m_pSerialClient = nullptr;
        m_pSerialPort = nullptr;
    }
    m_pVeinParamSerialOn->setValue(m_bSerialScpiActive);
}


void cSCPIServer::addSCPIClient()
{
    QTcpSocket* socket = m_pTcpServer->nextPendingConnection();
    cSCPIEthClient* client = new cSCPIEthClient(socket, m_pModule, m_ConfigData, m_pSCPIInterface); // each client our interface;
    connect(client,& cSCPIEthClient::destroyed, this, &cSCPIServer::deleteSCPIClient);
    m_SCPIClientList.append(client);
    if (m_SCPIClientList.count() == 1) {
        client->setAuthorisation(true);
    }
}


void cSCPIServer::deleteSCPIClient(QObject *obj)
{
    m_SCPIClientList.removeAll(static_cast<cSCPIClient*>(obj));
    if (m_SCPIClientList.count() > 0) {
        m_SCPIClientList.at(0)->setAuthorisation(true);
    }
}


void cSCPIServer::TCPError(QAbstractSocket::SocketError)
{
}


void cSCPIServer::setupTCPServer()
{
    // before we can call listen we must set up a valid interface that clients can connect to
    bool noError = m_pModuleInterface->setupInterface();
    noError = noError && m_pInterfaceInterface->setupInterface();
    noError = noError && m_pStatusInterface->setupInterface();
    noError = noError && m_pIEEE488Interface->setupInterface();

    if (!noError) {
        emit errMsg((tr(interfacejsonErrMsg)));
#ifdef DEBUG
        qDebug() << interfacejsonErrMsg;
#endif
    }

    noError = noError && m_pTcpServer->listen(QHostAddress(QHostAddress::AnyIPv4), m_ConfigData.m_InterfaceSocket.m_nPort);

    if(!noError) {
        emit errMsg((tr(interfaceETHErrMsg)));
#ifdef DEBUG
        qDebug() << interfaceETHErrMsg;
#endif
    }

    if (m_ConfigData.m_SerialDevice.m_nOn == 1) {
        connect(&m_SerialTestTimer, &QTimer::timeout, this, &cSCPIServer::testSerial);
        m_SerialTestTimer.start(serialPollTimerPeriod);
    }

    if (noError) {
        emit activationContinue();
    }
    else {
        emit activationError();
    }
}


void cSCPIServer::activationDone()
{
    m_bActive = true;
    emit activated();
}


void cSCPIServer::shutdownTCPServer()
{
    if (m_bSerialScpiActive) {
        destroySerialScpi();
    }
    for(auto client : m_SCPIClientList) {
        delete client;
    }
    m_SCPIClientList.clear();
    m_pTcpServer->close();
    emit deactivationContinue();
}


void cSCPIServer::deactivationDone()
{
    m_bActive = false;
    emit deactivated();
}


void cSCPIServer::testSerial()
{
    if (m_bActive) {
        QFile deviceFile;
        deviceFile.setFileName(m_ConfigData.m_SerialDevice.m_sDevice);
        if (deviceFile.exists()) {
            createSerialScpi();
        }
        else {
            destroySerialScpi();
        }
    }
}

void cSCPIServer::newSerialOn(QVariant serialOn)
{
    if(serialOn.toBool()) {
        createSerialScpi();
        if(!m_SerialTestTimer.isActive()) {
            m_SerialTestTimer.start(serialPollTimerPeriod);
        }
    }
    else {
        destroySerialScpi();
        if(m_SerialTestTimer.isActive()) {
            m_SerialTestTimer.stop();
        }
    }
}

}
