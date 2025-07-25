#include "statusmoduleinit.h"
#include "statusmodule.h"
#include <reply.h>
#include <proxy.h>
#include <errormessages.h>
#include <regexvalidator.h>
#include <zenuxdevicedetection.h>
#include <scpi.h>
#include <sysinfo.h>
#include <timerfactoryqt.h>
#include <QFile>
#include <intvalidator.h>
#include <QJsonDocument>

namespace STATUSMODULE
{

cStatusModuleInit::cStatusModuleInit(cStatusModule* module, cStatusModuleConfigData& configData) :
    cModuleActivist(module->getVeinModuleName()),
    m_pModule(module),
    m_ConfigData(configData)
{
    m_pPCBInterface = std::make_shared<Zera::cPCBInterface>();
    m_dspInterface = std::make_shared<Zera::cDSPInterface>();

    // m_pcbserverConnectionState.addTransition is done in pcbserverConnection
    m_pcbserverReadVersionState.addTransition(this, &cStatusModuleInit::activationContinue, &m_pcbReadVersionState);
    m_pcbReadVersionState.addTransition(this, &cStatusModuleInit::activationContinue, &m_pcbRegisterReadVersionNotifierState);
    m_pcbRegisterReadVersionNotifierState.addTransition(this, &cStatusModuleInit::activationContinue, &m_pcbserverReadCtrlVersionState);
    m_pcbserverReadCtrlVersionState.addTransition(this, &cStatusModuleInit::activationContinue, &m_pcbserverReadFPGAVersionState);
    m_pcbserverReadFPGAVersionState.addTransition(this, &cStatusModuleInit::activationContinue, &m_pcbserverReadSerialNrState);
    m_pcbserverReadSerialNrState.addTransition(this, &cStatusModuleInit::activationContinue, &m_pcbserverReadAdjStatusState);
    m_pcbserverReadAdjStatusState.addTransition(this, &cStatusModuleInit::activationContinue, &m_pcbserverReadAdjChksumState);
    m_pcbserverReadAdjChksumState.addTransition(this, &cStatusModuleInit::activationContinue, &m_pcbserverRegisterClampCatalogNotifierState);
    m_pcbserverRegisterClampCatalogNotifierState.addTransition(this, &cStatusModuleInit::activationContinue, &m_dspserverConnectionState);
    // m_dspserverConnectionState.addTransition is done in dspserverConnection
    m_dspserverReadVersionState.addTransition(this, &cStatusModuleInit::activationContinue, &m_dspserverReadDSPProgramState);
    m_dspserverReadDSPProgramState.addTransition(this, &cStatusModuleInit::activationContinue, &m_pcbserverRegisterSchnubbelStatusNotifierState);
    m_pcbserverRegisterSchnubbelStatusNotifierState.addTransition(this, &cStatusModuleInit::activationContinue, &m_pcbserverReadInitialSchnubbelStatus);
    m_pcbserverReadInitialSchnubbelStatus.addTransition(this, &cStatusModuleInit::activationContinue, &m_pcbserverRegisterAccumulatorStatusNotifierState);
    m_pcbserverRegisterAccumulatorStatusNotifierState.addTransition(this, &cStatusModuleInit::activationContinue, &m_pcbserverReadInitialAccumulatorStatus);
    m_pcbserverReadInitialAccumulatorStatus.addTransition(this, &cStatusModuleInit::activationContinue, &m_pcbserverRegisterAccumulatorSocNotifierState);
    m_pcbserverRegisterAccumulatorSocNotifierState.addTransition(this, &cStatusModuleInit::activationContinue, &m_pcbserverReadInitialAccumulatorSoc);
    m_pcbserverReadInitialAccumulatorSoc.addTransition(this, &cStatusModuleInit::activationContinue, &m_pcbserverRegisterCtrlVersionChange);
    m_pcbserverRegisterCtrlVersionChange.addTransition(this, &cStatusModuleInit::activationContinue, &m_activationDoneState);

    m_activationMachine.addState(&m_pcbserverConnectionState);
    m_activationMachine.addState(&m_pcbserverReadVersionState);
    m_activationMachine.addState(&m_pcbReadVersionState);
    m_activationMachine.addState(&m_pcbRegisterReadVersionNotifierState);
    m_activationMachine.addState(&m_pcbserverReadCtrlVersionState);
    m_activationMachine.addState(&m_pcbserverReadFPGAVersionState);
    m_activationMachine.addState(&m_pcbserverReadSerialNrState);
    m_activationMachine.addState(&m_pcbserverReadAdjStatusState);
    m_activationMachine.addState(&m_pcbserverReadAdjChksumState);
    m_activationMachine.addState(&m_pcbserverRegisterClampCatalogNotifierState);
    m_activationMachine.addState(&m_dspserverConnectionState);
    m_activationMachine.addState(&m_dspserverReadVersionState);
    m_activationMachine.addState(&m_dspserverReadDSPProgramState);
    m_activationMachine.addState(&m_pcbserverRegisterSchnubbelStatusNotifierState);
    m_activationMachine.addState(&m_pcbserverReadInitialSchnubbelStatus);
    m_activationMachine.addState(&m_pcbserverRegisterAccumulatorStatusNotifierState);
    m_activationMachine.addState(&m_pcbserverReadInitialAccumulatorStatus);
    m_activationMachine.addState(&m_pcbserverRegisterAccumulatorSocNotifierState);
    m_activationMachine.addState(&m_pcbserverReadInitialAccumulatorSoc);
    m_activationMachine.addState(&m_pcbserverRegisterCtrlVersionChange);
    m_activationMachine.addState(&m_activationDoneState);
    m_activationMachine.setInitialState(&m_pcbserverConnectionState);

    connect(&m_pcbserverConnectionState, &QState::entered, this, &cStatusModuleInit::pcbserverConnect);
    connect(&m_pcbserverReadVersionState, &QState::entered, this, &cStatusModuleInit::pcbserverReadVersion);
    connect(&m_pcbReadVersionState, &QState::entered, this, &cStatusModuleInit::pcbReadVersion);
    connect(&m_pcbRegisterReadVersionNotifierState, &QState::entered, this, &cStatusModuleInit::registerPCBVersionNotifier);
    connect(&m_pcbserverReadCtrlVersionState, &QState::entered, this, &cStatusModuleInit::pcbserverReadCtrlVersion);
    connect(&m_pcbserverReadFPGAVersionState, &QState::entered, this, &cStatusModuleInit::pcbserverReadFPGAVersion);
    connect(&m_pcbserverReadSerialNrState, &QState::entered, this, &cStatusModuleInit::pcbserverReadSerialNr);
    connect(&m_pcbserverReadAdjStatusState, &QState::entered, this, &cStatusModuleInit::pcbserverReadAdjStatus);
    connect(&m_pcbserverReadAdjChksumState, &QState::entered, this, &cStatusModuleInit::pcbserverReadAdjChksum);
    connect(&m_pcbserverRegisterClampCatalogNotifierState, &QState::entered, this, &cStatusModuleInit::registerClampCatalogNotifier);
    connect(&m_dspserverConnectionState, &QState::entered, this, &cStatusModuleInit::dspserverConnect);
    connect(&m_dspserverReadVersionState, &QState::entered, this, &cStatusModuleInit::dspserverReadVersion);
    connect(&m_dspserverReadDSPProgramState, &QState::entered, this, &cStatusModuleInit::dspserverReadDSPProgramVersion);
    connect(&m_pcbserverRegisterSchnubbelStatusNotifierState, &QState::entered, this, &cStatusModuleInit::registerSchnubbelStatusNotifier);
    connect(&m_pcbserverReadInitialSchnubbelStatus, &QState::entered, this, &cStatusModuleInit::getSchnubbelStatus);
    connect(&m_pcbserverRegisterAccumulatorStatusNotifierState, &QState::entered, this, &cStatusModuleInit::registerAccumulatorStatusNotifier);
    connect(&m_pcbserverReadInitialAccumulatorStatus, &QState::entered, this, &cStatusModuleInit::getAccumulatorStatus);
    connect(&m_pcbserverRegisterAccumulatorSocNotifierState, &QState::entered, this, &cStatusModuleInit::registerAccumulatorSocNotifier);
    connect(&m_pcbserverReadInitialAccumulatorSoc, &QState::entered, this, &cStatusModuleInit::getAccuStateOfCharge);
    connect(&m_pcbserverRegisterCtrlVersionChange, &QState::entered, this, &cStatusModuleInit::registerCtrlVersionsChangedNotifier);
    connect(&m_activationDoneState, &QState::entered, this, &cStatusModuleInit::activationDone);

    m_deactivationMachine.addState(&m_pcbserverUnregisterNotifiersState);
    m_deactivationMachine.addState(&m_deactivationDoneState);
    m_deactivationMachine.setInitialState(&m_pcbserverUnregisterNotifiersState);

    connect(&m_pcbserverUnregisterNotifiersState, &QState::entered, this, &cStatusModuleInit::unregisterNotifiers);
    connect(&m_deactivationDoneState, &QState::entered, this, &cStatusModuleInit::deactivationDone);

    // Adjustment re-read on clamp add / remove
    m_pcbserverReReadAdjStatusState.addTransition(this, &cStatusModuleInit::activationContinue, &m_pcbserverReReadAdjChksumState);
    m_pcbserverReReadAdjChksumState.addTransition(this, &cStatusModuleInit::activationContinue, &m_pcbserverReReadDoneState);
    // Terminate re-read state machine on error (ehm - how do activists handle this? Did not find a trace)
    m_pcbserverReReadAdjStatusState.addTransition(this, &cStatusModuleInit::activationError, &m_pcbserverReReadDoneState);
    m_pcbserverReReadAdjChksumState.addTransition(this, &cStatusModuleInit::activationError, &m_pcbserverReReadDoneState);

    m_pcbserverUnregisterNotifiersState.addTransition(this, &cStatusModuleInit::deactivationContinue, &m_deactivationDoneState);

    m_stateMachineAdjustmentReRead.addState(&m_pcbserverReReadAdjStatusState);
    m_stateMachineAdjustmentReRead.addState(&m_pcbserverReReadAdjChksumState);
    m_stateMachineAdjustmentReRead.addState(&m_pcbserverReReadDoneState);
    m_stateMachineAdjustmentReRead.setInitialState(&m_pcbserverReReadAdjStatusState);

    connect(&m_pcbserverReReadAdjStatusState, &QState::entered, this, &cStatusModuleInit::pcbserverReadAdjStatus);
    connect(&m_pcbserverReReadAdjChksumState, &QState::entered, this, &cStatusModuleInit::pcbserverReadAdjChksum);
    connect(&m_pcbserverReReadDoneState, &QState::entered, this, &cStatusModuleInit::setInterfaceComponents);
}

void cStatusModuleInit::generateVeinInterface()
{
    QString key;

    m_pPCBServerVersion = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                                key = QString("INF_PCBServerVersion"),
                                                QString("PCB-server version"),
                                                QVariant(QString("")) );
    m_pPCBServerVersion->setScpiInfo("STATUS", "VERSION:PCBSERVER", SCPI::isQuery, key );
    m_pModule->m_veinModuleParameterMap[key] = m_pPCBServerVersion; // for modules use

    m_pPCBVersion = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                          key = QString("INF_PCBVersion"),
                                          QString("PCB version"),
                                          QVariant(QString("")) );
    m_pPCBVersion->setScpiInfo("STATUS", "VERSION:PCB", SCPI::isQuery, key );
    m_pModule->m_veinModuleParameterMap[key] = m_pPCBVersion;

    m_pCtrlVersion = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                              key = QString("INF_CTRLVersion"),
                                              QString("Controller version"),
                                              QVariant(QString("") ));
    m_pCtrlVersion->setScpiInfo("STATUS", "VERSION:CONTROLER", SCPI::isQuery, key);
    m_pModule->m_veinModuleParameterMap[key] = m_pCtrlVersion;

    m_pFPGAVersion = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                           key = QString("INF_FPGAVersion"),
                                           QString("FPGA version"),
                                           QVariant(QString("") ));
    m_pFPGAVersion->setScpiInfo("STATUS", "VERSION:FPGA", SCPI::isQuery, key);
    m_pModule->m_veinModuleParameterMap[key] = m_pFPGAVersion;

    m_pSerialNumber = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                            key = QString("PAR_SerialNr"),
                                            QString("Serial number"),
                                            QVariant(QString("") ), true); // we select deferred notification
    m_pSerialNumber->setScpiInfo("STATUS", "SERIAL", SCPI::isQuery|SCPI::isCmdwP, key);
    m_pSerialNumber->setValidator(new cRegExValidator("^\\d{9}$"));
    m_pModule->m_veinModuleParameterMap[key] = m_pSerialNumber;

    m_pDSPServerVersion = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                                key = QString("INF_DSPServerVersion"),
                                                QString("DSP-server version"),
                                                QVariant(QString("") ));
    m_pDSPServerVersion->setScpiInfo("STATUS", "VERSION:DSPSERVER", SCPI::isQuery, key);
    m_pModule->m_veinModuleParameterMap[key] = m_pDSPServerVersion;

    m_pDSPProgramVersion = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                                 key = QString("INF_DSPVersion"),
                                                 QString("DSP software version"),
                                                 QVariant(QString("") ));
    m_pDSPProgramVersion->setScpiInfo("STATUS", "VERSION:DSP", SCPI::isQuery, key);
    m_pModule->m_veinModuleParameterMap[key] = m_pDSPProgramVersion;

    m_pReleaseNumber = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                             key = QString("INF_ReleaseNr"),
                                             QString("Release number"),
                                             QVariant(QString("") ));
    m_pReleaseNumber->setScpiInfo("STATUS", "RELEASE", SCPI::isQuery, key);
    m_pModule->m_veinModuleParameterMap[key] = m_pReleaseNumber;

    m_pDeviceType = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                                key = QString("INF_DeviceType"),
                                                QString("Device type"),
                                                QVariant(QString()));
    m_pDeviceType->setScpiInfo("STATUS", "DEVTYPE", SCPI::isQuery, key);
    m_pModule->m_veinModuleParameterMap[key] = m_pDeviceType;

    m_pCPUInfo = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                       key = QString("INF_CpuInfo"),
                                       QString("CPU/SOM info"),
                                       QVariant(QString("")));
    m_pCPUInfo->setScpiInfo("STATUS", "CPUINFO", SCPI::isQuery, key);
    m_pModule->m_veinModuleParameterMap[key] = m_pCPUInfo;

    m_pAdjustmentStatus = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                                   key = QString("INF_Adjusted"),
                                                   QString("Adjustment information"),
                                                   QVariant(""));
    m_pAdjustmentStatus->setScpiInfo("STATUS", "ADJUSTMENT", SCPI::isQuery, key);
    m_pModule->m_veinModuleParameterMap[key] = m_pAdjustmentStatus;

    m_pAdjustmentChksum = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                                   key = QString("INF_AdjChksum"),
                                                   QString("Adjustment data checksum device"),
                                                   QVariant(""));
    m_pAdjustmentChksum->setScpiInfo("STATUS", "ADJCHKSUM", SCPI::isQuery, key);
    m_pModule->m_veinModuleParameterMap[key] = m_pAdjustmentChksum;

    m_pSchnubbelStatus = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                               key = QString("INF_Schnubbel"),
                                               QString("Adjustment values write permission"),
                                               QVariant(0));
    m_pSchnubbelStatus->setScpiInfo("STATUS", "AUTHORIZATION", SCPI::isQuery, key);
    m_pModule->m_veinModuleParameterMap[key] = m_pSchnubbelStatus;

    m_pAccumulatorStatus = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                               key = QString("INF_AccumulatorStatus"),
                                               QString("Accumulator status"),
                                               QVariant(0));
    m_pModule->m_veinModuleParameterMap[key] = m_pAccumulatorStatus;
    m_pAccumulatorSoc = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                               key = QString("INF_AccumulatorSoc"),
                                               QString("Accumulator state of charge"),
                                               QVariant(int(0)));
    m_pModule->m_veinModuleParameterMap[key] = m_pAccumulatorSoc;
    if (m_ConfigData.m_accumulator) {
        m_pAccumulatorSoc->setScpiInfo("STATUS", "ACCUSOC", SCPI::isQuery, key);
        m_pAccumulatorStatus->setScpiInfo("STATUS", "ACCUSTATUS", SCPI::isQuery, key);
    }
}


void cStatusModuleInit::catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer)
{
    if (msgnr == 0) // 0 was reserved for async. messages
    {
        int notifierID = answer.toString().split(':').last().toInt();
        switch (notifierID) {
            case STATUSMODINIT::clampNotifierID:
                // clamp catalog changed: start adjustment state re-read
                if(m_bActive && !m_stateMachineAdjustmentReRead.isRunning()) {
                    m_stateMachineAdjustmentReRead.start();
                }
                break;
            case STATUSMODINIT::schnubbelNotifierID:
                getSchnubbelStatus();
                break;
            case STATUSMODINIT::accumulatorStatusNotifierID:
                getAccumulatorStatus();
                break;
            case STATUSMODINIT::accumulatorSocNotifierID:
                getAccuStateOfCharge();
                break;
            case STATUSMODINIT::ctrlVersionChangeID:
                pcbserverReadCtrlVersion();
                break;
            case STATUSMODINIT::pcbVersionChangeID:
                pcbReadVersion();
                break;
            }
    }
    else
    {
        if (m_MsgNrCmdList.contains(msgnr))
        {
            int cmd = m_MsgNrCmdList.take(msgnr);
            switch (cmd)
            {
            case STATUSMODINIT::registerClampCatalogNotifier:
                // we continue in any case - e.g COM5003 does not support clamps
                // just spit out a warning to journal
                if (reply != ack) {
                    qWarning("Register notification for clamps catalog failed - are clamps supported?");
                }
                emit activationContinue();
                break;
            case STATUSMODINIT::registerSchnubbelStatusNotifier:
                if (reply != ack) {
                    qWarning("Register notification for Schnubbel status failed");
                }
                emit activationContinue();
                break;
            case STATUSMODINIT::registerAccumulatorStatusNotifier:
                if (reply != ack) {
                    qWarning("Register notification for accumulator status failed - is accumulator supported?");
                }
                emit activationContinue();
                break;
            case STATUSMODINIT::registerAccumulatorSocNotifier:
                if (reply != ack) {
                    qWarning("Register notification for accumulator soc failed - is accumulator supported?");
                }
                emit activationContinue();
                break;
            case STATUSMODINIT::registerCtrlVersionChange:
                if (reply != ack) {
                    qWarning("Register notification for hotpluggable ctrl versions failed!");
                }
                emit activationContinue();
                break;
            case STATUSMODINIT::registerPCBVersionChange:
                if (reply != ack) {
                    qWarning("Register notification for hotpluggable PCB versions failed!");
                }
                emit activationContinue();
                break;
            case STATUSMODINIT::unregisterNotifiers:
                if (reply != ack) {
                    qWarning("Unregister notification failed");
                }
                emit deactivationContinue();
                break;

            case STATUSMODINIT::readPCBServerVersion:
                if (reply == ack) {
                    m_sPCBServerVersion = answer.toString();
                    emit activationContinue();
                }
                else
                    notifyActivationError(readPCBServerVersionErrMsg);
                break;

            case STATUSMODINIT::readPCBInfo:
                if (reply == ack) {
                    m_sPCBVersion = answer.toString();
                    m_pPCBVersion->setValue(m_sPCBVersion);
                    emit activationContinue();
                }
                else
                    notifyActivationError(readPCBInfoErrMsg);
                break;

            case STATUSMODINIT::readPCBServerCtrlVersion:
                if (reply == ack) {
                    m_sCtrlVersion = answer.toString();
                    m_pCtrlVersion->setValue(m_sCtrlVersion);
                    emit activationContinue();
                }
                else
                    notifyActivationError(readPCBCtrlVersionErrMSG);
                break;

            case STATUSMODINIT::readPCBServerFPGAVersion:
                if (reply == ack) {
                    m_sFPGAVersion = answer.toString();
                    emit activationContinue();
                }
                else
                    notifyActivationError(readPCBFPGAVersionErrMSG);
                break;


            case STATUSMODINIT::readPCBServerSerialNumber:
                if (reply == ack) {
                    m_sSerialNumber = answer.toString();
                    emit activationContinue();
                }
                else
                    notifyActivationError(readPCBSerialNrErrMSG);
                break;

            case STATUSMODINIT::writePCBServerSerialNumber:
                if (reply == ack) {
                    m_sSerialNumber = wantedSerialNr.toString();
                    m_pSerialNumber->setValue(wantedSerialNr); // m_pSerialNumber has deferred notification so this will send the event
                    emit m_pModule->parameterChanged();
                }
                else {
                    m_pSerialNumber->setError(); // in case of error we send an error event
                    notifyError(writePCBSerialNrErrMSG);
                }
                break;

            case STATUSMODINIT::readPCBServerAdjStatus:
                if (reply == ack) {
                    m_sAdjStatus = answer.toString();
                    emit activationContinue();
                }
                else
                    notifyActivationError(readadjstatusErrMsg);
                break;

            case STATUSMODINIT::readPCBServerAdjChksum:
                if (reply == ack) {
                    m_sAdjChksum = answer.toString();
                    emit activationContinue();
                }
                else
                    notifyActivationError(readadjchksumErrMsg);
                break;

            case STATUSMODINIT::readDSPServerVersion:
                if (reply == ack) {
                    m_sDSPServerVersion = answer.toString();
                    emit activationContinue();
                }
                else
                    notifyActivationError(readDSPServerVersionErrMsg);
                break;

            case STATUSMODINIT::readDSPServerDSPProgramVersion:
                if (reply == ack) {
                    m_sDSPProgramVersion = answer.toString();
                    emit activationContinue();
                }
                else
                    notifyActivationError(readDSPProgramVersionErrMsg);
                break;

            case STATUSMODINIT::readPCBServerSchnubbelStatus:
                if (reply == ack) {
                    m_pSchnubbelStatus->setValue(QVariant(answer.toInt()));
                    emit activationContinue();
                }
                else
                    notifyActivationError(readschnubbelstatusErrMsg);
                break;

            case STATUSMODINIT::readPCBServerAccumulatorStatus:
                if (!m_ConfigData.m_accumulator) {
                   m_pAccumulatorStatus->setValue(QVariant(0));
                   emit activationContinue();
                }
                else {
                    if (reply == ack) {
                        m_pAccumulatorStatus->setValue(QVariant(answer.toInt()));
                        emit activationContinue();
                    }
                    else
                        notifyActivationError(readaccumulatorstatusErrMsg);
                }
                break;

            case STATUSMODINIT::readPCBServerAccumulatorSoc:
                if (!m_ConfigData.m_accumulator) {
                   emit activationContinue();
                }
                else {
                    if (reply == ack) {
                       m_pAccumulatorSoc->setValue(QVariant(answer.toInt()));
                       emit activationContinue();
                    }
                    else
                       notifyActivationError(readaccumulatorsocErrMsg);
                }
                break;
            }
        }
    }
}

void cStatusModuleInit::notifyActivationError(QVariant value)
{
    notifyError(value);
    emit activationError();
}

QString cStatusModuleInit::findReleaseNr()
{
    QString releaseNr;

    releaseNr = SysInfo::getReleaseNr();
    if (releaseNr == "")
        notifyError(releaseNumberErrMsg);

    return releaseNr;
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

void cStatusModuleInit::getSchnubbelStatus()
{
    m_MsgNrCmdList[m_pPCBInterface->getAuthorizationStatus()] = STATUSMODINIT::readPCBServerSchnubbelStatus;
}

void cStatusModuleInit::getAccumulatorStatus()
{
    m_MsgNrCmdList[m_pPCBInterface->getAccumulatorStatus()] = STATUSMODINIT::readPCBServerAccumulatorStatus;
}

void cStatusModuleInit::getAccuStateOfCharge()
{
    m_MsgNrCmdList[m_pPCBInterface->getAccuStateOfCharge()] = STATUSMODINIT::readPCBServerAccumulatorSoc;
}

void cStatusModuleInit::setInterfaceComponents()
{
    m_pPCBServerVersion->setValue(QVariant(m_sPCBServerVersion));
    m_pPCBVersion->setValue(QVariant(m_sPCBVersion));
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


void cStatusModuleInit::pcbserverConnect()
{
    m_pPCBClient = Zera::Proxy::getInstance()->getConnectionSmart(m_pModule->getNetworkConfig()->m_pcbServiceConnectionInfo,
                                                                  m_pModule->getNetworkConfig()->m_tcpNetworkFactory);
    m_pcbserverConnectionState.addTransition(m_pPCBClient.get(), &Zera::ProxyClient::connected, &m_pcbserverReadVersionState);

    m_pPCBInterface->setClientSmart(m_pPCBClient);
    connect(m_pPCBInterface.get(), &AbstractServerInterface::serverAnswer, this, &cStatusModuleInit::catchInterfaceAnswer);
    Zera::Proxy::getInstance()->startConnectionSmart(m_pPCBClient);
}


void cStatusModuleInit::pcbserverReadVersion()
{
    m_MsgNrCmdList[m_pPCBInterface->readServerVersion()] = STATUSMODINIT::readPCBServerVersion;
}

void cStatusModuleInit::pcbReadVersion()
{
    m_MsgNrCmdList[m_pPCBInterface->readPCBInfo()] = STATUSMODINIT::readPCBInfo;
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
    m_MsgNrCmdList[m_pPCBInterface->registerNotifier(QString("SYSTEM:CLAMP:CHANNEL:CATALOG?"), STATUSMODINIT::clampNotifierID)] = STATUSMODINIT::registerClampCatalogNotifier;
}

void cStatusModuleInit::unregisterNotifiers()
{
    // we are handler of first state:
    m_bActive = false;
    m_MsgNrCmdList[m_pPCBInterface->unregisterNotifiers()] = STATUSMODINIT::unregisterNotifiers;
}

void cStatusModuleInit::dspserverConnect()
{
    m_dspClient = Zera::Proxy::getInstance()->getConnectionSmart(
        m_pModule->getNetworkConfig()->m_dspServiceConnectionInfo,
        m_pModule->getNetworkConfig()->m_tcpNetworkFactory);
    m_dspInterface->setClientSmart(m_dspClient);
    m_dspserverConnectionState.addTransition(m_dspClient.get(), &Zera::ProxyClient::connected, &m_dspserverReadVersionState);
    connect(m_dspInterface.get(), &AbstractServerInterface::serverAnswer, this, &cStatusModuleInit::catchInterfaceAnswer);
    Zera::Proxy::getInstance()->startConnectionSmart(m_dspClient);
}


void cStatusModuleInit::dspserverReadVersion()
{
    m_MsgNrCmdList[m_dspInterface->readServerVersion()] = STATUSMODINIT::readDSPServerVersion;
}


void cStatusModuleInit::dspserverReadDSPProgramVersion()
{
    m_MsgNrCmdList[m_dspInterface->readDeviceVersion()] = STATUSMODINIT::readDSPServerDSPProgramVersion;
}

void cStatusModuleInit::registerSchnubbelStatusNotifier()
{
    m_MsgNrCmdList[m_pPCBInterface->registerNotifier(QString("STATUS:AUTHORIZATION?"), STATUSMODINIT::schnubbelNotifierID)] = STATUSMODINIT::registerSchnubbelStatusNotifier;
}

void cStatusModuleInit::registerAccumulatorStatusNotifier()
{
    m_MsgNrCmdList[m_pPCBInterface->registerNotifier(QString("SYSTEM:ACCUMULATOR:STATUS?"), STATUSMODINIT::accumulatorStatusNotifierID)] = STATUSMODINIT::registerAccumulatorStatusNotifier;
}

void cStatusModuleInit::registerAccumulatorSocNotifier()
{
    m_MsgNrCmdList[m_pPCBInterface->registerNotifier(QString("SYSTEM:ACCUMULATOR:SOC?"), STATUSMODINIT::accumulatorSocNotifierID)] = STATUSMODINIT::registerAccumulatorSocNotifier;
}

void cStatusModuleInit::registerCtrlVersionsChangedNotifier()
{
    m_MsgNrCmdList[m_pPCBInterface->registerNotifier(QString("SYSTEM:VERSION:CTRL?"), STATUSMODINIT::ctrlVersionChangeID)] = STATUSMODINIT::registerCtrlVersionChange;
}

void cStatusModuleInit::registerPCBVersionNotifier()
{
    m_MsgNrCmdList[m_pPCBInterface->registerNotifier(QString("SYSTEM:VERSION:PCB?"), STATUSMODINIT::pcbVersionChangeID)] = STATUSMODINIT::registerPCBVersionChange;
}

void cStatusModuleInit::activationDone()
{
    m_sReleaseNumber = findReleaseNr();
    m_sDeviceType = ZenuxDeviceDetection::getDeviceNameFromKernelParam();
    if (m_sDeviceType.isEmpty())
        m_sDeviceType = QStringLiteral("unknown");
    m_sCPUInfo = findCpuInfo();
    setInterfaceComponents();
    connect(m_pSerialNumber, &VfModuleParameter::sigValueChanged, this, &cStatusModuleInit::newSerialNumber);
    m_bActive = true;
    emit activated();
}


void cStatusModuleInit::deactivationDone()
{
    Zera::Proxy::getInstance()->releaseConnectionSmart(m_dspClient);
    Zera::Proxy::getInstance()->releaseConnectionSmart(m_pPCBClient);
    disconnect(m_dspClient.get(), 0, this, 0);
    disconnect(m_pPCBInterface.get(), 0, this, 0);
    emit deactivated();
}


void cStatusModuleInit::newSerialNumber(QVariant serialNr)
{
    wantedSerialNr = serialNr;
    m_MsgNrCmdList[m_pPCBInterface->writeSerialNr(serialNr.toString())] = STATUSMODINIT::writePCBServerSerialNumber;
}
}
