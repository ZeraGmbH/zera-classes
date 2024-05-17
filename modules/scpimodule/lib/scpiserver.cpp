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

#include <vfmoduleparameter.h>
#include <vfmoduleactvalue.h>
#include <boolvalidator.h>

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
#include "scpimoduleconfigdata.h"
#include <sysinfo.h>

namespace SCPIMODULE
{

constexpr int serialPollTimerPeriod = 1000;

cSCPIServer::cSCPIServer(cSCPIModule *module, cSCPIModuleConfigData &configData) :
    m_pModule(module),
    m_ConfigData(configData)
{
    m_bSerialScpiActive = false;
    m_bActive = false;

    m_pSCPIInterface = new cSCPIInterface(m_ConfigData.m_sDeviceName); // our scpi interface with cmd interpreter
    m_pModuleInterface = new cModuleInterface(m_pModule, m_pSCPIInterface); // the modules interface
    m_pInterfaceInterface = new cInterfaceInterface(m_pModule, m_pSCPIInterface); // the interfaces interface
    m_pStatusInterface = new cStatusInterface(m_pModule, m_pSCPIInterface); // the scpi status interface
    m_pIEEE488Interface = new cIEEE4882Interface(m_pModule, m_pSCPIInterface); // the ieee448-2 interface

    m_pTcpServer = new TcpServerLimitedConn(m_ConfigData, m_SCPIClientList);
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
    // Commented are not deleted - but this requires deep investigations
    // we had our double-delete fiasco and things are ugly connected in here...
    /*delete m_pIEEE488Interface;
    delete m_pStatusInterface;
    delete m_pInterfaceInterface;
    delete m_pModuleInterface;*/
    delete m_pSCPIInterface;
    delete m_pTcpServer;
    deleteSerialPort();
}

void cSCPIServer::generateInterface()
{
    QString key;
    m_pVeinParamSerialOn = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->m_pModuleValidator,
                                                       key = QString("PAR_SerialScpiActive"),
                                                       QString("Enable/disable serial SCPI"),
                                                       QVariant(m_bSerialScpiActive));
    m_pVeinParamSerialOn->setValidator(new cBoolValidator());
    connect(m_pVeinParamSerialOn, &VfModuleParameter::sigValueChanged, this, &cSCPIServer::newSerialOn);
    m_pModule->m_veinModuleParameterMap[key] = m_pVeinParamSerialOn; // auto delete / meta-data / scpi

    m_pVeinSerialScpiDevFileName = new VfModuleActvalue(m_pModule->getEntityId(), m_pModule->m_pModuleValidator,
                                                    QString("ACT_SerialScpiDeviceFile"),
                                                    QString("Device file name for serial SCPI"),
                                                    QVariant(m_ConfigData.m_SerialDevice.m_sDevice) );
    m_pModule->veinModuleActvalueList.append(m_pVeinSerialScpiDevFileName); // auto delete / meta-data / scpi

    m_veinDevIface = new VfModuleActvalue(m_pModule->getEntityId(), m_pModule->m_pModuleValidator,
                                                     QString("ACT_DEV_IFACE"),
                                                     QString("SCPI interface description for current session"),
                                                     QVariant("") );
    m_pModule->veinModuleActvalueList.append(m_veinDevIface); // auto delete / meta-data / scpi

    m_optionalScpiQueue = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->m_pModuleValidator,
                                                         key = QString("PAR_OptionalScpiQueue"),
                                                         QString("Enable/disable order for SCPI commands"),
                                                         QVariant(m_ConfigData.m_enableScpiQueue.m_nActive));
    m_optionalScpiQueue->setValidator(new cBoolValidator());
    m_pModule->m_veinModuleParameterMap[key] = m_optionalScpiQueue;
}

cModuleInterface *cSCPIServer::getModuleInterface()
{
    return m_pModuleInterface;
}

void cSCPIServer::appendClient(cSCPIClient* client)
{
    m_SCPIClientList.append(client);
    if (m_SCPIClientList.count() == 1)
        client->setAuthorisation(true);
}

cSCPIInterface *cSCPIServer::getScpiInterface() const
{
    return m_pSCPIInterface;
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
            appendClient(m_pSerialClient);
            qInfo("Serial SCPI client connected / Active clients: %i", m_SCPIClientList.count());
            m_bSerialScpiActive = true;
        }
        else
            deleteSerialPort();
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
        deleteSerialPort();
        m_pSerialClient = nullptr;
        qInfo("Serial SCPI client disconnected / Active clients: %i", m_SCPIClientList.count());
    }
    m_pVeinParamSerialOn->setValue(m_bSerialScpiActive);
}

void cSCPIServer::deleteSerialPort()
{
    if(m_pSerialPort) {
        delete m_pSerialPort;
        m_pSerialPort = nullptr;
    }
}

void cSCPIServer::addSCPIClient()
{
    if(m_pTcpServer->hasPendingConnections()) {
        QTcpSocket* socket = m_pTcpServer->nextPendingConnection();
        cSCPIEthClient* client = new cSCPIEthClient(socket, m_pModule, m_ConfigData, m_pSCPIInterface); // each client our interface;
        appendClient(client);
        qInfo("Network SCPI client (%s) connected / Active clients: %i", qPrintable(client->getPeerAddress()), m_SCPIClientList.count());
        connect(client, &cSCPIEthClient::destroyed, this, &cSCPIServer::deleteSCPIClient);
     }
}

void cSCPIServer::deleteSCPIClient(QObject *obj)
{
    cSCPIEthClient* client = static_cast<cSCPIEthClient*>(obj);
    if(client) {
        m_SCPIClientList.removeAll(client);
        qInfo("Network SCPI client (%s) disconnected / Active clients: %i", qPrintable(client->getPeerAddress()), m_SCPIClientList.count());
        if(m_SCPIClientList.count() > 0)
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
    if (!noError)
        emit errMsg((tr(interfacejsonErrMsg)));

    noError = noError && m_pTcpServer->listen(QHostAddress(QHostAddress::AnyIPv4), m_ConfigData.m_InterfaceSocket.m_nPort);
    if(!noError)
        emit errMsg((tr(interfaceETHErrMsg)));

    if (m_ConfigData.m_SerialDevice.m_nOn == 1) {
        connect(&m_SerialTestTimer, &QTimer::timeout, this, &cSCPIServer::testSerial);
        m_SerialTestTimer.start(serialPollTimerPeriod);
    }
    if (noError)
        emit activationContinue();
    else
        emit activationError();
}

void cSCPIServer::activationDone()
{
    m_pSCPIInterface->checkAmbiguousShortNames();

    QMap<QString, QString> modelListBaseEntry({{"RELEASE", SysInfo::getReleaseNr()}});
    QString xml;
    m_pSCPIInterface->exportSCPIModelXML(xml, modelListBaseEntry);
    m_veinDevIface->setValue(xml);
    m_optionalScpiQueue->setValue(m_ConfigData.m_enableScpiQueue.m_nActive);
    m_bActive = true;
    connect(m_optionalScpiQueue, &VfModuleParameter::sigValueChanged, this, &cSCPIServer::controlScpiQueue);
    controlScpiQueue(QVariant(m_ConfigData.m_enableScpiQueue.m_nActive));
    emit activated();
}

void cSCPIServer::shutdownTCPServer()
{
    if (m_bSerialScpiActive)
        destroySerialScpi();
    for(auto client : qAsConst(m_SCPIClientList))
        delete client;
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
        if (deviceFile.exists())
            createSerialScpi();
        else
            destroySerialScpi();
    }
}

void cSCPIServer::newSerialOn(QVariant serialOn)
{
    bool on = serialOn.toBool();
    if(on) {
        createSerialScpi();
        if(!m_SerialTestTimer.isActive())
            m_SerialTestTimer.start(serialPollTimerPeriod);
    }
    else {
        destroySerialScpi();
        if(m_SerialTestTimer.isActive())
            m_SerialTestTimer.stop();
    }
    m_ConfigData.m_SerialDevice.m_nOn = on;
    emit m_pModule->parameterChanged();
}

void cSCPIServer::controlScpiQueue(QVariant scpiQueue)
{
    m_ConfigData.m_enableScpiQueue.m_nActive = scpiQueue.toInt();
    m_pSCPIInterface->setEnableQueue(scpiQueue.toBool());
    emit m_pModule->parameterChanged();
}
}
