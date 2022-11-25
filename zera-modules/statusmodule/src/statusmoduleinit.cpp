#include "statusmoduleinit.h"
#include "statusmodule.h"
#include <reply.h>
#include <errormessages.h>
#include <modulevalidator.h>
#include <regexvalidator.h>
#include <QFile>

namespace STATUSMODULE
{

cStatusModuleInit::cStatusModuleInit(cStatusModule* module, Zera::Proxy::cProxy* proxy, cStatusModuleConfigData& configData)
    :m_pModule(module), m_pProxy(proxy), m_ConfigData(configData)
{
    m_pPCBInterface = new Zera::Server::cPCBInterface();
    m_pDSPInterface = new Zera::Server::cDSPInterface();

    m_IdentifyState.addTransition(this, &cStatusModuleInit::activationContinue, &m_pcbserverConnectionState);
    // m_pcbserverConnectionState.addTransition is done in pcbserverConnection
    m_pcbserverReadVersionState.addTransition(this, &cStatusModuleInit::activationContinue, &m_pcbserverReadCtrlVersionState);
    m_pcbserverReadCtrlVersionState.addTransition(this, &cStatusModuleInit::activationContinue, &m_pcbserverReadFPGAVersionState);
    m_pcbserverReadFPGAVersionState.addTransition(this, &cStatusModuleInit::activationContinue, &m_pcbserverReadSerialNrState);
    m_pcbserverReadSerialNrState.addTransition(this, &cStatusModuleInit::activationContinue, &m_pcbserverReadAdjStatusState);
    m_pcbserverReadAdjStatusState.addTransition(this, &cStatusModuleInit::activationContinue, &m_pcbserverReadAdjChksumState);
    m_pcbserverReadAdjChksumState.addTransition(this, &cStatusModuleInit::activationContinue, &m_pcbserverRegisterClampCatalogNotifierState);
    m_pcbserverRegisterClampCatalogNotifierState.addTransition(this, &cStatusModuleInit::activationContinue, &m_dspserverConnectionState);
    // m_dspserverConnectionState.addTransition is done in dspserverConnection
    m_dspserverReadVersionState.addTransition(this, &cStatusModuleInit::activationContinue, &m_dspserverReadDSPProgramState);
    m_dspserverReadDSPProgramState.addTransition(this, &cStatusModuleInit::activationContinue, &m_activationDoneState);

    m_activationMachine.addState(&m_resourceManagerConnectState);
    m_activationMachine.addState(&m_IdentifyState);
    m_activationMachine.addState(&m_pcbserverConnectionState);
    m_activationMachine.addState(&m_pcbserverReadVersionState);
    m_activationMachine.addState(&m_pcbserverReadCtrlVersionState);
    m_activationMachine.addState(&m_pcbserverReadFPGAVersionState);
    m_activationMachine.addState(&m_pcbserverReadSerialNrState);
    m_activationMachine.addState(&m_pcbserverReadAdjStatusState);
    m_activationMachine.addState(&m_pcbserverReadAdjChksumState);
    m_activationMachine.addState(&m_pcbserverRegisterClampCatalogNotifierState);
    m_activationMachine.addState(&m_dspserverConnectionState);
    m_activationMachine.addState(&m_dspserverReadVersionState);
    m_activationMachine.addState(&m_dspserverReadDSPProgramState);
    m_activationMachine.addState(&m_activationDoneState);
    if(!m_ConfigData.m_bDemo) {
        m_activationMachine.setInitialState(&m_resourceManagerConnectState);
    } else {
        m_activationMachine.setInitialState(&m_activationDoneState);
    }

    connect(&m_resourceManagerConnectState, &QState::entered, this, &cStatusModuleInit::resourceManagerConnect);
    connect(&m_IdentifyState, &QState::entered, this, &cStatusModuleInit::sendRMIdent);
    connect(&m_pcbserverConnectionState, &QState::entered, this, &cStatusModuleInit::pcbserverConnect);
    connect(&m_pcbserverReadVersionState, &QState::entered, this, &cStatusModuleInit::pcbserverReadVersion);
    connect(&m_pcbserverReadCtrlVersionState, &QState::entered, this, &cStatusModuleInit::pcbserverReadCtrlVersion);
    connect(&m_pcbserverReadFPGAVersionState, &QState::entered, this, &cStatusModuleInit::pcbserverReadFPGAVersion);
    connect(&m_pcbserverReadSerialNrState, &QState::entered, this, &cStatusModuleInit::pcbserverReadSerialNr);
    connect(&m_pcbserverReadAdjStatusState, &QState::entered, this, &cStatusModuleInit::pcbserverReadAdjStatus);
    connect(&m_pcbserverReadAdjChksumState, &QState::entered, this, &cStatusModuleInit::pcbserverReadAdjChksum);
    connect(&m_pcbserverRegisterClampCatalogNotifierState, &QState::entered, this, &cStatusModuleInit::registerClampCatalogNotifier);
    connect(&m_dspserverConnectionState, &QState::entered, this, &cStatusModuleInit::dspserverConnect);
    connect(&m_dspserverReadVersionState, &QState::entered, this, &cStatusModuleInit::dspserverReadVersion);
    connect(&m_dspserverReadDSPProgramState, &QState::entered, this, &cStatusModuleInit::dspserverReadDSPProgramVersion);
    connect(&m_activationDoneState, &QState::entered, this, &cStatusModuleInit::activationDone);

    m_deactivationMachine.addState(&m_pcbserverUnregisterClampCatalogNotifierState);
    m_deactivationMachine.addState(&m_deactivationDoneState);
    if(!m_ConfigData.m_bDemo) {
        m_deactivationMachine.setInitialState(&m_pcbserverUnregisterClampCatalogNotifierState);
    } else {
        m_deactivationMachine.setInitialState(&m_deactivationDoneState);
    }

    connect(&m_pcbserverUnregisterClampCatalogNotifierState, &QState::entered, this, &cStatusModuleInit::unregisterClampCatalogNotifier);
    connect(&m_deactivationDoneState, &QState::entered, this, &cStatusModuleInit::deactivationDone);

    // Adjustment re-read on clamp add / remove
    m_pcbserverReReadAdjStatusState.addTransition(this, &cStatusModuleInit::activationContinue, &m_pcbserverReReadAdjChksumState);
    m_pcbserverReReadAdjChksumState.addTransition(this, &cStatusModuleInit::activationContinue, &m_pcbserverReReadDoneState);
    // Terminate re-read state machine on error (ehm - how do activists handle this? Did not find a trace)
    m_pcbserverReReadAdjStatusState.addTransition(this, &cStatusModuleInit::activationError, &m_pcbserverReReadDoneState);
    m_pcbserverReReadAdjChksumState.addTransition(this, &cStatusModuleInit::activationError, &m_pcbserverReReadDoneState);

    m_pcbserverUnregisterClampCatalogNotifierState.addTransition(this, &cStatusModuleInit::deactivationContinue, &m_deactivationDoneState);

    m_stateMachineAdjustmentReRead.addState(&m_pcbserverReReadAdjStatusState);
    m_stateMachineAdjustmentReRead.addState(&m_pcbserverReReadAdjChksumState);
    m_stateMachineAdjustmentReRead.addState(&m_pcbserverReReadDoneState);
    m_stateMachineAdjustmentReRead.setInitialState(&m_pcbserverReReadAdjStatusState);

    connect(&m_pcbserverReReadAdjStatusState, &QState::entered, this, &cStatusModuleInit::pcbserverReadAdjStatus);
    connect(&m_pcbserverReReadAdjChksumState, &QState::entered, this, &cStatusModuleInit::pcbserverReadAdjChksum);
    connect(&m_pcbserverReReadDoneState, &QState::entered, this, &cStatusModuleInit::setInterfaceComponents);

}


cStatusModuleInit::~cStatusModuleInit()
{
    delete m_pPCBInterface;
    delete m_pDSPInterface;
}


void cStatusModuleInit::generateInterface()
{
    QString key;
    m_pPCBServerVersion = new cVeinModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                                   key = QString("INF_PCBServerVersion"),
                                                   QString("PCB-server version"),
                                                   QVariant(QString("")) );

    m_pModule->veinModuleParameterHash[key] = m_pPCBServerVersion; // for modules use
    m_pPCBServerVersion->setSCPIInfo(new cSCPIInfo("STATUS", "VERSION:PCBSERVER", "2", key , "0", ""));


    m_pCtrlVersion = new cVeinModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                              key = QString("INF_CTRLVersion"),
                                              QString("Controler version"),
                                              QVariant(QString("") ));

    m_pModule->veinModuleParameterHash[key] = m_pCtrlVersion;
    m_pCtrlVersion->setSCPIInfo(new cSCPIInfo("STATUS", "VERSION:CONTROLER", "2", key, "0", ""));


    m_pFPGAVersion = new cVeinModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                              key = QString("INF_FPGAVersion"),
                                              QString("FPGA version"),
                                              QVariant(QString("") ));

    m_pModule->veinModuleParameterHash[key] = m_pFPGAVersion;
    m_pFPGAVersion->setSCPIInfo(new cSCPIInfo("STATUS", "VERSION:FPGA", "2", key, "0", ""));


    m_pSerialNumber = new cVeinModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                               key = QString("PAR_SerialNr"),
                                               QString("Serial number"),
                                               QVariant(QString("") ), true, false); // we select deferred notification

    m_pModule->veinModuleParameterHash[key] = m_pSerialNumber;
    m_pSerialNumber->setSCPIInfo(new cSCPIInfo("STATUS", "SERIAL", "10", key, "0", ""));
    cRegExValidator *regValidator;
    regValidator = new cRegExValidator("^\\d{9}$");
    m_pSerialNumber->setValidator(regValidator);

    m_pDSPServerVersion = new cVeinModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                                   key = QString("INF_DSPServerVersion"),
                                                   QString("DCP-server version"),
                                                   QVariant(QString("") ));

    m_pModule->veinModuleParameterHash[key] = m_pDSPServerVersion;
    m_pDSPServerVersion->setSCPIInfo(new cSCPIInfo("STATUS", "VERSION:DSPSERVER", "2", key, "0", ""));


    m_pDSPProgramVersion = new cVeinModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                                    key = QString("INF_DSPVersion"),
                                                    QString("DSP software version"),
                                                    QVariant(QString("") ));

    m_pModule->veinModuleParameterHash[key] = m_pDSPProgramVersion;
    m_pDSPProgramVersion->setSCPIInfo(new cSCPIInfo("STATUS", "VERSION:DSP", "2", key, "0", ""));


    m_pReleaseNumber = new cVeinModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                                key = QString("INF_ReleaseNr"),
                                                QString("Release number"),
                                                QVariant(QString("") ));

    m_pModule->veinModuleParameterHash[key] = m_pReleaseNumber;
    m_pReleaseNumber->setSCPIInfo(new cSCPIInfo("STATUS", "RELEASE", "2", key, "0", ""));

    m_pDeviceType = new cVeinModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                                key = QString("INF_DeviceType"),
                                                QString("Device type"),
                                                QVariant(QString()));

    m_pModule->veinModuleParameterHash[key] = m_pDeviceType;
    m_pDeviceType->setSCPIInfo(new cSCPIInfo("STATUS", "DEVTYPE", "2", key, "0", ""));

    m_pCPUInfo = new cVeinModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                                key = QString("INF_CpuInfo"),
                                                QString("CPU/SOM info"),
                                                QVariant(QString("")));

    m_pModule->veinModuleParameterHash[key] = m_pCPUInfo;
    m_pCPUInfo->setSCPIInfo(new cSCPIInfo("STATUS", "CPUINFO", "2", key, "0", ""));

    m_pAdjustmentStatus = new cVeinModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                                   key = QString("INF_Adjusted"),
                                                   QString("Adjustment information"),
                                                   QVariant(0));

    m_pModule->veinModuleParameterHash[key] = m_pAdjustmentStatus;
    m_pAdjustmentStatus->setSCPIInfo(new cSCPIInfo("STATUS", "ADJUSTMENT", "2", key, "0", ""));

    m_pAdjustmentChksum = new cVeinModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                                   key = QString("INF_AdjChksum"),
                                                   QString("Device adjustment data checksum"),
                                                   QVariant(0));

    m_pModule->veinModuleParameterHash[key] = m_pAdjustmentChksum;
    m_pAdjustmentChksum->setSCPIInfo(new cSCPIInfo("STATUS", "ADJCHKSUM", "2", key, "0", ""));
}


void cStatusModuleInit::catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer)
{
    if (msgnr == 0) // 0 was reserved for async. messages
    {
        // clamp catalog changed: start adjustment state re-read
        if(m_bActive && !m_stateMachineAdjustmentReRead.isRunning()) {
            m_stateMachineAdjustmentReRead.start();
        }
    }
    else
    {
        if (m_MsgNrCmdList.contains(msgnr))
        {
            int cmd = m_MsgNrCmdList.take(msgnr);
            switch (cmd)
            {
            case STATUSMODINIT::registernotifier:
                // we continue in any case - e.g COM5003 does not support clamps
                // just spit out a warning to journal
                if (reply != ack) {
                    qWarning("Register notification for clamps catalog failed - are clamps supported?");
                }
                emit activationContinue();
                break;
            case STATUSMODINIT::unregisternotifiers:
                if (reply != ack) {
                    qWarning("Unregister notification for clamps catalog failed - are clamps supported?");
                }
                emit deactivationContinue();
                break;

            case STATUSMODINIT::sendrmident:
                if (reply == ack) // we only continue if resource manager acknowledges
                    emit activationContinue();
                else
                {
                    emit errMsg((tr(rmidentErrMSG)));
                    emit activationError();
                }
                break;

            case STATUSMODINIT::readPCBServerVersion:
                if (reply == ack)
                {
                    m_sPCBServerVersion = answer.toString();
                    emit activationContinue();
                }
                else
                {
                    emit errMsg((tr(readPCBServerVersionErrMsg)));
                    emit activationError();
                }
                break;

            case STATUSMODINIT::readPCBServerCtrlVersion:
                if (reply == ack)
                {
                    m_sCtrlVersion = answer.toString();
                    emit activationContinue();
                }
                else
                {
                    emit errMsg((tr(readPCBCtrlVersionErrMSG)));
                    emit activationError();
                }
                break;

            case STATUSMODINIT::readPCBServerFPGAVersion:
                if (reply == ack)
                {
                    m_sFPGAVersion = answer.toString();
                    emit activationContinue();
                }
                else
                {
                    emit errMsg((tr(readPCBFPGAVersionErrMSG)));
                    emit activationError();
                }
                break;


            case STATUSMODINIT::readPCBServerSerialNumber:
                if (reply == ack)
                {
                    m_sSerialNumber = answer.toString();
                    emit activationContinue();
                }
                else
                {
                    emit errMsg((tr(readPCBSerialNrErrMSG)));
                    emit activationError();
                }
                break;

            case STATUSMODINIT::writePCBServerSerialNumber:
                if (reply == ack)
                {
                    m_sSerialNumber = wantedSerialNr.toString();
                    m_pSerialNumber->setValue(wantedSerialNr); // m_pSerialNumber has deferred notification so this will send the event
                    emit m_pModule->parameterChanged();
                }
                else
                {
                    m_pSerialNumber->setError(); // in case of error we send an error event
                    emit errMsg(tr(writePCBSerialNrErrMSG));
                }
                break;

            case STATUSMODINIT::readPCBServerAdjStatus:
                if (reply == ack)
                {
                    m_sAdjStatus = answer.toString();
                    emit activationContinue();
                }
                else {
                    emit errMsg((tr(readadjstatusErrMsg)));
                    emit activationError();
                }
                break;

            case STATUSMODINIT::readPCBServerAdjChksum:
                if (reply == ack)
                {
                    m_sAdjChksum = answer.toString();
                    emit activationContinue();
                }
                else
                {
                    emit errMsg((tr(readadjchksumErrMsg)));
                    emit activationError();
                }
                break;

            case STATUSMODINIT::readDSPServerVersion:
                if (reply == ack)
                {
                    m_sDSPServerVersion = answer.toString();
                    emit activationContinue();
                }
                else
                {
                    emit errMsg((tr(readDSPServerVersionErrMsg)));
                    emit activationError();
                }
                break;

            case STATUSMODINIT::readDSPServerDSPProgramVersion:
                if (reply == ack)
                {
                    m_sDSPProgramVersion = answer.toString();
                    emit activationContinue();
                }
                else
                {
                    emit errMsg((tr(readDSPProgramVersionErrMsg)));
                    emit activationError();
                }
                break;
            }
        }
    }
}


QString cStatusModuleInit::findReleaseNr()
{
    QString releaseNr;

    releaseNr = m_pModule->getReleaseNr(ReleaseInfoFilePath);
    if (releaseNr == "")
        emit errMsg(releaseNumberErrMsg);

    return releaseNr;
}

QString cStatusModuleInit::findDeviceType()
{
    QString devType = QStringLiteral("unknown");
    QString procKernelParamFilename = QStringLiteral("/proc/cmdline");
    QFile file(procKernelParamFilename);
    if(file.open(QIODevice::ReadOnly)) {
        QStringList kernelParams = QString::fromLatin1(file.readAll()).split(QStringLiteral(" "));
        QString paramSearch = QStringLiteral("zera_device=");
        for(auto param : kernelParams) {
            if(param.contains(paramSearch)) {
                devType = param.replace(paramSearch, QString()).trimmed();
                break;
            }
        }
        file.close();
    }
    return devType;
}

QString cStatusModuleInit::findCpuInfo()
{
    QString cpuInfo;
    QString eepromDumpFile = QStringLiteral("/tmp/varsom-eeprom.dump");
    QFile file(eepromDumpFile);
    if(file.open(QIODevice::ReadOnly)) {
        QByteArray eepromDump = file.readAll();
        file.close();
        // valid dump?
        if(eepromDump.count() >= 64 && eepromDump.left(4) == QByteArray("WARI")) {
            // structure taken from u-boot-variscite / mx6var_eeprom_v1.h
            QString partNumber = eepromDump.mid(4, 16);
            QString assembly = eepromDump.mid(20, 16);
            QString date = eepromDump.mid(36, 16);
            //int version = eepromDump.at(52);
            cpuInfo = QString("{\"PartNumber\":\"%1\",\"Assembly\":\"%2\",\"Date\":\"%3\"}").arg(partNumber).arg(assembly).arg(date);
        }
    }
    return cpuInfo;
}

void cStatusModuleInit::setupDemoOperation()
{
    m_sSerialNumber = QStringLiteral("123456789");
    m_sAdjStatus = "0";
}

void cStatusModuleInit::setInterfaceComponents()
{
    m_pPCBServerVersion->setValue(QVariant(m_sPCBServerVersion));
    m_pCtrlVersion->setValue(QVariant(m_sCtrlVersion));
    m_pFPGAVersion->setValue(QVariant(m_sFPGAVersion));
    m_pSerialNumber->setValue(QVariant(m_sSerialNumber));
    m_pDSPServerVersion->setValue((QVariant(m_sDSPServerVersion)));
    m_pDSPProgramVersion->setValue(QVariant(m_sDSPProgramVersion));
    m_pReleaseNumber->setValue(QVariant(m_sReleaseNumber));
    m_pDeviceType->setValue(QVariant(m_sDeviceType));
    m_pCPUInfo->setValue(QVariant(m_sCPUInfo));
    m_pAdjustmentStatus->setValue(QVariant(m_sAdjStatus));
    m_pAdjustmentChksum->setValue(QVariant(m_sAdjChksum));
}


void cStatusModuleInit::resourceManagerConnect()
{
    // first we try to get a connection to resource manager over proxy
    m_rmClient = m_pProxy->getConnectionSmart(m_ConfigData.m_RMSocket.m_sIP, m_ConfigData.m_RMSocket.m_nPort);
    // and then we set connection resource manager interface's connection
    m_rmInterface.setClientSmart(m_rmClient); //
    m_resourceManagerConnectState.addTransition(m_rmClient.get(), &Zera::Proxy::cProxyClient::connected, &m_IdentifyState);
    connect(&m_rmInterface, &Zera::Server::cRMInterface::serverAnswer, this, &cStatusModuleInit::catchInterfaceAnswer);
    // todo insert timer for timeout and/or connect error conditions
    m_pProxy->startConnectionSmart(m_rmClient);
}


void cStatusModuleInit::sendRMIdent()
{
    m_MsgNrCmdList[m_rmInterface.rmIdent(QString("VersionModuleInit%1").arg(m_pModule->getModuleNr()))] = STATUSMODINIT::sendrmident;
}


void cStatusModuleInit::pcbserverConnect()
{
    m_pPCBClient = m_pProxy->getConnection(m_ConfigData.m_PCBServerSocket.m_sIP, m_ConfigData.m_PCBServerSocket.m_nPort);
    m_pcbserverConnectionState.addTransition(m_pPCBClient, &Zera::Proxy::cProxyClient::connected, &m_pcbserverReadVersionState);

    m_pPCBInterface->setClient(m_pPCBClient);
    connect(m_pPCBInterface, &Zera::Server::cPCBInterface::serverAnswer, this, &cStatusModuleInit::catchInterfaceAnswer);
    m_pProxy->startConnection(m_pPCBClient);
}


void cStatusModuleInit::pcbserverReadVersion()
{
    m_MsgNrCmdList[m_pPCBInterface->readServerVersion()] = STATUSMODINIT::readPCBServerVersion;
}


void cStatusModuleInit::pcbserverReadCtrlVersion()
{
    m_MsgNrCmdList[m_pPCBInterface->readCTRLVersion()] = STATUSMODINIT::readPCBServerCtrlVersion;
}


void cStatusModuleInit::pcbserverReadFPGAVersion()
{
    m_MsgNrCmdList[m_pPCBInterface->readFPGAVersion()] = STATUSMODINIT::readPCBServerFPGAVersion;
}


void cStatusModuleInit::pcbserverReadSerialNr()
{
    m_MsgNrCmdList[m_pPCBInterface->readSerialNr()] = STATUSMODINIT::readPCBServerSerialNumber;
}


void cStatusModuleInit::pcbserverReadAdjStatus()
{
    m_MsgNrCmdList[m_pPCBInterface->getAdjustmentStatus()] = STATUSMODINIT::readPCBServerAdjStatus;
}


void cStatusModuleInit::pcbserverReadAdjChksum()
{
    m_MsgNrCmdList[m_pPCBInterface->getAdjustmentChksum()] = STATUSMODINIT::readPCBServerAdjChksum;
}

void cStatusModuleInit::registerClampCatalogNotifier()
{
    m_MsgNrCmdList[m_pPCBInterface->registerNotifier(QString("SYSTEM:CLAMP:CHANNEL:CATALOG?"), QString("1"))] = STATUSMODINIT::registernotifier;
}

void cStatusModuleInit::unregisterClampCatalogNotifier()
{
    // we are handler of first state:
    m_bActive = false;
    m_MsgNrCmdList[m_pPCBInterface->unregisterNotifiers()] = STATUSMODINIT::unregisternotifiers;
}

void cStatusModuleInit::dspserverConnect()
{
    // we set up our dsp server connection
    m_pDSPClient = m_pProxy->getConnection(m_ConfigData.m_DSPServerSocket.m_sIP, m_ConfigData.m_DSPServerSocket.m_nPort);
    m_pDSPInterface->setClient(m_pDSPClient);
    m_dspserverConnectionState.addTransition(m_pDSPClient, &Zera::Proxy::cProxyClient::connected, &m_dspserverReadVersionState);
    connect(m_pDSPInterface, &Zera::Server::cDSPInterface::serverAnswer, this, &cStatusModuleInit::catchInterfaceAnswer);
    m_pProxy->startConnection(m_pDSPClient);
}


void cStatusModuleInit::dspserverReadVersion()
{
    m_MsgNrCmdList[m_pDSPInterface->readServerVersion()] = STATUSMODINIT::readDSPServerVersion;
}


void cStatusModuleInit::dspserverReadDSPProgramVersion()
{
    m_MsgNrCmdList[m_pDSPInterface->readDeviceVersion()] = STATUSMODINIT::readDSPServerDSPProgramVersion;
}


void cStatusModuleInit::activationDone()
{
    if(!m_ConfigData.m_bDemo) {
        m_sReleaseNumber = findReleaseNr();
        m_sDeviceType = findDeviceType();
        m_sCPUInfo = findCpuInfo();
    }
    else {
        setupDemoOperation();
    }
    setInterfaceComponents();
    if(!m_ConfigData.m_bDemo) {
        connect(m_pSerialNumber, &cVeinModuleParameter::sigValueChanged, this, &cStatusModuleInit::newSerialNumber);
    }
    m_bActive = true;
    emit activated();
}


void cStatusModuleInit::deactivationDone()
{
    m_pProxy->releaseConnection(m_pDSPClient);
    m_pProxy->releaseConnection(m_pPCBClient);
    // and disconnect from our servers afterwards
    disconnect(&m_rmInterface, 0, this, 0);
    disconnect(m_pDSPInterface, 0, this, 0);
    disconnect(m_pPCBInterface, 0, this, 0);
    emit deactivated();
}


void cStatusModuleInit::newSerialNumber(QVariant serialNr)
{
    wantedSerialNr = serialNr;
    m_MsgNrCmdList[m_pPCBInterface->writeSerialNr(serialNr.toString())] = STATUSMODINIT::writePCBServerSerialNumber;
}

}
