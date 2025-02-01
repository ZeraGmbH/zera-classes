#include "adjustmentmodulemeasprogram.h"
#include "adjustmentmodule.h"
#include "adjustvalidator.h"
#include "adjustmentmoduleconfiguration.h"
#include "taskoffset.h"
#include <reply.h>
#include <proxy.h>
#include <intvalidator.h>
#include <useratan.h>
#include <errormessages.h>
#include <math.h>

cAdjustmentModuleMeasProgram::cAdjustmentModuleMeasProgram(cAdjustmentModule* module, std::shared_ptr<BaseModuleConfiguration> pConfiguration) :
    cBaseMeasWorkProgram(pConfiguration, module->getVeinModuleName()),
    m_pModule(module),
    m_commonObjects(std::make_shared<AdjustmentModuleCommon>(m_pModule->getNetworkConfig())),
    m_activator(getConfData()->m_AdjChannelList, m_commonObjects, module->getVeinModuleName())
{
    connect(&m_activator, &AdjustmentModuleActivator::sigActivationReady, this, &cAdjustmentModuleMeasProgram::onActivationReady);
    connect(&m_activator, &AdjustmentModuleActivator::sigDeactivationReady, this, &cAdjustmentModuleMeasProgram::onDeactivationReady);
    connect(&m_activator, &AdjustmentModuleActivator::sigRangesReloaded, this, &cAdjustmentModuleMeasProgram::onNewRanges);

    connect(&m_offsetTasks, &TaskTemplate::sigFinish, [&](bool ok) {
        if(ok)
            m_pPARAdjustOffset->setValue(m_currEnv.m_receivedPar);
    });

    m_computationStartState.addTransition(this, &cAdjustmentModuleMeasProgram::computationContinue, &m_computationFinishState);
    m_computationMachine.addState(&m_computationStartState);
    m_computationMachine.addState(&m_computationFinishState);

    m_computationMachine.setInitialState(&m_computationStartState);

    connect(&m_computationStartState, &QState::entered, this, &cAdjustmentModuleMeasProgram::computationStart);
    connect(&m_computationFinishState, &QState::entered, this, &cAdjustmentModuleMeasProgram::computationFinished);

    m_storageStartState.addTransition(this, &cAdjustmentModuleMeasProgram::storageContinue, &m_storageFinishState);
    m_storageMachine.addState(&m_storageStartState);
    m_storageMachine.addState(&m_storageFinishState);

    m_storageMachine.setInitialState(&m_storageStartState);

    connect(&m_storageStartState, &QState::entered, this, &cAdjustmentModuleMeasProgram::storageStart);
    connect(&m_storageFinishState, &QState::entered, this, &cAdjustmentModuleMeasProgram::storageFinished);

    m_adjustamplitudeGetCorrState.addTransition(this, &cAdjustmentModuleMeasProgram::adjustamplitudeContinue, &m_adjustamplitudeSetNodeState);
    m_adjustamplitudeGetCorrState.addTransition(this, &cAdjustmentModuleMeasProgram::adjustError, &m_adjustamplitudeFinishState);
    m_adjustamplitudeSetNodeState.addTransition(this, &cAdjustmentModuleMeasProgram::adjustamplitudeContinue, &m_adjustamplitudeFinishState);
    m_adjustamplitudeSetNodeState.addTransition(this, &cAdjustmentModuleMeasProgram::adjustError, &m_adjustamplitudeFinishState);
    m_adjustAmplitudeMachine.addState(&m_adjustamplitudeGetCorrState);
    m_adjustAmplitudeMachine.addState(&m_adjustamplitudeSetNodeState);
    m_adjustAmplitudeMachine.addState(&m_adjustamplitudeFinishState);

    m_adjustAmplitudeMachine.setInitialState(&m_adjustamplitudeGetCorrState);

    connect(&m_adjustamplitudeGetCorrState, &QState::entered, this, &cAdjustmentModuleMeasProgram::adjustamplitudeGetCorr);
    connect(&m_adjustamplitudeSetNodeState, &QState::entered, this, &cAdjustmentModuleMeasProgram::adjustamplitudeSetNode);

    m_adjustphaseGetCorrState.addTransition(this, &cAdjustmentModuleMeasProgram::adjustphaseContinue, &m_adjustphaseSetNodeState);
    m_adjustphaseGetCorrState.addTransition(this, &cAdjustmentModuleMeasProgram::adjustError, &m_adjustphaseFinishState);
    m_adjustphaseSetNodeState.addTransition(this, &cAdjustmentModuleMeasProgram::adjustphaseContinue, &m_adjustphaseFinishState);
    m_adjustphaseSetNodeState.addTransition(this, &cAdjustmentModuleMeasProgram::adjustError, &m_adjustphaseFinishState);
    m_adjustPhaseMachine.addState(&m_adjustphaseGetCorrState);
    m_adjustPhaseMachine.addState(&m_adjustphaseSetNodeState);
    m_adjustPhaseMachine.addState(&m_adjustphaseFinishState);

    m_adjustPhaseMachine.setInitialState(&m_adjustphaseGetCorrState);

    connect(&m_adjustphaseGetCorrState, &QState::entered, this, &cAdjustmentModuleMeasProgram::adjustphaseGetCorr);
    connect(&m_adjustphaseSetNodeState, &QState::entered, this, &cAdjustmentModuleMeasProgram::adjustphaseSetNode);
}

void cAdjustmentModuleMeasProgram::start()
{
    // nothing to start
}

void cAdjustmentModuleMeasProgram::stop()
{
    // and nothing to stop
}

void cAdjustmentModuleMeasProgram::activate()
{
    if(!checkExternalVeinComponents())
        return;
    m_activator.activate();
}

void cAdjustmentModuleMeasProgram::deactivate()
{
    m_activator.deactivate();
}

cAdjustmentModuleConfigData *cAdjustmentModuleMeasProgram::getConfData()
{
    return qobject_cast<cAdjustmentModuleConfiguration*>(m_pConfiguration.get())->getConfigurationData();
}

bool cAdjustmentModuleMeasProgram::checkExternalVeinComponents()
{
    bool ok = true;
    adjInfoType adjInfo = getConfData()->m_ReferenceAngle;
    const VeinStorage::AbstractDatabase* storageDb = m_pModule->getStorageDb();
    if (!storageDb->hasStoredValue(adjInfo.m_nEntity, adjInfo.m_sComponent))
        ok = false;
    adjInfo = getConfData()->m_ReferenceFrequency;
    if (!storageDb->hasStoredValue(adjInfo.m_nEntity, adjInfo.m_sComponent))
        ok = false;

    for (int i = 0; ok && i<getConfData()->m_nAdjustmentChannelCount; i++) {
        // we test if all configured actual value data exist
        QString chn = getConfData()->m_AdjChannelList.at(i);
        adjInfo = getConfData()->m_AdjChannelInfoHash[chn]->rmsAdjInfo;
        const QString errMagTemplate = "Entity %1 / component %2 not found";
        if (adjInfo.m_bAvail && !storageDb->hasStoredValue(adjInfo.m_nEntity, adjInfo.m_sComponent)) {
            notifyError(errMagTemplate.arg(adjInfo.m_nEntity).arg(adjInfo.m_sComponent));
            ok = false;
        }
        adjInfo = getConfData()->m_AdjChannelInfoHash[chn]->phaseAdjInfo;
        if (adjInfo.m_bAvail && !storageDb->hasStoredValue(adjInfo.m_nEntity, adjInfo.m_sComponent)) {
            notifyError(errMagTemplate.arg(adjInfo.m_nEntity).arg(adjInfo.m_sComponent));
            ok = false;
        }
        adjInfo = getConfData()->m_AdjChannelInfoHash[chn]->dcAdjInfo;
        if (adjInfo.m_bAvail && !storageDb->hasStoredValue(adjInfo.m_nEntity, adjInfo.m_sComponent)) {
            notifyError(errMagTemplate.arg(adjInfo.m_nEntity).arg(adjInfo.m_sComponent));
            ok = false;
        }
        adjInfo = getConfData()->m_AdjChannelInfoHash[chn]->rangeAdjInfo;
        if (adjInfo.m_bAvail && !storageDb->hasStoredValue(adjInfo.m_nEntity, adjInfo.m_sComponent)) {
            notifyError(errMagTemplate.arg(adjInfo.m_nEntity).arg(adjInfo.m_sComponent));
            ok = false;
        }
    }
    return ok;
}

void cAdjustmentModuleMeasProgram::setAdjustEnvironment(QVariant var)
{
    m_currEnv.m_receivedPar = var;
    QStringList sl = var.toString().split(',');
    m_currEnv.m_channelAlias = sl.at(0);
    m_currEnv.m_rangeName = sl.at(1);
    m_currEnv.m_targetValue = sl.at(2).toDouble();
    m_currEnv.m_channelMName = m_commonObjects->m_channelAliasHash[m_currEnv.m_channelAlias];
}

double cAdjustmentModuleMeasProgram::cmpPhase(QVariant var)
{
    QList<double> list = var.value<QList<double> >();
    if(list.count() != 2)
        return qQNaN();
    return userAtan(list.at(1), list.at(0));
}

double cAdjustmentModuleMeasProgram::symAngle(double ang)
{
    double a = ang;
    while (a > 180.0)
        a -= 360.0;
    while (a < -180.0)
        a += 360.0;
    return a;
}

void cAdjustmentModuleMeasProgram::onActivationReady()
{
    connect(m_commonObjects->m_pcbConnection.getInterface().get(), &Zera::cPCBInterface::serverAnswer,
            this, &cAdjustmentModuleMeasProgram::catchInterfaceAnswer);
    setInterfaceValidation();

    m_bActive = true;
    emit activated();
}

void cAdjustmentModuleMeasProgram::onDeactivationReady()
{
    m_bActive = false;
    emit deactivated();
}

void cAdjustmentModuleMeasProgram::onNewRanges()
{
    setInterfaceValidation();
}

void cAdjustmentModuleMeasProgram::setInterfaceValidation()
{
    // we must set the validators for the adjustment commands now
    // we know the channel names and their ranges now
    // ....very special validator

    // first the validator for gain adjustment
    cDoubleValidator dValidator = cDoubleValidator(0, 2000, 1e-7);
    cAdjustValidator3d* adjValidator = new cAdjustValidator3d(this);
    for (int i = 0; i < getConfData()->m_nAdjustmentChannelCount; i++) {
        QString sysName = getConfData()->m_AdjChannelList.at(i);
        if (getConfData()->m_AdjChannelInfoHash[sysName]->rmsAdjInfo.m_bAvail) {
            const AdjustChannelInfo* adjChnInfo = m_commonObjects->m_adjustChannelInfoHash[getConfData()->m_AdjChannelList.at(i)].get();
            adjValidator->addValidator(adjChnInfo->m_sAlias, adjChnInfo->m_sRangelist, dValidator);
        }
    }
    m_pPARAdjustAmplitude->setValidator(adjValidator);

    // validator for offset adjustment
    cDoubleValidator dOffsetValidator = cDoubleValidator(-2000, 2000, 1e-7);
    adjValidator = new cAdjustValidator3d(this);
    for (int i = 0; i < getConfData()->m_nAdjustmentChannelCount; i++) {
        QString sysName = getConfData()->m_AdjChannelList.at(i);
        if (getConfData()->m_AdjChannelInfoHash[sysName]->dcAdjInfo.m_bAvail) {
            const AdjustChannelInfo* adjChnInfo = m_commonObjects->m_adjustChannelInfoHash[getConfData()->m_AdjChannelList.at(i)].get();
            adjValidator->addValidator(adjChnInfo->m_sAlias, adjChnInfo->m_sRangelist, dOffsetValidator);
        }
    }
    m_pPARAdjustOffset->setValidator(adjValidator);

    // validator for phase adjustment
    dValidator = cDoubleValidator(-360.0, 360.0, 1e-7);
    adjValidator = new cAdjustValidator3d(this);
    for (int i = 0; i < getConfData()->m_nAdjustmentChannelCount; i++) {
        QString sysName = getConfData()->m_AdjChannelList.at(i);
        if (getConfData()->m_AdjChannelInfoHash[sysName]->phaseAdjInfo.m_bAvail) {
            const AdjustChannelInfo* adjChnInfo =
                m_commonObjects->m_adjustChannelInfoHash[getConfData()->m_AdjChannelList.at(i)].get();
            adjValidator->addValidator(adjChnInfo->m_sAlias, adjChnInfo->m_sRangelist, dValidator);
        }
    }
    m_pPARAdjustPhase->setValidator(adjValidator);

    // validator for adjustment status setting
    cIntValidator iValidator = cIntValidator(0, 255);
    cAdjustValidator3i* adjValidatori = new cAdjustValidator3i(this);
    for (int i = 0; i < getConfData()->m_nAdjustmentChannelCount; i++) {
        const AdjustChannelInfo* adjChnInfo =
            m_commonObjects->m_adjustChannelInfoHash[getConfData()->m_AdjChannelList.at(i)].get();
        adjValidatori->addValidator(adjChnInfo->m_sAlias, adjChnInfo->m_sRangelist, iValidator);
    }
    m_pPARAdjustGainStatus->setValidator(adjValidatori);
    adjValidatori = new cAdjustValidator3i(*adjValidatori);
    m_pPARAdjustPhaseStatus->setValidator(adjValidatori);
    adjValidatori = new cAdjustValidator3i(*adjValidatori);
    m_pPARAdjustOffsetStatus->setValidator(adjValidatori);

    cAdjustValidator2* adjInitValidator = new cAdjustValidator2(this);
    for (int i = 0; i < getConfData()->m_nAdjustmentChannelCount; i++) {
        const AdjustChannelInfo* adjChnInfo =
            m_commonObjects->m_adjustChannelInfoHash[getConfData()->m_AdjChannelList.at(i)].get();
        adjInitValidator->addValidator(adjChnInfo->m_sAlias, adjChnInfo->m_sRangelist);
    }
    m_pPARAdjustInit->setValidator(adjInitValidator);

    // we accept every thing here and test command when we work on it
    cAdjustValidatorFine* adjValidatorFine = new cAdjustValidatorFine();
    m_pPARAdjustSend->setValidator(adjValidatorFine);
    adjValidatorFine = new cAdjustValidatorFine();
    m_pPARAdjustPCBData->setValidator(adjValidatorFine);
    adjValidatorFine = new cAdjustValidatorFine();
    m_pPARAdjustClampData->setValidator(adjValidatorFine);
}

void cAdjustmentModuleMeasProgram::generateVeinInterface()
{
    QString key;
    cIntValidator* iValidator;
    cSCPIInfo* scpiInfo;

    m_pPARComputation = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->m_pModuleValidator,
                                                 key = QString("PAR_Computation"),
                                                 QString("Start computation of adjustment coefficients"),
                                                 QVariant(int(0)),
                                                 false); // no deferred notification necessary

    m_pModule->m_veinModuleParameterMap[key] = m_pPARComputation;
    scpiInfo = new cSCPIInfo("CALCULATE", "COMPUTATION", "10", m_pPARComputation->getName(), "0", "");
    m_pPARComputation->setSCPIInfo(scpiInfo);
    iValidator = new cIntValidator(0,1);
    m_pPARComputation->setValidator(iValidator);
    connect(m_pPARComputation, &VfModuleParameter::sigValueChanged, this, &cAdjustmentModuleMeasProgram::computationStartCommand);


    m_pPARStorage = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->m_pModuleValidator,
                                             key = QString("PAR_Storage"),
                                             QString("Save adjustment data"),
                                             QVariant(int(0)),
                                             true); // deferred notification necessary

    m_pModule->m_veinModuleParameterMap[key] = m_pPARStorage;
    scpiInfo = new cSCPIInfo("CALCULATE", "STORAGE", "10", m_pPARStorage->getName(), "0", "");
    m_pPARStorage->setSCPIInfo(scpiInfo);
    iValidator = new cIntValidator(1,2);
    m_pPARStorage->setValidator(iValidator);
    connect(m_pPARStorage, &VfModuleParameter::sigValueChanged, this, &cAdjustmentModuleMeasProgram::storageStartCommand);


    m_pPARAdjustGainStatus = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->m_pModuleValidator,
                                                  key = QString("PAR_AdjustGainStatus"),
                                                  QString("Gain adjustment status"),
                                                  QVariant(int(0)),
                                                  true); // deferred notification necessary

    m_pModule->m_veinModuleParameterMap[key] = m_pPARAdjustGainStatus;
    scpiInfo = new cSCPIInfo("CALCULATE", "GSTATUS", "10", m_pPARAdjustGainStatus->getName(), "0", "");
    m_pPARAdjustGainStatus->setSCPIInfo(scpiInfo);
    connect(m_pPARAdjustGainStatus, &VfModuleParameter::sigValueChanged, this, &cAdjustmentModuleMeasProgram::setAdjustGainStatusStartCommand);

    m_pPARAdjustPhaseStatus = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->m_pModuleValidator,
                                                       key = QString("PAR_AdjustPhaseStatus"),
                                                       QString("Phase adjustment status"),
                                                       QVariant(int(0)),
                                                       true); // deferred notification necessary

    m_pModule->m_veinModuleParameterMap[key] = m_pPARAdjustPhaseStatus;
    scpiInfo = new cSCPIInfo("CALCULATE", "PSTATUS", "10", m_pPARAdjustPhaseStatus->getName(), "0", "");
    m_pPARAdjustPhaseStatus->setSCPIInfo(scpiInfo);
    connect(m_pPARAdjustPhaseStatus, &VfModuleParameter::sigValueChanged, this, &cAdjustmentModuleMeasProgram::setAdjustPhaseStatusStartCommand);

    m_pPARAdjustOffsetStatus = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->m_pModuleValidator,
                                                        key = QString("PAR_AdjustOffsetStatus"),
                                                        QString("Offset adjustment status"),
                                                        QVariant(int(0)),
                                                        true); // deferred notification necessary

    m_pModule->m_veinModuleParameterMap[key] = m_pPARAdjustOffsetStatus;
    scpiInfo = new cSCPIInfo("CALCULATE", "OSTATUS", "10", m_pPARAdjustOffsetStatus->getName(), "0", "");
    m_pPARAdjustOffsetStatus->setSCPIInfo(scpiInfo);
    connect(m_pPARAdjustOffsetStatus, &VfModuleParameter::sigValueChanged, this, &cAdjustmentModuleMeasProgram::setAdjustOffsetStatusStartCommand);


    m_pPARAdjustInit = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->m_pModuleValidator,
                                                key = QString("PAR_AdjustInit"),
                                                QString("Initialize adjustment data"),
                                                QVariant(int(0)),
                                                false); // no deferred notification necessary

    m_pModule->m_veinModuleParameterMap[key] = m_pPARAdjustInit;
    scpiInfo = new cSCPIInfo("CALCULATE", "INIT", "10", m_pPARAdjustInit->getName(), "0", "");
    m_pPARAdjustInit->setSCPIInfo(scpiInfo);
    connect(m_pPARAdjustInit, &VfModuleParameter::sigValueChanged, this, &cAdjustmentModuleMeasProgram::setAdjustInitStartCommand);


    m_pPARAdjustAmplitude = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->m_pModuleValidator,
                                                     key = QString("PAR_AdjustAmplitude"),
                                                     QString("One amplitude adjustment node"),
                                                     QVariant(QString("")),
                                                     true); // deferred notification necessary

    m_pModule->m_veinModuleParameterMap[key] = m_pPARAdjustAmplitude;
    scpiInfo = new cSCPIInfo("CALCULATE", "AMPLITUDE", "10", m_pPARAdjustAmplitude->getName(), "0", "");
    m_pPARAdjustAmplitude->setSCPIInfo(scpiInfo);
    // we will set the validator later after activation we will know the channel names and their ranges
    connect(m_pPARAdjustAmplitude, &VfModuleParameter::sigValueChanged, this, &cAdjustmentModuleMeasProgram::setAdjustAmplitudeStartCommand);


    m_pPARAdjustPhase = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->m_pModuleValidator,
                                                 key = QString("PAR_AdjustPhase"),
                                                 QString("One phase adjustment node"),
                                                 QVariant(QString("")),
                                                 true); // no deferred notification necessary

    m_pModule->m_veinModuleParameterMap[key] = m_pPARAdjustPhase;
    scpiInfo = new cSCPIInfo("CALCULATE", "PHASE", "10", m_pPARAdjustPhase->getName(), "0", "");
    m_pPARAdjustPhase->setSCPIInfo(scpiInfo);
    // we will set the validator later after activation we will know the channel names and their ranges
    connect(m_pPARAdjustPhase, &VfModuleParameter::sigValueChanged, this, &cAdjustmentModuleMeasProgram::setAdjustPhaseStartCommand);

    m_pPARAdjustOffset = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->m_pModuleValidator,
                                                  key = QString("PAR_Adjustoffset"),
                                                  QString("One offset adjustment node"),
                                                  QVariant(QString("")),
                                                  true); // no deferred notification necessary

    m_pModule->m_veinModuleParameterMap[key] = m_pPARAdjustOffset;
    // we will set the validator later after activation we will know the channel names and their ranges
    scpiInfo = new cSCPIInfo("CALCULATE", "OFFSET", "10", m_pPARAdjustOffset->getName(), "0", "");
    m_pPARAdjustOffset->setSCPIInfo(scpiInfo);
    connect(m_pPARAdjustOffset, &VfModuleParameter::sigValueChanged, this, &cAdjustmentModuleMeasProgram::setAdjustOffsetStartCommand);

    m_pPARAdjustSend = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->m_pModuleValidator,
                                                key = QString("PAR_AdjustSend"),
                                                QString("Send command to specified port"),
                                                QVariant(QString("")),
                                                false,
                                                true); // deferred query notification necessary !!!!!

    m_pModule->m_veinModuleParameterMap[key] = m_pPARAdjustSend;
    // we will set the validator later after activation we will know the channel names and their ranges
    scpiInfo = new cSCPIInfo("CALCULATE", "SEND", "2", m_pPARAdjustSend->getName(), "0", "");
    m_pPARAdjustSend->setSCPIInfo(scpiInfo);
    connect(m_pPARAdjustSend, &VfModuleParameter::sigValueQuery, this, &cAdjustmentModuleMeasProgram::transparentDataSend2Port);

    m_pPARAdjustPCBData = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->m_pModuleValidator,
                                                   key = QString("PAR_AdjustPCBData"),
                                                   QString("Get and set pcb adjustment data"),
                                                   QVariant(QString("")),
                                                   true,  // deferred command notification necessary !!!!!
                                                   true); // deferred query notification necessary !!!!!
    m_pModule->m_veinModuleParameterMap[key] = m_pPARAdjustPCBData;
    // we will set the validator later after activation we will know the channel names and their ranges
    scpiInfo = new cSCPIInfo("CALCULATE", "PCB", "18", m_pPARAdjustPCBData->getName(), "0", "");
    m_pPARAdjustPCBData->setSCPIInfo(scpiInfo);
    connect(m_pPARAdjustPCBData, &VfModuleParameter::sigValueChanged, this, &cAdjustmentModuleMeasProgram::writePCBAdjustmentData);
    connect(m_pPARAdjustPCBData, &VfModuleParameter::sigValueQuery, this, &cAdjustmentModuleMeasProgram::readPCBAdjustmentData);

    m_pPARAdjustClampData = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->m_pModuleValidator,
                                                     key = QString("PAR_AdjustCLAMPData"),
                                                     QString("Get and set clamp adjustment data"),
                                                     QVariant(QString("")),
                                                     true, // deferred command notification necessary !!!!!
                                                     true); // deferred query notification necessary !!!!!
    m_pModule->m_veinModuleParameterMap[key] = m_pPARAdjustClampData;
    // we will set the validator later after activation we will know the channel names and their ranges
    scpiInfo = new cSCPIInfo("CALCULATE", "CLAMP", "18", m_pPARAdjustClampData->getName(), "0", "");
    m_pPARAdjustClampData->setSCPIInfo(scpiInfo);
    connect(m_pPARAdjustClampData, &VfModuleParameter::sigValueChanged, this, &cAdjustmentModuleMeasProgram::writeCLAMPAdjustmentData);
    connect(m_pPARAdjustClampData, &VfModuleParameter::sigValueQuery, this, &cAdjustmentModuleMeasProgram::readCLAMPAdjustmentData);
}

void cAdjustmentModuleMeasProgram::computationStartCommand(QVariant var)
{
    if (var.toInt() == 1)
        m_computationMachine.start();
}

void cAdjustmentModuleMeasProgram::computationStart()
{
    m_MsgNrCmdList[m_commonObjects->m_pcbConnection.getInterface()->adjustComputation()] = adjustcomputation;
}

void cAdjustmentModuleMeasProgram::computationFinished()
{
    m_pPARComputation->setValue(QVariant(0));
}

void cAdjustmentModuleMeasProgram::storageStartCommand(QVariant var)
{
    StorageType par = static_cast<StorageType>(var.toInt());
    if(par == INTERNAL || par == CLAMP) {
        m_storageType = par;
        m_storageMachine.start();
    }
}

void cAdjustmentModuleMeasProgram::storageStart()
{
    if(m_storageType == INTERNAL)
        m_MsgNrCmdList[m_commonObjects->m_pcbConnection.getInterface()->adjustStorage()] = adjuststorage;
    else //if(m_storageType == CLAMP) // there are no tests for this yet
        m_MsgNrCmdList[m_commonObjects->m_pcbConnection.getInterface()->adjustStorageClamp()] = adjuststorage;
}

void cAdjustmentModuleMeasProgram::storageFinished()
{
    m_pPARStorage->setValue(QVariant(0));
}

void cAdjustmentModuleMeasProgram::setAdjustGainStatusStartCommand(QVariant var)
{
    QStringList sl = var.toString().split(',');
    QString sysName = m_commonObjects->m_channelAliasHash[sl.at(0)];;
    m_MsgNrCmdList[m_commonObjects->m_pcbConnection.getInterface()->setAdjustGainStatus(sysName, sl.at(1), sl.at(2).toInt())] = setadjustgainstatus;
}

void cAdjustmentModuleMeasProgram::setAdjustPhaseStatusStartCommand(QVariant var)
{
    QStringList sl = var.toString().split(',');
    QString sysName = m_commonObjects->m_channelAliasHash[sl.at(0)];;
    m_MsgNrCmdList[m_commonObjects->m_pcbConnection.getInterface()->setAdjustPhaseStatus(sysName, sl.at(1), sl.at(2).toInt())] = setadjustphasestatus;
}

void cAdjustmentModuleMeasProgram::setAdjustOffsetStatusStartCommand(QVariant var)
{
    QStringList sl = var.toString().split(',');
    QString sysName = m_commonObjects->m_channelAliasHash[sl.at(0)];;
    m_MsgNrCmdList[m_commonObjects->m_pcbConnection.getInterface()->setAdjustOffsetStatus(sysName, sl.at(1), sl.at(2).toInt())] = setadjustoffsetstatus;
}

void cAdjustmentModuleMeasProgram::setAdjustInitStartCommand(QVariant var)
{
    QStringList sl = var.toString().split(',');
    QString chnName = sl.at(0);
    if (m_adjustIteratorHash.contains(chnName))
        delete m_adjustIteratorHash.take(chnName);
    m_adjustIteratorHash[chnName] = new cAdjustIterators();
}

double cAdjustmentModuleMeasProgram::calcAbsoluteError(double actualValue, double targetValue)
{
    return fabs(100 * (targetValue - actualValue) / actualValue);
}

double cAdjustmentModuleMeasProgram::calcAdjAbsoluteError()
{
    return calcAbsoluteError(m_currEnv.m_actualValue, m_currEnv.m_targetValue);
}

bool cAdjustmentModuleMeasProgram::checkRangeIsWanted(QString adjType)
{
    const VeinStorage::AbstractDatabase *storageDb = m_pModule->getStorageDb();
    int rangeEntity = getConfData()->m_AdjChannelInfoHash[m_currEnv.m_channelMName]->rangeAdjInfo.m_nEntity;
    QString rangeComponent = getConfData()->m_AdjChannelInfoHash[m_currEnv.m_channelMName]->rangeAdjInfo.m_sComponent;
    QString currentRange = storageDb->getStoredValue(rangeEntity, rangeComponent).toString();
    if(currentRange != m_currEnv.m_rangeName) {
        notifyError(QString("Wrong range on %1 adjustment! Wanted: %2 / Current: %3").arg(
            adjType, m_currEnv.m_rangeName, currentRange));
        return false;
    }
    return true;
}

void cAdjustmentModuleMeasProgram::setAdjustAmplitudeStartCommand(QVariant var)
{
    setAdjustEnvironment(var);
    if(!checkRangeIsWanted("gain")) {
        m_pPARAdjustAmplitude->setError();
        return;
    }

    int adjustEntity = getConfData()->m_AdjChannelInfoHash[m_currEnv.m_channelMName]->rmsAdjInfo.m_nEntity;
    QString adjustComponent = getConfData()->m_AdjChannelInfoHash[m_currEnv.m_channelMName]->rmsAdjInfo.m_sComponent;

    const VeinStorage::AbstractDatabase *storageDb = m_pModule->getStorageDb();
    m_currEnv.m_actualValue = storageDb->getStoredValue(adjustEntity, adjustComponent).toDouble();
    bool outOfLimits = calcAdjAbsoluteError() > maxAmplitudeErrorPercent;
    if(outOfLimits) {
        notifyError(QString("Gain to adjust is out of limit! Wanted: %1 / Current: %2")
                        .arg(m_currEnv.m_targetValue)
                        .arg(m_currEnv.m_actualValue));
        m_pPARAdjustAmplitude->setError();
        return;
    }
    m_adjustAmplitudeMachine.start();
}

void cAdjustmentModuleMeasProgram::adjustamplitudeGetCorr()
{
    m_MsgNrCmdList[m_commonObjects->m_pcbConnection.getInterface()->getAdjGainCorrection(
        m_currEnv.m_channelMName, m_currEnv.m_rangeName, m_currEnv.m_actualValue)] = getadjgaincorrection;
}

void cAdjustmentModuleMeasProgram::adjustamplitudeSetNode()
{
    // we will not crash if the user forgot to initialize the iterators
    // but we will get error messages from server if the iterator becomes
    // greater than adjustment order
    cAdjustIterators *adjPhaseGainIterators;
    if (m_adjustIteratorHash.contains(m_currEnv.m_channelAlias))
        adjPhaseGainIterators = m_adjustIteratorHash[m_currEnv.m_channelAlias];
    else
        m_adjustIteratorHash[m_currEnv.m_channelAlias] = adjPhaseGainIterators = new cAdjustIterators();
    // we simlpy correct the actualvalue before calculating corr
    double Corr = m_currEnv.m_targetValue * m_currEnv.m_AdjustCorrection / m_currEnv.m_actualValue;
    m_MsgNrCmdList[m_commonObjects->m_pcbConnection.getInterface()->setGainNode(
        m_currEnv.m_channelMName,
        m_currEnv.m_rangeName,
        adjPhaseGainIterators->m_nAdjustGainIt,
        Corr,
        m_currEnv.m_targetValue)] = setgainnode;
    adjPhaseGainIterators->m_nAdjustGainIt++;
}

void cAdjustmentModuleMeasProgram::setAdjustPhaseStartCommand(QVariant var)
{
    setAdjustEnvironment(var);
    if(!checkRangeIsWanted("phase")) {
        m_pPARAdjustPhase->setError();
        return;
    }

    int adjustEntity = getConfData()->m_AdjChannelInfoHash[m_currEnv.m_channelMName]->phaseAdjInfo.m_nEntity;
    QString adjustComponent = getConfData()->m_AdjChannelInfoHash[m_currEnv.m_channelMName]->phaseAdjInfo.m_sComponent;
    m_currEnv.m_actualValue = cmpPhase(m_pModule->getStorageDb()->getStoredValue(adjustEntity, adjustComponent));
    if(qIsNaN(m_currEnv.m_actualValue)) {
        notifyError("Phase to adjust has no actual value!");
        m_pPARAdjustPhase->setError();
        return;
    }
    double diffAngleAbs = fabs(symAngle(m_currEnv.m_actualValue - m_currEnv.m_targetValue));
    if(diffAngleAbs > maxPhaseErrorDegrees) {
        notifyError("Phase to adjust is out of limit!");
        m_pPARAdjustPhase->setError();
        return;
    }
    m_adjustPhaseMachine.start();
}

void cAdjustmentModuleMeasProgram::adjustphaseGetCorr()
{
    m_currEnv.m_AdjustFrequency = m_pModule->getStorageDb()->getStoredValue(getConfData()->m_ReferenceFrequency.m_nEntity, getConfData()->m_ReferenceFrequency.m_sComponent).toDouble();
    m_MsgNrCmdList[m_commonObjects->m_pcbConnection.getInterface()->getAdjPhaseCorrection(
        m_currEnv.m_channelMName,
        m_currEnv.m_rangeName,
        m_currEnv.m_AdjustFrequency)] = getadjphasecorrection;
}

void cAdjustmentModuleMeasProgram::adjustphaseSetNode()
{
    cAdjustIterators *adjPhaseGainIterators;
    if (m_adjustIteratorHash.contains(m_currEnv.m_channelAlias))
        adjPhaseGainIterators = m_adjustIteratorHash[m_currEnv.m_channelAlias];
    else
        m_adjustIteratorHash[m_currEnv.m_channelAlias] = adjPhaseGainIterators = new cAdjustIterators();
    double Corr = symAngle((m_currEnv.m_actualValue + m_currEnv.m_AdjustCorrection) - m_currEnv.m_targetValue); // we simlpy correct the actualvalue before calculating corr
    m_MsgNrCmdList[m_commonObjects->m_pcbConnection.getInterface()->setPhaseNode(
        m_currEnv.m_channelMName,
        m_currEnv.m_rangeName,
        adjPhaseGainIterators->m_nAdjustPhaseIt,
        Corr,
        m_currEnv.m_AdjustFrequency)] = setphasenode;
    adjPhaseGainIterators->m_nAdjustPhaseIt++;
}

void cAdjustmentModuleMeasProgram::setAdjustOffsetStartCommand(QVariant var)
{
    setAdjustEnvironment(var);
    if(!checkRangeIsWanted("offset")) {
        m_pPARAdjustOffset->setError();
        return;
    }

    int adjustEntity = getConfData()->m_AdjChannelInfoHash[m_currEnv.m_channelMName]->dcAdjInfo.m_nEntity;
    QString adjustComponent = getConfData()->m_AdjChannelInfoHash[m_currEnv.m_channelMName]->dcAdjInfo.m_sComponent;
    double adjustActualValue = m_pModule->getStorageDb()->getStoredValue(adjustEntity, adjustComponent).toDouble();
    m_offsetTasks.addSub(TaskOffset::create(m_commonObjects->m_pcbConnection.getInterface(),
                                            m_currEnv.m_channelMName, m_currEnv.m_rangeName,
                                            adjustActualValue, m_currEnv.m_targetValue,
                                            TRANSACTION_TIMEOUT, [&](QString errorMsg){
                             notifyError(errorMsg);
                             m_pPARAdjustOffset->setError();
                         }));
    m_offsetTasks.start();
}

void cAdjustmentModuleMeasProgram::transparentDataSend2Port(QVariant var)
{
    QList<QString> sl = var.toString().split(',');
    if (sl.count() == 2) { // we expect a port number and a command
        int port = sl.at(0).toInt();
        if (port == m_pModule->getNetworkConfig()->m_pcbServiceConnectionInfo.m_nPort) {
            m_MsgNrCmdList[m_commonObjects->m_pcbConnection.getInterface()->transparentCommand(sl.at(1))] = sendtransparentcmd;
            return;
        }
    }
    m_pPARAdjustSend->setError();
}

void cAdjustmentModuleMeasProgram::writePCBAdjustmentData(QVariant var)
{
    m_currEnv.m_receivedPar = var;
    m_MsgNrCmdList[m_commonObjects->m_pcbConnection.getInterface()->setPCBAdjustmentData(var.toString())] = setpcbadjustmentdata;
}

void cAdjustmentModuleMeasProgram::readPCBAdjustmentData(QVariant)
{
    m_MsgNrCmdList[m_commonObjects->m_pcbConnection.getInterface()->getPCBAdjustmentData()] = getpcbadjustmentdata;
}

void cAdjustmentModuleMeasProgram::writeCLAMPAdjustmentData(QVariant var)
{
    m_currEnv.m_receivedPar = var;
    m_MsgNrCmdList[m_commonObjects->m_pcbConnection.getInterface()->setClampAdjustmentData(var.toString())] = setclampadjustmentdata;
}

void cAdjustmentModuleMeasProgram::readCLAMPAdjustmentData(QVariant)
{
    m_MsgNrCmdList[m_commonObjects->m_pcbConnection.getInterface()->getClampAdjustmentData()] = getclampadjustmentdata;
}

void cAdjustmentModuleMeasProgram::catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer)
{
    if (msgnr == 0) { // 0 was reserved for async. messages
        m_activator.reloadRanges();
    }
    else {
        // because rangemodulemeasprogram, adjustment and rangeobsermatic share the same dsp interface
        if (m_MsgNrCmdList.contains(msgnr)) {
            int cmd = m_MsgNrCmdList.take(msgnr);
            switch (cmd)
            {
            case adjustcomputation:
                if (reply == ack)
                    emit computationContinue();
                else {
                    m_computationMachine.stop();
                    notifyError(adjustcomputationPCBErrMSG);
                }
                break;

            case adjuststorage:
                if (reply == ack)
                    emit storageContinue();
                else {
                    m_storageMachine.stop();
                    notifyError(adjuststoragePCBErrMSG);
                    m_pPARStorage->setError();
                }
                break;

            case setadjustgainstatus:
                if (reply == ack)
                    m_pPARAdjustGainStatus->setValue(QVariant(0));
                else {
                    notifyError(adjuststatusPCBErrMSG);
                    m_pPARAdjustGainStatus->setError();
                }
                break;

            case setadjustphasestatus:
                if (reply == ack)
                    m_pPARAdjustPhaseStatus->setValue(QVariant(0));
                else {
                    notifyError(adjuststatusPCBErrMSG);
                    m_pPARAdjustPhaseStatus->setError();
                }
                break;

            case setadjustoffsetstatus:
                if (reply == ack)
                    m_pPARAdjustOffsetStatus->setValue(QVariant(0));
                else {
                    notifyError(adjuststatusPCBErrMSG);
                    m_pPARAdjustOffsetStatus->setError();
                }
                break;

            case getadjgaincorrection:
                if (reply == ack) {
                    m_currEnv.m_AdjustCorrection = answer.toDouble();
                    emit adjustamplitudeContinue();
                }
                else {
                    emit adjustError();
                    notifyError(readGainCorrErrMsg);
                    m_pPARAdjustAmplitude->setError();
                }
                break;

            case setgainnode:
                if (reply == ack) {
                    emit adjustamplitudeContinue();
                    m_pPARAdjustAmplitude->setValue(m_currEnv.m_receivedPar);
                }
                else {
                    emit adjustError();
                    notifyError(setGainNodeErrMsg);
                    m_pPARAdjustAmplitude->setError();
                }
                break;

            case setoffsetnode:
                if (reply == ack) {
                    m_pPARAdjustOffset->setValue(m_currEnv.m_receivedPar);
                    emit adjustoffsetContinue();
                }
                else {
                    emit adjustError();
                    notifyError(setOffsetNodeErrMsg);
                    m_pPARAdjustOffset->setError();
                }
                break;

            case getadjphasecorrection:
                if (reply == ack) {
                    m_currEnv.m_AdjustCorrection = answer.toDouble();
                    emit adjustphaseContinue();
                }
                else {
                    emit adjustError();
                    notifyError(readPhaseCorrErrMsg);
                    m_pPARAdjustPhase->setError();
                }
                break;

            case setphasenode:
                if (reply == ack) {
                    m_pPARAdjustPhase->setValue(m_currEnv.m_receivedPar);
                    emit adjustphaseContinue();
                }
                else {
                    emit adjustError();
                    notifyError(setPhaseNodeErrMsg);
                    m_pPARAdjustPhase->setError();
                }
                break;

            case sendtransparentcmd:
                // if (reply == ack)
                // in any case we return the answer to client
                m_pPARAdjustSend->setValue(answer);
                break;

            case getpcbadjustmentdata:
                if (reply == ack)
                    m_pPARAdjustPCBData->setValue(answer);
                else {
                    m_pPARAdjustPCBData->setError();
                    notifyError(readPCBXMLMSG);
                }
                break;

            case setpcbadjustmentdata:
                if (reply == ack)
                    m_pPARAdjustPCBData->setValue(m_currEnv.m_receivedPar);
                else {
                    m_pPARAdjustPCBData->setError();
                    notifyError(writePCBXMLMSG);
                }
                break;

            case getclampadjustmentdata:
                if (reply == ack)
                    m_pPARAdjustClampData->setValue(answer);
                else {
                    m_pPARAdjustClampData->setError();
                    notifyError(readClampXMLMSG);
                }
                break;

            case setclampadjustmentdata:
                if (reply == ack)
                    m_pPARAdjustClampData->setValue(m_currEnv.m_receivedPar);
                else {
                    m_pPARAdjustClampData->setError();
                    notifyError(writeClampXMLMSG);
                }
                break;
            }
        }
    }
}

cAdjustIterators::cAdjustIterators()
{
    m_nAdjustGainIt = m_nAdjustPhaseIt = 0;
}
