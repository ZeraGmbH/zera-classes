#include "adjustmentmodulemeasprogram.h"
#include "adjustmentmodule.h"
#include "adjustvalidator.h"
#include "adjustmentmoduleconfiguration.h"
#include "debug.h"
#include <modulevalidator.h>
#include <reply.h>
#include <intvalidator.h>
#include <useratan.h>
#include <rminterface.h>
#include <errormessages.h>
#include <math.h>

namespace ADJUSTMENTMODULE
{

cAdjustmentModuleMeasProgram::cAdjustmentModuleMeasProgram(cAdjustmentModule* module, Zera::Proxy::cProxy* proxy, std::shared_ptr<cBaseModuleConfiguration> pConfiguration)
    :cBaseMeasWorkProgram(pConfiguration), m_pModule(module), m_pProxy(proxy)
{
    m_pRMInterface = new Zera::Server::cRMInterface();
    m_bAuthorized = true; // per default we are authorized

    // setting up statemachine for "activating" adjustment
    // m_rmConnectState.addTransition is done in rmConnect
    m_IdentifyState.addTransition(this, &cAdjustmentModuleMeasProgram::activationContinue, &m_readResourceTypesState);
    m_readResourceTypesState.addTransition(this, &cAdjustmentModuleMeasProgram::activationContinue, &m_readResourceState);
    m_readResourceState.addTransition(this, &cAdjustmentModuleMeasProgram::activationContinue, &m_readResourceInfoState);
    m_readResourceInfoState.addTransition(this, &cAdjustmentModuleMeasProgram::activationContinue, &m_readResourceInfoLoopState);
    m_readResourceInfoLoopState.addTransition(this, &cAdjustmentModuleMeasProgram::activationContinue, &m_pcbConnectionState);
    m_readResourceInfoLoopState.addTransition(this, &cAdjustmentModuleMeasProgram::activationLoop, &m_readResourceInfoState);
    m_pcbConnectionState.addTransition(this, &cAdjustmentModuleMeasProgram::activationContinue, &m_pcbConnectionLoopState);
    m_pcbConnectionLoopState.addTransition(this, &cAdjustmentModuleMeasProgram::activationContinue, &m_readChnAliasState);
    m_pcbConnectionLoopState.addTransition(this, &cAdjustmentModuleMeasProgram::activationLoop, &m_pcbConnectionState);
    m_readChnAliasState.addTransition(this, &cAdjustmentModuleMeasProgram::activationContinue, &m_readChnAliasLoopState);
    m_readChnAliasLoopState.addTransition(this, &cAdjustmentModuleMeasProgram::activationContinue, &m_readRangelistState);
    m_readChnAliasLoopState.addTransition(this, &cAdjustmentModuleMeasProgram::activationLoop, &m_readChnAliasState);
    m_readRangelistState.addTransition(this, &cAdjustmentModuleMeasProgram::activationContinue, &m_readRangelistLoopState);
    m_readRangelistLoopState.addTransition(this, &cAdjustmentModuleMeasProgram::activationContinue, &m_searchActualValuesState);
    m_readRangelistLoopState.addTransition(this, &cAdjustmentModuleMeasProgram::activationLoop, &m_readRangelistState);
    m_searchActualValuesState.addTransition(this, &cAdjustmentModuleMeasProgram::activationContinue, &m_activationDoneState);

    m_activationMachine.addState(&m_rmConnectState);
    m_activationMachine.addState(&m_IdentifyState);
    m_activationMachine.addState(&m_readResourceTypesState);
    m_activationMachine.addState(&m_readResourceState);
    m_activationMachine.addState(&m_readResourceInfoState);
    m_activationMachine.addState(&m_readResourceInfoLoopState);
    m_activationMachine.addState(&m_pcbConnectionState);
    m_activationMachine.addState(&m_pcbConnectionLoopState);
    m_activationMachine.addState(&m_readChnAliasState);
    m_activationMachine.addState(&m_readChnAliasLoopState);
    m_activationMachine.addState(&m_readRangelistState);
    m_activationMachine.addState(&m_readRangelistLoopState);
    m_activationMachine.addState(&m_searchActualValuesState);
    m_activationMachine.addState(&m_activationDoneState);

    m_activationMachine.setInitialState(&m_rmConnectState);

    connect(&m_rmConnectState, &QState::entered, this, &cAdjustmentModuleMeasProgram::rmConnect);
    connect(&m_IdentifyState, &QState::entered, this, &cAdjustmentModuleMeasProgram::sendRMIdent);
    connect(&m_readResourceTypesState, &QState::entered, this, &cAdjustmentModuleMeasProgram::readResourceTypes);
    connect(&m_readResourceState, &QState::entered, this, &cAdjustmentModuleMeasProgram::readResource);
    connect(&m_readResourceInfoState, &QState::entered, this, &cAdjustmentModuleMeasProgram::readResourceInfo);
    connect(&m_readResourceInfoLoopState, &QState::entered, this, &cAdjustmentModuleMeasProgram::readResourceInfoLoop);
    connect(&m_pcbConnectionState, &QState::entered, this, &cAdjustmentModuleMeasProgram::pcbConnection);
    connect(&m_pcbConnectionLoopState, &QState::entered, this, &cAdjustmentModuleMeasProgram::pcbConnectionLoop);
    connect(&m_readChnAliasState, &QState::entered, this, &cAdjustmentModuleMeasProgram::readChnAlias);
    connect(&m_readChnAliasLoopState, &QState::entered, this, &cAdjustmentModuleMeasProgram::readChnAliasLoop);
    connect(&m_readRangelistState, &QState::entered, this, &cAdjustmentModuleMeasProgram::readRangelist);
    connect(&m_readRangelistLoopState, &QState::entered, this, &cAdjustmentModuleMeasProgram::readRangelistLoop);
    connect(&m_searchActualValuesState, &QState::entered, this, &cAdjustmentModuleMeasProgram::searchActualValues);
    connect(&m_activationDoneState, &QState::entered, this, &cAdjustmentModuleMeasProgram::activationDone);

    // setting up statemachine deactivation
    m_deactivateState.addTransition(this, &cAdjustmentModuleMeasProgram::deactivationContinue, &m_deactivateDoneState);

    m_deactivationMachine.addState(&m_deactivateState);
    m_deactivationMachine.addState(&m_deactivateDoneState);

    m_deactivationMachine.setInitialState(&m_deactivateState);

    connect(&m_deactivateState, &QState::entered, this, &cAdjustmentModuleMeasProgram::deactivateMeas);
    connect(&m_deactivateDoneState, &QState::entered, this, &cAdjustmentModuleMeasProgram::deactivateMeasDone);

    m_computationStartState.addTransition(this, &cAdjustmentModuleMeasProgram::computationContinue, &m_computationTestState);
    m_computationTestState.addTransition(this, &cAdjustmentModuleMeasProgram::computationContinue, &m_computationStartState);
    m_computationTestState.addTransition(this, &cAdjustmentModuleMeasProgram::computationDone, &m_computationFinishState);
    m_computationMachine.addState(&m_computationStartState);
    m_computationMachine.addState(&m_computationTestState);
    m_computationMachine.addState(&m_computationFinishState);
    m_computationMachine.setInitialState(&m_computationStartState);

    connect(&m_computationStartState, &QState::entered, this, &cAdjustmentModuleMeasProgram::computationStart);
    connect(&m_computationTestState, &QState::entered, this, &cAdjustmentModuleMeasProgram::computationTest);
    connect(&m_computationFinishState, &QState::entered, this, &cAdjustmentModuleMeasProgram::computationFinished);

    m_storageStartState.addTransition(this, &cAdjustmentModuleMeasProgram::storageContinue, &m_storageTestState);
    m_storageTestState.addTransition(this, &cAdjustmentModuleMeasProgram::storageContinue, &m_storageStartState);
    m_storageTestState.addTransition(this, &cAdjustmentModuleMeasProgram::storageDone, &m_storageFinishState);
    m_storageMachine.addState(&m_storageStartState);
    m_storageMachine.addState(&m_storageTestState);
    m_storageMachine.addState(&m_storageFinishState);
    m_storageMachine.setInitialState(&m_storageStartState);

    connect(&m_storageStartState, &QState::entered, this, &cAdjustmentModuleMeasProgram::storageStart);
    connect(&m_storageTestState, &QState::entered, this, &cAdjustmentModuleMeasProgram::storageTest);
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

    m_adjustOffsetGetCorrState.addTransition(this, &cAdjustmentModuleMeasProgram::adjustoffsetContinue, &m_adjustOffsetGetRejection);
    m_adjustOffsetGetCorrState.addTransition(this, &cAdjustmentModuleMeasProgram::adjustError, &m_adjustoffsetFinishState);
    m_adjustOffsetGetRejection.addTransition(this, &cAdjustmentModuleMeasProgram::adjustoffsetContinue, &m_adjustOffsetGetRejectionValue);
    m_adjustOffsetGetRejection.addTransition(this, &cAdjustmentModuleMeasProgram::adjustError, &m_adjustoffsetFinishState);
    m_adjustOffsetGetRejectionValue.addTransition(this, &cAdjustmentModuleMeasProgram::adjustoffsetContinue, &m_adjustoffsetSetNodeState);
    m_adjustOffsetGetRejectionValue.addTransition(this, &cAdjustmentModuleMeasProgram::adjustError, &m_adjustoffsetFinishState);
    m_adjustoffsetSetNodeState.addTransition(this, &cAdjustmentModuleMeasProgram::adjustoffsetContinue, &m_adjustoffsetFinishState);
    m_adjustoffsetSetNodeState.addTransition(this, &cAdjustmentModuleMeasProgram::adjustError, &m_adjustoffsetFinishState);
    m_adjustOffsetMachine.addState(&m_adjustOffsetGetCorrState);
    m_adjustOffsetMachine.addState(&m_adjustOffsetGetRejection);
    m_adjustOffsetMachine.addState(&m_adjustOffsetGetRejectionValue);
    m_adjustOffsetMachine.addState(&m_adjustoffsetSetNodeState);
    m_adjustOffsetMachine.addState(&m_adjustoffsetFinishState);
    m_adjustOffsetMachine.setInitialState(&m_adjustOffsetGetCorrState);

    connect(&m_adjustOffsetGetCorrState, &QState::entered, this, [&] () {
        m_AdjustActualValue = m_pModule->m_pStorageSystem->getStoredValue(m_AdjustEntity, m_AdjustComponent).toDouble();
        m_MsgNrCmdList[m_AdjustPCBInterface->getAdjOffsetCorrection(m_sAdjustSysName, m_sAdjustRange, m_AdjustActualValue)] = enGetAdjOffsetCorrection;
    });
    connect(&m_adjustOffsetGetRejection, &QState::entered, this, [&] () {
        m_MsgNrCmdList[m_AdjustPCBInterface->getRejection(m_sAdjustSysName, m_sAdjustRange)] = enGetAdjOffsetRejection;
    });
    connect(&m_adjustOffsetGetRejectionValue, &QState::entered, this, [&] () {
        m_MsgNrCmdList[m_AdjustPCBInterface->getUrvalue(m_sAdjustSysName, m_sAdjustRange)] = enGetAdjOffsetRejectionValue;
    });
    connect(&m_adjustoffsetSetNodeState, &QState::entered, this, [&] () {
        cAdjustIterators *pits;
        if (m_adjustIteratorHash.contains(m_sAdjustChannel))
            pits = m_adjustIteratorHash[m_sAdjustChannel];
        else
            m_adjustIteratorHash[m_sAdjustChannel] = pits = new cAdjustIterators();
        double rawActual = m_AdjustActualValue;
        if(fabs(m_AdjustCorrection) > 1e-3) {
            rawActual = m_AdjustActualValue - m_AdjustCorrection * m_AdjustRejectionValue / m_AdjustRejection;
        }
        double Corr = (m_AdjustTargetValue - rawActual) * m_AdjustRejection / m_AdjustRejectionValue;
        m_MsgNrCmdList[m_AdjustPCBInterface->setOffsetNode(m_sAdjustSysName, m_sAdjustRange, pits->m_nAdjustOffsetIt, Corr, m_AdjustTargetValue)] = setoffsetnode;
        pits->m_nAdjustOffsetIt++;
    });

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


cAdjustmentModuleMeasProgram::~cAdjustmentModuleMeasProgram()
{
}


void cAdjustmentModuleMeasProgram::start()
{
    // nothing to start
}


void cAdjustmentModuleMeasProgram::stop()
{
    // and nothing to stop
}

cAdjustmentModuleConfigData *cAdjustmentModuleMeasProgram::getConfData()
{
    return qobject_cast<cAdjustmentModuleConfiguration*>(m_pConfiguration.get())->getConfigurationData();
}


void cAdjustmentModuleMeasProgram::setAdjustEnvironment(QVariant var)
{
    QStringList sl;

    receivedPar = var;
    sl = var.toString().split(',');
    m_sAdjustChannel = sl.at(0);
    m_sAdjustRange = sl.at(1);
    m_AdjustTargetValue = sl.at(2).toDouble();
    m_sAdjustSysName = m_AliasChannelHash[m_sAdjustChannel];
    m_AdjustPCBInterface = m_adjustChannelInfoHash[m_sAdjustSysName]->m_pPCBInterface;
}


double cAdjustmentModuleMeasProgram::cmpPhase(QVariant var)
{
    QList<double> list;
    double phi;

    list = var.value<QList<double> >();
    phi = userAtan(list.at(1), list.at(0));
    return phi;
}


double cAdjustmentModuleMeasProgram::symAngle(double ang)
{
    double a;

    a = ang;
    while (a > 180.0)
        a -= 360.0;
    while (a < -180.0)
        a += 360.0;

    return a;
}


void cAdjustmentModuleMeasProgram::setInterfaceValidation()
{
    // we must set the validators for the adjustment commands now
    // we know the channel names and their ranges now
    // ....very special validator

    cAdjustValidator3d* adjValidatord;
    cAdjustValidator3i* adjValidatori;
    cAdjustValidatorFine* adjValidatorFine;

    cAdjustChannelInfo* adjChnInfo;
    QString sysName;

    // first the validator for amplitude adjustment
    cDoubleValidator dValidator = cDoubleValidator(0, 2000,1e-7);
    adjValidatord = new cAdjustValidator3d(this);
    for (int i = 0; i < getConfData()->m_nAdjustmentChannelCount; i++)
    {
        sysName = getConfData()->m_AdjChannelList.at(i);
        if (getConfData()->m_AdjChannelInfoHash[sysName]->amplitudeAdjInfo.m_bAvail)
        {
            adjChnInfo = m_adjustChannelInfoHash[getConfData()->m_AdjChannelList.at(i)];
            adjValidatord->addValidator(adjChnInfo->m_sAlias, adjChnInfo->m_sRangelist, dValidator);
        }
    }
    m_pPARAdjustAmplitude->setValidator(adjValidatord);

    // validator for offset adjustment
    adjValidatord = new cAdjustValidator3d(this);
    for (int i = 0; i < getConfData()->m_nAdjustmentChannelCount; i++)
    {
        sysName = getConfData()->m_AdjChannelList.at(i);
        if (getConfData()->m_AdjChannelInfoHash[sysName]->offsetAdjInfo.m_bAvail)
        {
            adjChnInfo = m_adjustChannelInfoHash[getConfData()->m_AdjChannelList.at(i)];
            adjValidatord->addValidator(adjChnInfo->m_sAlias, adjChnInfo->m_sRangelist, dValidator);
        }
    }
    m_pPARAdjustOffset->setValidator(adjValidatord);

    // validator for angle adjustment
    dValidator = cDoubleValidator(0, 360,1e-7);
    adjValidatord = new cAdjustValidator3d(this);
    for (int i = 0; i < getConfData()->m_nAdjustmentChannelCount; i++)
    {
        sysName = getConfData()->m_AdjChannelList.at(i);
        if (getConfData()->m_AdjChannelInfoHash[sysName]->phaseAdjInfo.m_bAvail)
        {
            adjChnInfo = m_adjustChannelInfoHash[getConfData()->m_AdjChannelList.at(i)];
            adjValidatord->addValidator(adjChnInfo->m_sAlias, adjChnInfo->m_sRangelist, dValidator);
        }
    }
    m_pPARAdjustPhase->setValidator(adjValidatord);

    // validator for adjustment status setting
    cIntValidator iValidator = cIntValidator(0,255);

    adjValidatori = new cAdjustValidator3i(this);
    for (int i = 0; i < getConfData()->m_nAdjustmentChannelCount; i++)
    {
        adjChnInfo = m_adjustChannelInfoHash[getConfData()->m_AdjChannelList.at(i)];
        adjValidatori->addValidator(adjChnInfo->m_sAlias, adjChnInfo->m_sRangelist, iValidator);
    }
    m_pPARAdjustGainStatus->setValidator(adjValidatori);
    adjValidatori = new cAdjustValidator3i(*adjValidatori);
    m_pPARAdjustPhaseStatus->setValidator(adjValidatori);
    adjValidatori = new cAdjustValidator3i(*adjValidatori);
    m_pPARAdjustOffsetStatus->setValidator(adjValidatori);

    cAdjustValidator2* adjInitValidator;

    adjInitValidator = new cAdjustValidator2(this);
    for (int i = 0; i < getConfData()->m_nAdjustmentChannelCount; i++)
    {
        adjChnInfo = m_adjustChannelInfoHash[getConfData()->m_AdjChannelList.at(i)];
        adjInitValidator->addValidator(adjChnInfo->m_sAlias, adjChnInfo->m_sRangelist);
    }
    m_pPARAdjustInit->setValidator(adjInitValidator);

    adjValidatorFine = new cAdjustValidatorFine(); // we accept every thing here and test command when we work on it
    m_pPARAdjustSend->setValidator(adjValidatorFine);
    adjValidatorFine = new cAdjustValidatorFine();
    m_pPARAdjustPCBData->setValidator(adjValidatorFine);
    adjValidatorFine = new cAdjustValidatorFine();
    m_pPARAdjustClampData->setValidator(adjValidatorFine);
}


void cAdjustmentModuleMeasProgram::generateInterface()
{
    QString key;
    cIntValidator* iValidator;
    cSCPIInfo* scpiInfo;

    m_pPARComputation = new cVeinModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                                 key = QString("PAR_Computation"),
                                                 QString("Start computation of adjustment coefficients"),
                                                 QVariant(int(0)),
                                                 false); // no deferred notification necessary

    m_pModule->veinModuleParameterHash[key] = m_pPARComputation;
    scpiInfo = new cSCPIInfo("CALCULATE", "COMPUTATION", "10", m_pPARComputation->getName(), "0", "");
    m_pPARComputation->setSCPIInfo(scpiInfo);
    iValidator = new cIntValidator(0,1);
    m_pPARComputation->setValidator(iValidator);
    connect(m_pPARComputation, &cVeinModuleParameter::sigValueChanged, this, &cAdjustmentModuleMeasProgram::computationStartCommand);


    m_pPARStorage = new cVeinModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                             key = QString("PAR_Storage"),
                                             QString("Save adjustment data"),
                                             QVariant(int(0)),
                                             true); // deferred notification necessary

    m_pModule->veinModuleParameterHash[key] = m_pPARStorage;
    scpiInfo = new cSCPIInfo("CALCULATE", "STORAGE", "10", m_pPARStorage->getName(), "0", "");
    m_pPARStorage->setSCPIInfo(scpiInfo);
    iValidator = new cIntValidator(1,2);
    m_pPARStorage->setValidator(iValidator);
    connect(m_pPARStorage, &cVeinModuleParameter::sigValueChanged, this, &cAdjustmentModuleMeasProgram::storageStartCommand);


    m_pPARAdjustGainStatus = new cVeinModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                                  key = QString("PAR_AdjustGainStatus"),
                                                  QString("Gain adjustment status"),
                                                  QVariant(int(0)),
                                                  true); // deferred notification necessary

    m_pModule->veinModuleParameterHash[key] = m_pPARAdjustGainStatus;
    scpiInfo = new cSCPIInfo("CALCULATE", "GSTATUS", "10", m_pPARAdjustGainStatus->getName(), "0", "");
    m_pPARAdjustGainStatus->setSCPIInfo(scpiInfo);
    connect(m_pPARAdjustGainStatus, &cVeinModuleParameter::sigValueChanged, this, &cAdjustmentModuleMeasProgram::setAdjustGainStatusStartCommand);

    m_pPARAdjustPhaseStatus = new cVeinModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                                       key = QString("PAR_AdjustPhaseStatus"),
                                                       QString("Phase adjustment status"),
                                                       QVariant(int(0)),
                                                       true); // deferred notification necessary

    m_pModule->veinModuleParameterHash[key] = m_pPARAdjustPhaseStatus;
    scpiInfo = new cSCPIInfo("CALCULATE", "PSTATUS", "10", m_pPARAdjustPhaseStatus->getName(), "0", "");
    m_pPARAdjustPhaseStatus->setSCPIInfo(scpiInfo);
    connect(m_pPARAdjustPhaseStatus, &cVeinModuleParameter::sigValueChanged, this, &cAdjustmentModuleMeasProgram::setAdjustPhaseStatusStartCommand);

    m_pPARAdjustOffsetStatus = new cVeinModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                                        key = QString("PAR_AdjustOffsetStatus"),
                                                        QString("Offset adjustment status"),
                                                        QVariant(int(0)),
                                                        true); // deferred notification necessary

    m_pModule->veinModuleParameterHash[key] = m_pPARAdjustOffsetStatus;
    scpiInfo = new cSCPIInfo("CALCULATE", "OSTATUS", "10", m_pPARAdjustOffsetStatus->getName(), "0", "");
    m_pPARAdjustOffsetStatus->setSCPIInfo(scpiInfo);
    connect(m_pPARAdjustOffsetStatus, &cVeinModuleParameter::sigValueChanged, this, &cAdjustmentModuleMeasProgram::setAdjustOffsetStatusStartCommand);


    m_pPARAdjustInit = new cVeinModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                                key = QString("PAR_AdjustInit"),
                                                QString("Initialize adjustment data"),
                                                QVariant(int(0)),
                                                false); // no deferred notification necessary

    m_pModule->veinModuleParameterHash[key] = m_pPARAdjustInit;
    scpiInfo = new cSCPIInfo("CALCULATE", "INIT", "10", m_pPARAdjustInit->getName(), "0", "");
    m_pPARAdjustInit->setSCPIInfo(scpiInfo);
    connect(m_pPARAdjustInit, &cVeinModuleParameter::sigValueChanged, this, &cAdjustmentModuleMeasProgram::setAdjustInitStartCommand);


    m_pPARAdjustAmplitude = new cVeinModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                                     key = QString("PAR_AdjustAmplitude"),
                                                     QString("One amplitude adjustment node"),
                                                     QVariant(QString("")),
                                                     true); // deferred notification necessary

    m_pModule->veinModuleParameterHash[key] = m_pPARAdjustAmplitude;
    scpiInfo = new cSCPIInfo("CALCULATE", "AMPLITUDE", "10", m_pPARAdjustAmplitude->getName(), "0", "");
    m_pPARAdjustAmplitude->setSCPIInfo(scpiInfo);
    // we will set the validator later after activation we will know the channel names and their ranges
    connect(m_pPARAdjustAmplitude, &cVeinModuleParameter::sigValueChanged, this, &cAdjustmentModuleMeasProgram::setAdjustAmplitudeStartCommand);


    m_pPARAdjustPhase = new cVeinModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                                 key = QString("PAR_AdjustPhase"),
                                                 QString("One phase adjustment node"),
                                                 QVariant(QString("")),
                                                 true); // no deferred notification necessary

    m_pModule->veinModuleParameterHash[key] = m_pPARAdjustPhase;
    scpiInfo = new cSCPIInfo("CALCULATE", "PHASE", "10", m_pPARAdjustPhase->getName(), "0", "");
    m_pPARAdjustPhase->setSCPIInfo(scpiInfo);
    // we will set the validator later after activation we will know the channel names and their ranges
    connect(m_pPARAdjustPhase, &cVeinModuleParameter::sigValueChanged, this, &cAdjustmentModuleMeasProgram::setAdjustPhaseStartCommand);

    m_pPARAdjustOffset = new cVeinModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                                  key = QString("PAR_Adjustoffset"),
                                                  QString("One offset adjustment node"),
                                                  QVariant(QString("")),
                                                  true); // no deferred notification necessary

    m_pModule->veinModuleParameterHash[key] = m_pPARAdjustOffset;
    // we will set the validator later after activation we will know the channel names and their ranges
    scpiInfo = new cSCPIInfo("CALCULATE", "OFFSET", "10", m_pPARAdjustOffset->getName(), "0", "");
    m_pPARAdjustOffset->setSCPIInfo(scpiInfo);
    connect(m_pPARAdjustOffset, &cVeinModuleParameter::sigValueChanged, this, &cAdjustmentModuleMeasProgram::setAdjustOffsetStartCommand);

    m_pPARAdjustSend = new cVeinModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                                key = QString("PAR_AdjustSend"),
                                                QString("Send command to specified port"),
                                                QVariant(QString("")),
                                                false,
                                                true); // deferred query notification necessary !!!!!

    m_pModule->veinModuleParameterHash[key] = m_pPARAdjustSend;
    // we will set the validator later after activation we will know the channel names and their ranges
    scpiInfo = new cSCPIInfo("CALCULATE", "SEND", "2", m_pPARAdjustSend->getName(), "0", "");
    m_pPARAdjustSend->setSCPIInfo(scpiInfo);
    connect(m_pPARAdjustSend, &cVeinModuleParameter::sigValueQuery, this, &cAdjustmentModuleMeasProgram::transparentDataSend2Port);

    m_pPARAdjustPCBData = new cVeinModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                                   key = QString("PAR_AdjustPCBData"),
                                                   QString("Get and set pcb adjustment data"),
                                                   QVariant(QString("")),
                                                   true,  // deferred command notification necessary !!!!!
                                                   true); // deferred query notification necessary !!!!!
    m_pModule->veinModuleParameterHash[key] = m_pPARAdjustPCBData;
    // we will set the validator later after activation we will know the channel names and their ranges
    scpiInfo = new cSCPIInfo("CALCULATE", "PCB", "18", m_pPARAdjustPCBData->getName(), "0", "");
    m_pPARAdjustPCBData->setSCPIInfo(scpiInfo);
    connect(m_pPARAdjustPCBData, &cVeinModuleParameter::sigValueChanged, this, &cAdjustmentModuleMeasProgram::writePCBAdjustmentData);
    connect(m_pPARAdjustPCBData, &cVeinModuleParameter::sigValueQuery, this, &cAdjustmentModuleMeasProgram::readPCBAdjustmentData);

    m_pPARAdjustClampData = new cVeinModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                                     key = QString("PAR_AdjustCLAMPData"),
                                                     QString("Get and set clamp adjustment data"),
                                                     QVariant(QString("")),
                                                     true, // deferred command notification necessary !!!!!
                                                     true); // deferred query notification necessary !!!!!
    m_pModule->veinModuleParameterHash[key] = m_pPARAdjustClampData;
    // we will set the validator later after activation we will know the channel names and their ranges
    scpiInfo = new cSCPIInfo("CALCULATE", "CLAMP", "18", m_pPARAdjustClampData->getName(), "0", "");
    m_pPARAdjustClampData->setSCPIInfo(scpiInfo);
    connect(m_pPARAdjustClampData, &cVeinModuleParameter::sigValueChanged, this, &cAdjustmentModuleMeasProgram::writeCLAMPAdjustmentData);
    connect(m_pPARAdjustClampData, &cVeinModuleParameter::sigValueQuery, this, &cAdjustmentModuleMeasProgram::readCLAMPAdjustmentData);
}


bool cAdjustmentModuleMeasProgram::isAuthorized()
{
    return m_bAuthorized;
}


void cAdjustmentModuleMeasProgram::rmConnect()
{
    // we instantiate a working resource manager interface first
    // so first we try to get a connection to resource manager over proxy
    m_pRMClient = m_pProxy->getConnection(getConfData()->m_RMSocket.m_sIP, getConfData()->m_RMSocket.m_nPort);
    m_rmConnectState.addTransition(m_pRMClient, &Zera::Proxy::cProxyClient::connected, &m_IdentifyState);
    // and then we set connection resource manager interface's connection
    m_pRMInterface->setClient(m_pRMClient); //
    // todo insert timer for timeout

    connect(m_pRMInterface, &Zera::Server::cRMInterface::serverAnswer, this, &cAdjustmentModuleMeasProgram::catchInterfaceAnswer);
    m_pProxy->startConnection(m_pRMClient);
}


void cAdjustmentModuleMeasProgram::sendRMIdent()
{
    m_MsgNrCmdList[m_pRMInterface->rmIdent(QString("Adjustment"))] = sendrmident;
}


void cAdjustmentModuleMeasProgram::readResourceTypes()
{
    m_MsgNrCmdList[m_pRMInterface->getResourceTypes()] = readresourcetypes;
}


void cAdjustmentModuleMeasProgram::readResource()
{
    m_MsgNrCmdList[m_pRMInterface->getResources("SENSE")] = readresource;
    activationIt = 0; // we prepare iteration for querying the pcbservers
}


void cAdjustmentModuleMeasProgram::readResourceInfo()
{
    m_MsgNrCmdList[m_pRMInterface->getResourceInfo("SENSE", getConfData()->m_AdjChannelList.at(activationIt))] = readresourceinfo;
}


void cAdjustmentModuleMeasProgram::readResourceInfoLoop()
{
    activationIt = (activationIt + 1) % getConfData()->m_nAdjustmentChannelCount;
    if (activationIt == 0)
        emit activationContinue();
    else
        emit activationLoop();
}


void cAdjustmentModuleMeasProgram::pcbConnection()
{
    QString sChannel;
    int port;
    Zera::Proxy::cProxyClient* pcbclient;

    cAdjustChannelInfo* adjustChannelInfo;

    sChannel = getConfData()->m_AdjChannelList.at(activationIt); // the system channel name we work on
    adjustChannelInfo = new cAdjustChannelInfo();
    m_adjustChannelInfoHash[sChannel] = adjustChannelInfo;

    port = m_chnPortHash[getConfData()->m_AdjChannelList.at(activationIt)]; // the port
    if (m_portChannelHash.contains(port))
    {
        // the channels share the same interface
        adjustChannelInfo->m_pPCBInterface = m_adjustChannelInfoHash[m_portChannelHash[port] ]->m_pPCBInterface;
        emit activationContinue();
    }
    else
    {
        m_portChannelHash[port] = sChannel;
        pcbclient = m_pProxy->getConnection(getConfData()->m_PCBSocket.m_sIP, port);
        m_pcbClientList.append(pcbclient);
        m_pcbConnectionState.addTransition(pcbclient, &Zera::Proxy::cProxyClient::connected, &m_pcbConnectionLoopState);
        pcbInterface = new Zera::Server::cPCBInterface();
        m_pcbInterfaceList.append(pcbInterface);
        pcbInterface->setClient(pcbclient);
        connect(pcbInterface, &Zera::Server::cPCBInterface::serverAnswer, this, &cAdjustmentModuleMeasProgram::catchInterfaceAnswer);
        adjustChannelInfo->m_pPCBInterface = pcbInterface;
        m_pProxy->startConnection((pcbclient));
    }
}


void cAdjustmentModuleMeasProgram::pcbConnectionLoop()
{
    activationIt = (activationIt + 1) % getConfData()->m_nAdjustmentChannelCount;
    if (activationIt == 0)
        emit activationContinue();
    else
        emit activationLoop();
}


void cAdjustmentModuleMeasProgram::readChnAlias()
{
    QString name;

    name = getConfData()->m_AdjChannelList.at(activationIt);
    m_MsgNrCmdList[m_adjustChannelInfoHash[name]->m_pPCBInterface->getAlias(name)] = readchnalias;
}


void cAdjustmentModuleMeasProgram::readChnAliasLoop()
{
    activationIt = (activationIt + 1) % getConfData()->m_nAdjustmentChannelCount;
    if (activationIt == 0)
        emit activationContinue();
    else
        emit activationLoop();
}


void cAdjustmentModuleMeasProgram::readRangelist()
{
    QString name;

    name = getConfData()->m_AdjChannelList.at(activationIt);
    m_MsgNrCmdList[m_adjustChannelInfoHash[name]->m_pPCBInterface->getRangeList(name)] = readrangelist;
}


void cAdjustmentModuleMeasProgram::readRangelistLoop()
{
    activationIt = (activationIt + 1) % getConfData()->m_nAdjustmentChannelCount;
    if (activationIt == 0)
        emit activationContinue();
    else
        emit activationLoop();
}


void cAdjustmentModuleMeasProgram::searchActualValues()
{
    bool error;
    adjInfoType adjInfo;

    error = false;

    adjInfo = getConfData()->m_ReferenceAngle;
    if (!m_pModule->m_pStorageSystem->hasStoredValue(adjInfo.m_nEntity, adjInfo.m_sComponent))
        error = true;
    adjInfo = getConfData()->m_ReferenceFrequency;
    if (!m_pModule->m_pStorageSystem->hasStoredValue(adjInfo.m_nEntity, adjInfo.m_sComponent))
        error = true;

    for (int i = 0; i < getConfData()->m_nAdjustmentChannelCount; i++)
    {
        // we test if all configured actual value data exist
        QString chn;

        chn = getConfData()->m_AdjChannelList.at(i);
        adjInfo = getConfData()->m_AdjChannelInfoHash[chn]->amplitudeAdjInfo;

        if (adjInfo.m_bAvail)
            if (!m_pModule->m_pStorageSystem->hasStoredValue(adjInfo.m_nEntity, adjInfo.m_sComponent))
            {
                error = true;
                break;
            }

        adjInfo = getConfData()->m_AdjChannelInfoHash[chn]->phaseAdjInfo;

        if (adjInfo.m_bAvail)
            if (!m_pModule->m_pStorageSystem->hasStoredValue(adjInfo.m_nEntity, adjInfo.m_sComponent))
            {
                error = true;
                break;
            }

        adjInfo = getConfData()->m_AdjChannelInfoHash[chn]->offsetAdjInfo;

        if (adjInfo.m_bAvail)
            if (!m_pModule->m_pStorageSystem->hasStoredValue(adjInfo.m_nEntity, adjInfo.m_sComponent))
            {
                error = true;
                break;
            }
    }

    if (error)
    {
        emit errMsg(moduleActValueErrMsg);
        emit activationError();
    }
    else
        emit activationContinue();
}


void cAdjustmentModuleMeasProgram::activationDone()
{
    m_bActive = true;
    setInterfaceValidation();
    connect(&m_AuthTimer, &QTimer::timeout, this , &cAdjustmentModuleMeasProgram::fetchAuthorizationStatus);
    m_AuthTimer.start(5000);
    emit activated();
}


void cAdjustmentModuleMeasProgram::deactivateMeas()
{
    m_bActive = false;
    emit deactivationContinue();
}


void cAdjustmentModuleMeasProgram::deactivateMeasDone()
{
    emit deactivated();
}


void cAdjustmentModuleMeasProgram::computationStartCommand(QVariant var)
{
    if (var.toInt() == 1)
    {
        computationIt = 0;
        m_computationMachine.start();
    }
}


void cAdjustmentModuleMeasProgram::computationStart()
{
    // we have to start adjustment coefficient computation for all particpating pcbservers
    m_MsgNrCmdList[m_pcbInterfaceList.at(computationIt)->adjustComputation()] = adjustcomputation;
}


void cAdjustmentModuleMeasProgram::computationTest()
{
    computationIt++;
    if (computationIt == m_pcbInterfaceList.count())
        emit computationDone();
    else
        emit computationContinue();
}


void cAdjustmentModuleMeasProgram::computationFinished()
{
    m_pPARComputation->setValue(QVariant(0));
}


void cAdjustmentModuleMeasProgram::storageStartCommand(QVariant var)
{
    int par;
    par = var.toInt();

    if ((par == 1) || (par == 2))
    {
        storageIt = 0;
        storageType = par;
        m_storageMachine.start();
    }
}


void cAdjustmentModuleMeasProgram::storageStart()
{
    // we have to start saving adjustment data for all particpating pcbservers
    if (storageType == 1)
        m_MsgNrCmdList[m_pcbInterfaceList.at(storageIt)->adjustStorage()] = adjuststorage;
    else
        m_MsgNrCmdList[m_pcbInterfaceList.at(storageIt)->adjustStorageClamp()] = adjuststorage;
}


void cAdjustmentModuleMeasProgram::storageTest()
{
    storageIt++;
    if (storageIt == m_pcbInterfaceList.count())
        emit storageDone();
    else
        emit storageContinue();
}


void cAdjustmentModuleMeasProgram::storageFinished()
{
    m_pPARStorage->setValue(QVariant(0));
}


void cAdjustmentModuleMeasProgram::setAdjustGainStatusStartCommand(QVariant var)
{
    QString sysName;
    QStringList sl;
    cAdjustChannelInfo* adjChannelinfo;

    sl = var.toString().split(',');
    sysName = m_AliasChannelHash[sl.at(0)];
    adjChannelinfo = m_adjustChannelInfoHash[sysName];

    m_MsgNrCmdList[adjChannelinfo->m_pPCBInterface->setAdjustGainStatus(sysName, sl.at(1), sl.at(2).toInt())] = setadjustgainstatus;
}


void cAdjustmentModuleMeasProgram::setAdjustPhaseStatusStartCommand(QVariant var)
{
    QString sysName;
    QStringList sl;
    cAdjustChannelInfo* adjChannelinfo;

    sl = var.toString().split(',');
    sysName = m_AliasChannelHash[sl.at(0)];
    adjChannelinfo = m_adjustChannelInfoHash[sysName];

    m_MsgNrCmdList[adjChannelinfo->m_pPCBInterface->setAdjustPhaseStatus(sysName, sl.at(1), sl.at(2).toInt())] = setadjustphasestatus;

}


void cAdjustmentModuleMeasProgram::setAdjustOffsetStatusStartCommand(QVariant var)
{
    QString sysName;
    QStringList sl;
    cAdjustChannelInfo* adjChannelinfo;

    sl = var.toString().split(',');
    sysName = m_AliasChannelHash[sl.at(0)];
    adjChannelinfo = m_adjustChannelInfoHash[sysName];

    m_MsgNrCmdList[adjChannelinfo->m_pPCBInterface->setAdjustOffsetStatus(sysName, sl.at(1), sl.at(2).toInt())] = setadjustoffsetstatus;
}


void cAdjustmentModuleMeasProgram::setAdjustInitStartCommand(QVariant var)
{
    QString chnName;
    QStringList sl;
    cAdjustIterators *pits; // pointer iterators

    sl = var.toString().split(',');
    chnName = sl.at(0);
    if (m_adjustIteratorHash.contains(chnName))
    {
        pits = m_adjustIteratorHash.take(chnName);
        delete pits;
    }

    pits = new cAdjustIterators();
    m_adjustIteratorHash[chnName] = pits;
}


void cAdjustmentModuleMeasProgram::setAdjustAmplitudeStartCommand(QVariant var)
{
    setAdjustEnvironment(var);
    m_AdjustEntity = getConfData()->m_AdjChannelInfoHash[m_sAdjustSysName]->amplitudeAdjInfo.m_nEntity;
    m_AdjustComponent = getConfData()->m_AdjChannelInfoHash[m_sAdjustSysName]->amplitudeAdjInfo.m_sComponent;
    m_adjustAmplitudeMachine.start();
}


void cAdjustmentModuleMeasProgram::adjustamplitudeGetCorr()
{
    m_AdjustActualValue = m_pModule->m_pStorageSystem->getStoredValue(m_AdjustEntity, m_AdjustComponent).toDouble();
    m_MsgNrCmdList[m_AdjustPCBInterface->getAdjGainCorrection(m_sAdjustSysName, m_sAdjustRange, m_AdjustActualValue)] = getadjgaincorrection;
}


void cAdjustmentModuleMeasProgram::adjustamplitudeSetNode()
{
    cAdjustIterators *pits;
    double Corr;

    // we will not crash if the user forgot to initialize the iterators
    // but we will get error messages from server if the iterator becomes
    // greater than adjustment order
    if (m_adjustIteratorHash.contains(m_sAdjustChannel))
        pits = m_adjustIteratorHash[m_sAdjustChannel];
    else
        m_adjustIteratorHash[m_sAdjustChannel] = pits = new cAdjustIterators();

    Corr = m_AdjustTargetValue * m_AdjustCorrection / m_AdjustActualValue ; // we simlpy correct the actualvalue before calculating corr
    m_MsgNrCmdList[m_AdjustPCBInterface->setGainNode(m_sAdjustSysName, m_sAdjustRange, pits->m_nAdjustGainIt, Corr, m_AdjustTargetValue)] = setgainnode;
    pits->m_nAdjustGainIt++;
}


void cAdjustmentModuleMeasProgram::setAdjustPhaseStartCommand(QVariant var)
{
    setAdjustEnvironment(var);
    m_AdjustEntity = getConfData()->m_AdjChannelInfoHash[m_sAdjustSysName]->phaseAdjInfo.m_nEntity;
    m_AdjustComponent = getConfData()->m_AdjChannelInfoHash[m_sAdjustSysName]->phaseAdjInfo.m_sComponent;
    m_adjustPhaseMachine.start();
}


void cAdjustmentModuleMeasProgram::adjustphaseGetCorr()
{
    m_AdjustActualValue = cmpPhase(m_pModule->m_pStorageSystem->getStoredValue(m_AdjustEntity, m_AdjustComponent));
    m_AdjustFrequency = m_pModule->m_pStorageSystem->getStoredValue(getConfData()->m_ReferenceFrequency.m_nEntity, getConfData()->m_ReferenceFrequency.m_sComponent).toDouble();
    m_MsgNrCmdList[m_AdjustPCBInterface->getAdjPhaseCorrection(m_sAdjustSysName, m_sAdjustRange, m_AdjustFrequency)] = getadjphasecorrection;
}


void cAdjustmentModuleMeasProgram::adjustphaseSetNode()
{
    cAdjustIterators *pits;
    double Corr;

    if (m_adjustIteratorHash.contains(m_sAdjustChannel))
        pits = m_adjustIteratorHash[m_sAdjustChannel];
    else
        m_adjustIteratorHash[m_sAdjustChannel] = pits = new cAdjustIterators();

    Corr = symAngle((m_AdjustActualValue + m_AdjustCorrection) - m_AdjustTargetValue); // we simlpy correct the actualvalue before calculating corr
    m_MsgNrCmdList[m_AdjustPCBInterface->setPhaseNode(m_sAdjustSysName, m_sAdjustRange, pits->m_nAdjustPhaseIt, Corr, m_AdjustFrequency)] = setphasenode;
    pits->m_nAdjustPhaseIt++;
}


void cAdjustmentModuleMeasProgram::setAdjustOffsetStartCommand(QVariant var)
{
    setAdjustEnvironment(var);
    m_AdjustEntity = getConfData()->m_AdjChannelInfoHash[m_sAdjustSysName]->offsetAdjInfo.m_nEntity;
    m_AdjustComponent = getConfData()->m_AdjChannelInfoHash[m_sAdjustSysName]->offsetAdjInfo.m_sComponent;
    m_adjustOffsetMachine.start();
}


void cAdjustmentModuleMeasProgram::transparentDataSend2Port(QVariant var)
{
    QList<QString> sl;
    int port;

    sl = var.toString().split(',');
    if (sl.count() == 2) // we expect a port number and a command
    {
        port = sl.at(0).toInt();
        if (m_portChannelHash.contains(port))
        {
            m_MsgNrCmdList[m_adjustChannelInfoHash[m_portChannelHash[port]]->m_pPCBInterface->transparentCommand(sl.at(1))] = sendtransparentcmd;
            return;
        }
    }

    m_pPARAdjustSend->setError();
}


void cAdjustmentModuleMeasProgram::writePCBAdjustmentData(QVariant var)
{
    receivedPar = var;
    m_MsgNrCmdList[m_AdjustPCBInterface->setPCBAdjustmentData(var.toString())] = setpcbadjustmentdata;
}


void cAdjustmentModuleMeasProgram::readPCBAdjustmentData(QVariant)
{
    m_MsgNrCmdList[m_AdjustPCBInterface->getPCBAdjustmentData()] = getpcbadjustmentdata;
}


void cAdjustmentModuleMeasProgram::writeCLAMPAdjustmentData(QVariant var)
{
    receivedPar = var;
    m_MsgNrCmdList[m_AdjustPCBInterface->setClampAdjustmentData(var.toString())] = setclampadjustmentdata;
}


void cAdjustmentModuleMeasProgram::readCLAMPAdjustmentData(QVariant)
{
    m_MsgNrCmdList[m_AdjustPCBInterface->getClampAdjustmentData()] = getclampadjustmentdata;
}


void cAdjustmentModuleMeasProgram::fetchAuthorizationStatus()
{
    QList<QString> sysnameList;
    sysnameList = m_AliasChannelHash.values();
    m_AdjustPCBInterface = m_adjustChannelInfoHash[sysnameList.at(0)]->m_pPCBInterface;

    m_MsgNrCmdList[m_AdjustPCBInterface->getAuthorizationStatus()] = getauthorizationstatus;
}


void cAdjustmentModuleMeasProgram::catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer)
{
    if (msgnr == 0) // 0 was reserved for async. messages
    {
        // that we will ignore
    }
    else
    {
        // because rangemodulemeasprogram, adjustment and rangeobsermatic share the same dsp interface
        if (m_MsgNrCmdList.contains(msgnr))
        {
            int cmd = m_MsgNrCmdList.take(msgnr);
            switch (cmd)
            {
            case sendrmident:
                if (reply == ack) // we only continue if resource manager acknowledges
                    emit activationContinue();
                else
                {
                    emit errMsg(tr(rmidentErrMSG));
#ifdef DEBUG
                    qDebug() << rmidentErrMSG;
#endif
                    emit activationError();
                }
                break;

            case readresourcetypes:
                if ((reply == ack) && (answer.toString().contains("SENSE")))
                    emit activationContinue();
                else
                {
                    emit errMsg((tr(resourcetypeErrMsg)));
        #ifdef DEBUG
                    qDebug() << resourcetypeErrMsg;
        #endif
                    emit activationError();
                }
                break;

            case readresource:
            {
                bool allPresent;

                allPresent = false;
                if (reply == ack)
                {
                    int n;
                    QString s;

                    s = answer.toString();
                    allPresent = true;
                    n = getConfData()->m_nAdjustmentChannelCount;
                    for (int i = 0; i < n; i++)
                        allPresent = allPresent && s.contains(getConfData()->m_AdjChannelList.at(i));
                }

                if (allPresent)
                    emit activationContinue();
                else
                {
                    emit errMsg((tr(resourceErrMsg)));
#ifdef DEBUG
                    qDebug() << resourceErrMsg;
#endif
                    emit activationError();
                }
                break;
            }

            case readresourceinfo:
            {
                bool ok;
                int port;
                QStringList sl;

                sl = answer.toString().split(';');
                if ((reply ==ack) && (sl.length() >= 4))
                {
                    port = sl.at(3).toInt(&ok);

                    if (ok)
                    {
                        m_chnPortHash[getConfData()->m_AdjChannelList.at(activationIt)] = port;
                        emit activationContinue();
                    }

                    else
                    {
                        emit errMsg((tr(resourceInfoErrMsg)));
#ifdef DEBUG
                        qDebug() << resourceInfoErrMsg;
#endif
                        emit activationError();
                    }
                }

                else
                {
                    emit errMsg((tr(resourceInfoErrMsg)));
#ifdef DEBUG
                    qDebug() << resourceInfoErrMsg;
#endif
                    emit activationError();
                }

                break;

            }

            case readchnalias:
            {
                if (reply == ack)
                {
                    QString alias, sysName;
                    alias = answer.toString();
                    sysName = getConfData()->m_AdjChannelList.at(activationIt);
                    m_AliasChannelHash[alias] = sysName;
                    m_adjustChannelInfoHash[sysName]->m_sAlias = alias;
                    emit activationContinue();
                }
                else
                {
                    emit errMsg((tr(readaliasErrMsg)));
        #ifdef DEBUG
                    qDebug() << readaliasErrMsg;
        #endif
                    emit activationError();
                }
                break;
            }

            case readrangelist:
                if (reply == ack)
                {
                    m_adjustChannelInfoHash[getConfData()->m_AdjChannelList.at(activationIt)]->m_sRangelist = answer.toStringList();
                    emit activationContinue();
                }
                else
                {
                    emit errMsg((tr(readrangelistErrMsg)));
        #ifdef DEBUG
                    qDebug() << readrangelistErrMsg;
        #endif
                    emit activationError();
                }
                break;

            case adjustcomputation:
                if (reply == ack)
                {
                    emit computationContinue();
                }
                else
                {
                    m_computationMachine.stop();
                    emit errMsg((tr(adjustcomputationPCBErrMSG)));
#ifdef DEBUG
                    qDebug() << adjustcomputationPCBErrMSG;
#endif
                    emit executionError();
                }
                break;

            case adjuststorage:
                if (reply == ack)
                {
                    emit storageContinue();
                }
                else
                {
                    m_storageMachine.stop();
                    emit errMsg((tr(adjuststoragePCBErrMSG)));
#ifdef DEBUG
                    qDebug() << adjuststoragePCBErrMSG;
#endif
                    emit executionError();
                    m_pPARStorage->setError();
                }
                break;

            case setadjustgainstatus:
                if (reply == ack)
                {
                    m_pPARAdjustGainStatus->setValue(QVariant(0));
                }
                else
                {
                    emit errMsg((tr(adjuststatusPCBErrMSG)));
#ifdef DEBUG
                    qDebug() << adjuststatusPCBErrMSG;
#endif
                    emit executionError();
                    m_pPARAdjustGainStatus->setError();
                }
                break;

            case setadjustphasestatus:
                if (reply == ack)
                {
                    m_pPARAdjustPhaseStatus->setValue(QVariant(0));
                }
                else
                {
                    emit errMsg((tr(adjuststatusPCBErrMSG)));
#ifdef DEBUG
                    qDebug() << adjuststatusPCBErrMSG;
#endif
                    emit executionError();
                    m_pPARAdjustPhaseStatus->setError();
                }
                break;


            case setadjustoffsetstatus:
                if (reply == ack)
                {
                    m_pPARAdjustOffsetStatus->setValue(QVariant(0));
                }
                else
                {
                    emit errMsg((tr(adjuststatusPCBErrMSG)));
#ifdef DEBUG
                    qDebug() << adjuststatusPCBErrMSG;
#endif
                    emit executionError();
                    m_pPARAdjustOffsetStatus->setError();
                }
                break;


            case getadjgaincorrection:
                if (reply == ack)
                {
                    m_AdjustCorrection = answer.toDouble();
                    emit adjustamplitudeContinue();
                }
                else
                {
                    emit errMsg(readGainCorrErrMsg);
                    emit adjustError();
#ifdef DEBUG
                    qDebug() << readGainCorrErrMsg;
#endif
                    emit executionError();
                    m_pPARAdjustAmplitude->setError();
                }
                break;

            case setgainnode:
                if (reply == ack)
                {
                    emit adjustamplitudeContinue();
                    m_pPARAdjustAmplitude->setValue(receivedPar);
                }
                else
                {
                    emit errMsg(setGainNodeErrMsg);
                    emit adjustError();
#ifdef DEBUG
                    qDebug() << setGainNodeErrMsg;
#endif
                    emit executionError();
                    m_pPARAdjustAmplitude->setError();
                }
                break;

            case enGetAdjOffsetCorrection:
                if (reply == ack)
                {
                    m_AdjustCorrection = answer.toDouble();
                    emit adjustoffsetContinue();
                }
                else
                {
                    emit errMsg(readOffsetCorrErrMsg);
                    emit adjustError();
#ifdef DEBUG
                    qDebug() << readOffsetCorrErrMsg;
#endif
                    emit executionError();
                    m_pPARAdjustOffset->setError();
                }
                break;

            case enGetAdjOffsetRejection:
                if (reply == ack)
                {
                    m_AdjustRejection = answer.toDouble();
                    emit adjustoffsetContinue();
                }
                else
                {
                    emit errMsg(readrangerejectionErrMsg);
                    emit adjustError();
#ifdef DEBUG
                    qDebug() << readrangerejectionErrMsg;
#endif
                    emit executionError();
                    m_pPARAdjustOffset->setError();
                }
                break;

            case enGetAdjOffsetRejectionValue:
                if (reply == ack)
                {
                    m_AdjustRejectionValue = answer.toDouble();
                    emit adjustoffsetContinue();
                }
                else
                {
                    emit errMsg(readrangeurvalueErrMsg);
                    emit adjustError();
#ifdef DEBUG
                    qDebug() << readrangeurvalueErrMsg;
#endif
                    emit executionError();
                    m_pPARAdjustOffset->setError();
                }
                break;

            case setoffsetnode:
                if (reply == ack)
                {
                    m_pPARAdjustOffset->setValue(receivedPar);
                    emit adjustoffsetContinue();
                }
                else
                {
                    emit errMsg(setOffsetNodeErrMsg);
                    emit adjustError();
#ifdef DEBUG
                    qDebug() << setOffsetNodeErrMsg;
#endif
                    emit executionError();
                    m_pPARAdjustOffset->setError();
                }
                break;

            case getadjphasecorrection:
                if (reply == ack)
                {
                    m_AdjustCorrection = answer.toDouble();
                    emit adjustphaseContinue();
                }
                else
                {
                    emit errMsg(readPhaseCorrErrMsg);
                    emit adjustError();
#ifdef DEBUG
                    qDebug() << readPhaseCorrErrMsg;
#endif
                    emit executionError();
                    m_pPARAdjustPhase->setError();
                }
                break;

            case setphasenode:
                if (reply == ack)
                {
                    m_pPARAdjustPhase->setValue(receivedPar);
                    emit adjustphaseContinue();
                }
                else
                {
                    emit errMsg(setPhaseNodeErrMsg);
                    emit adjustError();
#ifdef DEBUG
                    qDebug() << setPhaseNodeErrMsg;
#endif
                    emit executionError();
                    m_pPARAdjustPhase->setError();
                }
                break;

            case getauthorizationstatus:
                if (reply == ack)
                    m_bAuthorized = (answer.toInt() > 0);
                else
                    emit errMsg(readauthorizationErrMSG);
                break;

            case sendtransparentcmd:
                // if (reply == ack)
                // in any case we return the answer to client
                m_pPARAdjustSend->setValue(answer);
                break;

            case getpcbadjustmentdata:
                if (reply == ack)
                    m_pPARAdjustPCBData->setValue(answer);
                else
                {
                    m_pPARAdjustPCBData->setError();
                    emit errMsg(readPCBXMLMSG);
                }
                break;


            case setpcbadjustmentdata:
                if (reply == ack)
                    m_pPARAdjustPCBData->setValue(receivedPar);
                else
                {
                    m_pPARAdjustPCBData->setError();
                    emit errMsg(writePCBXMLMSG);
                }
                break;

            case getclampadjustmentdata:
                if (reply == ack)
                    m_pPARAdjustClampData->setValue(answer);
                else
                {
                    m_pPARAdjustClampData->setError();
                    emit errMsg(readClampXMLMSG);
                }
                break;

            case setclampadjustmentdata:
                if (reply == ack)
                    m_pPARAdjustClampData->setValue(receivedPar);
                else
                {
                    m_pPARAdjustClampData->setError();
                    emit errMsg(writeClampXMLMSG);
                }
                break;
            }
        }
    }
}

cAdjustIterators::cAdjustIterators()
{
    m_nAdjustGainIt = m_nAdjustOffsetIt = m_nAdjustPhaseIt = 0;
}

}



