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
#include <zenuxdeviceinfo.h>

namespace SCPIMODULE
{

constexpr int serialPollTimerPeriod = 1000;

cSCPIServer::cSCPIServer(cSCPIModule *module, cSCPIModuleConfigData &configData) :
    cModuleActivist(module->getVeinModuleName()),
    m_pModule(module),
    m_ConfigData(configData),
    m_scpiInterface(m_ConfigData.m_sDeviceName),
    m_moduleInterface(m_pModule, &m_scpiInterface),
    m_interfaceInterface(m_pModule, &m_scpiInterface),
    m_statusInterface(m_pModule, &m_scpiInterface),
    m_ieee488Interface(m_pModule, &m_scpiInterface)
{
    m_bSerialScpiActive = false;
    m_bActive = false;

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
    delete m_pTcpServer;
    deleteSerialPort();
}

void cSCPIServer::generateVeinInterface()
{
    QString key;
    m_pVeinParamSerialOn = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                                       key = QString("PAR_SerialScpiActive"),
                                                       QString("Enable/disable serial SCPI"),
                                                       QVariant(m_bSerialScpiActive));
    m_pVeinParamSerialOn->setValidator(new cBoolValidator());
    connect(m_pVeinParamSerialOn, &VfModuleParameter::sigValueChanged, this, &cSCPIServer::newSerialOn);
    m_pModule->m_veinModuleParameterMap[key] = m_pVeinParamSerialOn; // auto delete / meta-data / scpi

    m_pVeinSerialScpiDevFileName = new VfModuleComponent(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                                    QString("ACT_SerialScpiDeviceFile"),
                                                    QString("Device file name for serial SCPI"),
                                                    QVariant(m_ConfigData.m_SerialDevice.m_sDevice) );
    m_pModule->m_veinComponentsWithMetaAndScpi.append(m_pVeinSerialScpiDevFileName); // auto delete / meta-data / scpi

    m_veinDevIface = new VfModuleComponent(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                                     QString("ACT_DEV_IFACE"),
                                                     QString("SCPI interface description for current session"),
                                                     QVariant("") );
    m_pModule->m_veinComponentsWithMetaAndScpi.append(m_veinDevIface); // auto delete / meta-data / scpi

    m_optionalScpiQueue = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                                         key = QString("PAR_OptionalScpiQueue"),
                                                         QString("Enable/disable order for SCPI commands"),
                                                         QVariant(m_ConfigData.m_enableScpiQueue.m_nActive));
    m_optionalScpiQueue->setValidator(new cBoolValidator());
    m_pModule->m_veinModuleParameterMap[key] = m_optionalScpiQueue;
}

cModuleInterface *cSCPIServer::getModuleInterface()
{
    return &m_moduleInterface;
}

void cSCPIServer::appendClient(cSCPIClient* client)
{
    m_SCPIClientList.append(client);
    if (m_SCPIClientList.count() == 1)
        client->setAuthorisation(true);
}

cSCPIInterface *cSCPIServer::getScpiInterface()
{
    return &m_scpiInterface;
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

            m_pSerialClient = new cSCPISerialClient(m_pSerialPort, m_pModule, m_ConfigData, &m_scpiInterface);
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
        cSCPIEthClient* client = new cSCPIEthClient(socket, m_pModule, m_ConfigData, &m_scpiInterface); // each client our interface;
        appendClient(client);
        qInfo("Network SCPI client (%s) connected / Active clients: %i", qPrintable(client->getPeerAddress()), m_SCPIClientList.count());
        connect(socket, &QTcpSocket::disconnected, this, [=]() {
            deleteSCPIClient(client);
        });
     }
}

void cSCPIServer::deleteSCPIClient(cSCPIClient *client)
{
    // don't use for other than remove from list - it is destroyed and not usable
    if(m_SCPIClientList.contains(client)) {
        for(auto iter=m_pModule->scpiParameterCmdInfoHash.begin(); iter!=m_pModule->scpiParameterCmdInfoHash.end(); ) {
            if(iter.value()->getClient() == client)
                iter = m_pModule->scpiParameterCmdInfoHash.erase(iter);
            else
                iter++;
        }
        m_SCPIClientList.removeAll(client);
        delete client;
        qInfo("Network SCPI client deleted / Active clients: %i", m_SCPIClientList.count());
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
    QString errorMsg;
    bool ok = m_moduleInterface.setupInterface();
    ok = ok && m_interfaceInterface.setupInterface();
    ok = ok && m_statusInterface.setupInterface();
    ok = ok && m_ieee488Interface.setupInterface();
    if (!ok)
        errorMsg = interfacejsonErrMsg;

    ok = ok && m_pTcpServer->listen(QHostAddress(QHostAddress::AnyIPv4), m_ConfigData.m_InterfaceSocket.m_nPort);
    if(!ok)
        errorMsg = interfaceETHErrMsg;

    if (ok && m_ConfigData.m_SerialDevice.m_nOn == 1) {
        connect(&m_SerialTestTimer, &QTimer::timeout, this, &cSCPIServer::testSerial);
        m_SerialTestTimer.start(serialPollTimerPeriod);
    }
    if (ok)
        emit activationContinue();
    else
        notifyError(errorMsg);
}

void cSCPIServer::activationDone()
{
    m_scpiInterface.checkAmbiguousShortNames();

    QMap<QString, QString> modelListBaseEntry({{"RELEASE", ZenuxDeviceInfo::getZenuxRelease()}});
    QString xml;
    m_scpiInterface.exportSCPIModelXML(xml, modelListBaseEntry);
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
        deleteSCPIClient(client);
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
    bool queueActiveNew = scpiQueue.toInt();
    bool queueActiveOld = m_ConfigData.m_enableScpiQueue.m_nActive;
    m_ConfigData.m_enableScpiQueue.m_nActive = queueActiveNew;
    m_scpiInterface.setEnableQueue(queueActiveNew);
    if(queueActiveNew != queueActiveOld)
        emit m_pModule->parameterChanged();
}
}
