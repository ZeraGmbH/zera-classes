#include <QString>
#include <QStateMachine>
#include <QState>
#include <QJsonObject>
#include <QJsonArray>
#include <QStringList>
#include <proxy.h>
#include <proxyclient.h>
#include <pcbinterface.h>
#include <rminterface.h>

#include <ve_storagesystem.h>
#include <modulevalidator.h>
#include <intvalidator.h>
#include <veinmodulecomponentinput.h>
#include <veinmoduleactvalue.h>
#include <veinmoduleparameter.h>
#include <scpiinfo.h>
#include <useratan.h>

#include "debug.h"
#include "errormessages.h"
#include "reply.h"
#include "measmodeinfo.h"
#include "adjustmentmodule.h"
#include "adjustmentmodulemeasprogram.h"
#include "adjustvalidator.h"


namespace ADJUSTMENTMODULE
{

cAdjustmentModuleMeasProgram::cAdjustmentModuleMeasProgram(cAdjustmentModule* module, Zera::Proxy::cProxy* proxy, cAdjustmentModuleConfigData& configdata)
    :m_pModule(module), m_pProxy(proxy), m_ConfigData(configdata)
{
    // setting up statemachine for "activating" adjustment
    // m_rmConnectState.addTransition is done in rmConnect
    m_IdentifyState.addTransition(this, SIGNAL(activationContinue()), &m_readResourceTypesState);
    m_readResourceTypesState.addTransition(this, SIGNAL(activationContinue()), &m_readResourceState);
    m_readResourceState.addTransition(this, SIGNAL(activationContinue()), &m_readResourceInfoState);
    m_readResourceInfoState.addTransition(this, SIGNAL(activationContinue()), &m_readResourceInfoLoopState);
    m_readResourceInfoLoopState.addTransition(this, SIGNAL(activationContinue()), &m_pcbConnectionState);
    m_readResourceInfoLoopState.addTransition(this, SIGNAL(activationLoop()), &m_readResourceInfoState);
    m_pcbConnectionState.addTransition(this, SIGNAL(activationContinue()), &m_pcbConnectionLoopState);
    m_pcbConnectionLoopState.addTransition(this, SIGNAL(activationContinue()), &m_readChnAliasState);
    m_pcbConnectionLoopState.addTransition(this, SIGNAL(activationLoop()), &m_pcbConnectionState);
    m_readChnAliasState.addTransition(this, SIGNAL(activationContinue()), &m_readChnAliasLoopState);
    m_readChnAliasLoopState.addTransition(this, SIGNAL(activationContinue()), &m_readRangelistState);
    m_readChnAliasLoopState.addTransition(this, SIGNAL(activationLoop()), &m_readChnAliasState);
    m_readRangelistState.addTransition(this, SIGNAL(activationContinue()), &m_readRangelistLoopState);
    m_readRangelistLoopState.addTransition(this, SIGNAL(activationContinue()), &m_searchActualValuesState);
    m_readRangelistLoopState.addTransition(this, SIGNAL(activationLoop()), &m_readRangelistState);
    m_searchActualValuesState.addTransition(this, SIGNAL(activationContinue()), &m_activationDoneState);

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

    connect(&m_rmConnectState, SIGNAL(entered()), SLOT(rmConnect()));
    connect(&m_IdentifyState, SIGNAL(entered()), SLOT(sendRMIdent()));
    connect(&m_readResourceTypesState, SIGNAL(entered()), SLOT(readResourceTypes()));
    connect(&m_readResourceState, SIGNAL(entered()), SLOT(readResource()));
    connect(&m_readResourceInfoState, SIGNAL(entered()), SLOT(readResourceInfo()));
    connect(&m_readResourceInfoLoopState, SIGNAL(entered()), SLOT(readResourceInfoLoop()));
    connect(&m_pcbConnectionState, SIGNAL(entered()), SLOT(pcbConnection()));
    connect(&m_pcbConnectionLoopState, SIGNAL(entered()), SLOT(pcbConnectionLoop()));
    connect(&m_readChnAliasState, SIGNAL(entered()), SLOT(readChnAlias()));
    connect(&m_readRangelistLoopState, SIGNAL(entered()), SLOT(readChnAliasLoop()));
    connect(&m_readRangelistState, SIGNAL(entered()), SLOT(readRangelist()));
    connect(&m_readRangelistLoopState, SIGNAL(entered()), SLOT(readRangelistLoop()));
    connect(&m_searchActualValuesState, SIGNAL(entered()), SLOT(searchActualValues()));
    connect(&m_activationDoneState, SIGNAL(entered()), SLOT(activationDone()));

    // setting up statemachine deactivation
    m_deactivateState.addTransition(this, SIGNAL(deactivationContinue()), &m_deactivateDoneState);

    m_deactivationMachine.addState(&m_deactivateState);
    m_deactivationMachine.addState(&m_deactivateDoneState);

    m_deactivationMachine.setInitialState(&m_deactivateState);

    connect(&m_deactivateState, SIGNAL(entered()), SLOT(deactivateMeas()));
    connect(&m_deactivateDoneState, SIGNAL(entered()), SLOT(deactivateMeasDone()));

    m_computationStartState.addTransition(this, SIGNAL(computationContinue()), &m_computationTestState);
    m_computationTestState.addTransition(this, SIGNAL(computationContinue()), &m_computationStartState);
    m_computationTestState.addTransition(this, SIGNAL(computationDone()), &m_computationFinishState);
    m_computationMachine.addState(&m_computationStartState);
    m_computationMachine.addState(&m_computationTestState);
    m_computationMachine.addState(&m_computationFinishState);
    m_computationMachine.setInitialState(&m_computationStartState);

    connect(&m_computationStartState, SIGNAL(entered()), SLOT(computationStart()));
    connect(&m_computationTestState, SIGNAL(entered()), SLOT(computationTest()));
    connect(&m_computationFinishState, SIGNAL(entered()), SLOT(computationFinished()));

    m_storageStartState.addTransition(this, SIGNAL(computationContinue()), &m_storageTestState);
    m_storageTestState.addTransition(this, SIGNAL(computationContinue()), &m_storageStartState);
    m_storageTestState.addTransition(this, SIGNAL(computationDone()), &m_storageFinishState);
    m_storageMachine.addState(&m_storageStartState);
    m_storageMachine.addState(&m_storageTestState);
    m_storageMachine.addState(&m_storageFinishState);
    m_storageMachine.setInitialState(&m_storageStartState);

    connect(&m_storageStartState, SIGNAL(entered()), SLOT(storageStart()));
    connect(&m_storageTestState, SIGNAL(entered()), SLOT(storageTest()));
    connect(&m_storageFinishState, SIGNAL(entered()), SLOT(storageFinished()));

    m_adjustamplitudeGetCorrState.addTransition(this, SIGNAL(adjustamplitudeContinue()), &m_adjustamplitudeSetNodeState);
    m_adjustamplitudeGetCorrState.addTransition(this, SIGNAL(adjustError()), &m_adjustamplitudeFinishState);
    m_adjustamplitudeSetNodeState.addTransition(this, SIGNAL(adjustamplitudeContinue()), &m_adjustamplitudeFinishState);
    m_adjustamplitudeSetNodeState.addTransition(this, SIGNAL(adjustError()), &m_adjustamplitudeFinishState);
    m_adjustAmplitudeMachine.addState(&m_adjustamplitudeGetCorrState);
    m_adjustAmplitudeMachine.addState(&m_adjustamplitudeSetNodeState);
    m_adjustAmplitudeMachine.addState(&m_adjustamplitudeFinishState);
    m_adjustAmplitudeMachine.setInitialState(&m_adjustamplitudeGetCorrState);

    connect(&m_adjustamplitudeGetCorrState, SIGNAL(entered()), SLOT(adjustamplitudeGetCorr()));
    connect(&m_adjustamplitudeSetNodeState, SIGNAL(entered()), SLOT(adjustamplitudeSetNode()));

    m_adjustoffsetGetCorrState.addTransition(this, SIGNAL(adjustoffsetContinue()), &m_adjustoffsetSetNodeState);
    m_adjustoffsetGetCorrState.addTransition(this, SIGNAL(adjustError()), &m_adjustoffsetFinishState);
    m_adjustoffsetSetNodeState.addTransition(this, SIGNAL(adjustoffsetContinue()), &m_adjustoffsetFinishState);
    m_adjustoffsetSetNodeState.addTransition(this, SIGNAL(adjustError()), &m_adjustoffsetFinishState);
    m_adjustOffsetMachine.addState(&m_adjustoffsetGetCorrState);
    m_adjustOffsetMachine.addState(&m_adjustoffsetSetNodeState);
    m_adjustOffsetMachine.addState(&m_adjustoffsetFinishState);
    m_adjustOffsetMachine.setInitialState(&m_adjustoffsetGetCorrState);

    connect(&m_adjustoffsetGetCorrState, SIGNAL(entered()), SLOT(adjustoffsetGetCorr()));
    connect(&m_adjustoffsetSetNodeState, SIGNAL(entered()), SLOT(adjustoffsetSetNode()));

    m_adjustphaseGetCorrState.addTransition(this, SIGNAL(adjustphaseContinue()), &m_adjustphaseSetNodeState);
    m_adjustphaseGetCorrState.addTransition(this, SIGNAL(adjustError()), &m_adjustphaseFinishState);
    m_adjustphaseSetNodeState.addTransition(this, SIGNAL(adjustphaseContinue()), &m_adjustphaseFinishState);
    m_adjustphaseSetNodeState.addTransition(this, SIGNAL(adjustError()), &m_adjustphaseFinishState);
    m_adjustPhaseMachine.addState(&m_adjustphaseGetCorrState);
    m_adjustPhaseMachine.addState(&m_adjustphaseSetNodeState);
    m_adjustPhaseMachine.addState(&m_adjustphaseFinishState);
    m_adjustPhaseMachine.setInitialState(&m_adjustphaseGetCorrState);

    connect(&m_adjustphaseGetCorrState, SIGNAL(entered()), SLOT(adjustphaseGetCorr()));
    connect(&m_adjustphaseSetNodeState, SIGNAL(entered()), SLOT(adjustphaseSetNode()));
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


void cAdjustmentModuleMeasProgram::setAdjustEnvironment(QVariant var)
{
    QStringList sl;

    sl = var.toString().split(';');
    m_sAdjustChannel = sl.at(0);
    m_sAdjustRange = sl.at(1);
    m_AdjustTargetValue = sl.at(2).toDouble();
    m_sAdjustSysName = m_AliasChannelHash[m_sAdjustChannel];
    m_AdjustPCBInterface = m_adjustChannelInfoHash[m_sAdjustSysName]->m_pPCBInterface;

    m_AdjustEntity = m_ConfigData.m_AdjChannelInfoHash[m_sAdjustSysName]->amplitudeAdjInfo.m_nEntity;
    m_AdjustComponent = m_ConfigData.m_AdjChannelInfoHash[m_sAdjustSysName]->amplitudeAdjInfo.m_sComponent;
}


double cAdjustmentModuleMeasProgram::cmpPhase(QVariant var)
{
    QStringList sl;
    double phi;

    sl = var.toString().split(';');
    phi = userAtan(sl.at(0).toDouble(), sl.at(1).toDouble());
    return phi;
}


void cAdjustmentModuleMeasProgram::setInterfaceValidation()
{
    // we must set the validators for the adjustment commands now
    // we know the channel names and their ranges now
    // ....very special validator

    cAdjustValidator3d* adjValidatord;
    cAdjustValidator3i* adjValidatori;
    cAdjustChannelInfo* adjChnInfo;

    // first the validator for amplitude adjustment
    cDoubleValidator dValidator = cDoubleValidator(0, 2000,1e-7);
    adjValidatord = new cAdjustValidator3d();
    for (int i = 0; i < m_ConfigData.m_nAdjustmentChannelCount; i++)
    {
        adjChnInfo = m_adjustChannelInfoHash[m_ConfigData.m_AdjChannelList.at(i)];
        adjValidatord->addValidator(adjChnInfo->m_sAlias, adjChnInfo->m_sRangelist, dValidator);
    }
    m_pPARAdjustAmplitude->setValidator(adjValidatord);

    // validator for offset adjustment
    adjValidatord = new cAdjustValidator3d();
    for (int i = 0; i < m_ConfigData.m_nAdjustmentChannelCount; i++)
    {
        adjChnInfo = m_adjustChannelInfoHash[m_ConfigData.m_AdjChannelList.at(i)];
        adjValidatord->addValidator(adjChnInfo->m_sAlias, adjChnInfo->m_sRangelist, dValidator);
    }
    m_pPARAdjustOffset->setValidator(adjValidatord);

    // validator for angle adjustment
    dValidator = cDoubleValidator(0, 360,1e-7);
    adjValidatord = new cAdjustValidator3d();
    for (int i = 0; i < m_ConfigData.m_nAdjustmentChannelCount; i++)
    {
        adjChnInfo = m_adjustChannelInfoHash[m_ConfigData.m_AdjChannelList.at(i)];
        adjValidatord->addValidator(adjChnInfo->m_sAlias, adjChnInfo->m_sRangelist, dValidator);
    }
    m_pPARAdjustPhase->setValidator(adjValidatord);

    // validator for adjustment status setting
    cIntValidator iValidator = cIntValidator(0,255);

    adjValidatori = new cAdjustValidator3i();
    for (int i = 0; i < m_ConfigData.m_nAdjustmentChannelCount; i++)
    {
        adjChnInfo = m_adjustChannelInfoHash[m_ConfigData.m_AdjChannelList.at(i)];
        adjValidatori->addValidator(adjChnInfo->m_sAlias, adjChnInfo->m_sRangelist, iValidator);
    }
    m_pPARAdjustStatus->setValidator(adjValidatori);

    cAdjustValidator2* adjInitValidator;

    adjInitValidator = new cAdjustValidator2();
    for (int i = 0; i < m_ConfigData.m_nAdjustmentChannelCount; i++)
    {
        adjChnInfo = m_adjustChannelInfoHash[m_ConfigData.m_AdjChannelList.at(i)];
        adjInitValidator->addValidator(adjChnInfo->m_sAlias, adjChnInfo->m_sRangelist);
    }
    m_pPARAdjustInit->setValidator(adjInitValidator);
}


void cAdjustmentModuleMeasProgram::generateInterface()
{
    QString key;
    cIntValidator* iValidator;
    cSCPIInfo* scpiInfo;

    m_pPARComputation = new cVeinModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                                 key = QString("PAR_Computation"),
                                                 QString("Component for command to start computation of adjustment coefficients"),
                                                 QVariant(int(0)),
                                                 false); // no deferred notification necessary

    m_pModule->veinModuleParameterHash[key] = m_pPARComputation;
    scpiInfo = new cSCPIInfo("CALCULATE", "COMPUTATION", "10", m_pPARComputation->getName(), "0", "");
    m_pPARComputation->setSCPIInfo(scpiInfo);
    iValidator = new cIntValidator(0,1);
    m_pPARComputation->setValidator(iValidator);
    connect(m_pPARComputation, SIGNAL(sigValueChanged(QVariant)), SLOT(computationStartCommand(QVariant)));


    m_pPARStorage = new cVeinModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                             key = QString("PAR_Storage"),
                                             QString("Component for command to start saving of adjustment data"),
                                             QVariant(int(0)),
                                             false); // no deferred notification necessary

    m_pModule->veinModuleParameterHash[key] = m_pPARStorage;
    scpiInfo = new cSCPIInfo("CALCULATE", "STORAGE", "10", m_pPARStorage->getName(), "0", "");
    m_pPARStorage->setSCPIInfo(scpiInfo);
    iValidator = new cIntValidator(0,1);
    m_pPARStorage->setValidator(iValidator);
    connect(m_pPARStorage, SIGNAL(sigValueChanged(QVariant)), SLOT(storageStartCommand(QVariant)));


    m_pPARAdjustStatus = new cVeinModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                                  key = QString("PAR_AdjustStatus"),
                                                  QString("Component for command to start saving of adjustment data"),
                                                  QVariant(int(0)),
                                                  false); // no deferred notification necessary

    m_pModule->veinModuleParameterHash[key] = m_pPARAdjustStatus;
    scpiInfo = new cSCPIInfo("CALCULATE", "STATUS", "10", m_pPARAdjustStatus->getName(), "0", "");
    m_pPARAdjustStatus->setSCPIInfo(scpiInfo);
    connect(m_pPARAdjustStatus, SIGNAL(sigValueChanged(QVariant)), SLOT(setAdjustStatusStartCommand(QVariant)));


    m_pPARAdjustInit = new cVeinModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                                key = QString("PAR_AdjustInit"),
                                                QString("Component for command to start initialiazing of adjustment"),
                                                QVariant(int(0)),
                                                false); // no deferred notification necessary

    m_pModule->veinModuleParameterHash[key] = m_pPARAdjustInit;
    scpiInfo = new cSCPIInfo("CALCULATE", "INIT", "10", m_pPARAdjustInit->getName(), "0", "");
    m_pPARAdjustInit->setSCPIInfo(scpiInfo);
    connect(m_pPARAdjustInit, SIGNAL(sigValueChanged(QVariant)), SLOT(setAdjustInitStartCommand(QVariant)));


    m_pPARAdjustAmplitude = new cVeinModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                                     key = QString("PAR_AdjustAmplitude"),
                                                     QString("Component for command to set 1 amplitude adjustment node"),
                                                     QVariant(QString("")),
                                                     false); // no deferred notification necessary

    m_pModule->veinModuleParameterHash[key] = m_pPARAdjustAmplitude;
    scpiInfo = new cSCPIInfo("CALCULATE", "AMPLITUDE", "10", m_pPARAdjustAmplitude->getName(), "0", "");
    m_pPARAdjustAmplitude->setSCPIInfo(scpiInfo);
    // we will set the validator later after activation we will know the channel names and their ranges
    connect(m_pPARAdjustAmplitude, SIGNAL(sigValueChanged(QVariant)), SLOT(setAdjustAmplitudeStartCommand(QVariant)));


    m_pPARAdjustPhase = new cVeinModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                                 key = QString("PAR_AdjustPhase"),
                                                 QString("Component for command to set 1 phase adjustment node"),
                                                 QVariant(QString("")),
                                                 false); // no deferred notification necessary

    m_pModule->veinModuleParameterHash[key] = m_pPARAdjustPhase;
    scpiInfo = new cSCPIInfo("CALCULATE", "PHASE", "10", m_pPARAdjustPhase->getName(), "0", "");
    m_pPARAdjustPhase->setSCPIInfo(scpiInfo);
    // we will set the validator later after activation we will know the channel names and their ranges
    connect(m_pPARAdjustPhase, SIGNAL(sigValueChanged(QVariant)), SLOT(setAdjustPhaseStartCommand(QVariant)));


    m_pPARAdjustOffset = new cVeinModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                                  key = QString("PAR_Adjustoffset"),
                                                  QString("Component for command to set 1 offset adjustment node"),
                                                  QVariant(QString("")),
                                                  false); // no deferred notification necessary

    m_pModule->veinModuleParameterHash[key] = m_pPARAdjustOffset;
    // we will set the validator later after activation we will know the channel names and their ranges
    scpiInfo = new cSCPIInfo("CALCULATE", "OFFSET", "10", m_pPARAdjustOffset->getName(), "0", "");
    m_pPARAdjustOffset->setSCPIInfo(scpiInfo);
    connect(m_pPARAdjustOffset, SIGNAL(sigValueChanged(QVariant)), SLOT(setAdjustOffsetStartCommand(QVariant)));
}


void cAdjustmentModuleMeasProgram::deleteInterface()
{
}


void cAdjustmentModuleMeasProgram::rmConnect()
{
    // we instantiate a working resource manager interface first
    // so first we try to get a connection to resource manager over proxy
    m_pRMClient = m_pProxy->getConnection(m_ConfigData.m_RMSocket.m_sIP, m_ConfigData.m_RMSocket.m_nPort);
    m_rmConnectState.addTransition(m_pRMClient, SIGNAL(connected()), &m_IdentifyState);
    // and then we set connection resource manager interface's connection
    m_pRMInterface->setClient(m_pRMClient); //
    // todo insert timer for timeout

    connect(m_pRMInterface, SIGNAL(serverAnswer(quint32, quint8, QVariant)), this, SLOT(catchInterfaceAnswer(quint32, quint8, QVariant)));
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
    m_MsgNrCmdList[m_pRMInterface->getResourceInfo("SENSE", m_ConfigData.m_AdjChannelList.at(activationIt))] = readresourceinfo;
}


void cAdjustmentModuleMeasProgram::readResourceInfoLoop()
{
    activationIt = (activationIt + 1) % m_ConfigData.m_nAdjustmentChannelCount;
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
    Zera::Server::cPCBInterface *pcbInterface;
    cAdjustChannelInfo* adjustChannelInfo;

    sChannel = m_ConfigData.m_AdjChannelList.at(activationIt); // the system channel name we work on
    adjustChannelInfo = new cAdjustChannelInfo();
    m_adjustChannelInfoHash[sChannel] = adjustChannelInfo;

    port = m_chnPortHash[m_ConfigData.m_AdjChannelList.at(activationIt)]; // the port
    if (m_portChannelHash.contains(port))
    {
        // the channels share the same interface
        adjustChannelInfo->m_pPCBInterface = m_adjustChannelInfoHash[m_portChannelHash[port] ]->m_pPCBInterface;
        emit activationContinue();
    }
    else
    {
        m_portChannelHash[port] = sChannel;
        pcbclient = m_pProxy->getConnection(m_ConfigData.m_PCBSocket.m_sIP, port);
        m_pcbClientList.append(pcbclient);
        m_pcbConnectionState.addTransition(pcbclient, SIGNAL(connected()), &m_pcbConnectionLoopState);
        pcbInterface = new Zera::Server::cPCBInterface();
        m_pcbInterfaceList.append(pcbInterface);
        pcbInterface->setClient(pcbclient);
        connect(pcbInterface, SIGNAL(serverAnswer(quint32, quint8, QVariant)), this, SLOT(catchInterfaceAnswer(quint32, quint8, QVariant)));
        adjustChannelInfo->m_pPCBInterface = pcbInterface;
        m_pProxy->startConnection((pcbclient));
    }
}


void cAdjustmentModuleMeasProgram::pcbConnectionLoop()
{
    activationIt = (activationIt + 1) % m_ConfigData.m_nAdjustmentChannelCount;
    if (activationIt == 0)
        emit activationContinue();
    else
        emit activationLoop();
}


void cAdjustmentModuleMeasProgram::readChnAlias()
{
    QString name;

    name = m_ConfigData.m_AdjChannelList.at(activationIt);
    m_MsgNrCmdList[m_adjustChannelInfoHash[name]->m_pPCBInterface->getAlias(name)] = readchnalias;
}


void cAdjustmentModuleMeasProgram::readChnAliasLoop()
{
    activationIt = (activationIt + 1) % m_ConfigData.m_nAdjustmentChannelCount;
    if (activationIt == 0)
        emit activationContinue();
    else
        emit activationLoop();
}


void cAdjustmentModuleMeasProgram::readRangelist()
{
    QString name;

    name = m_ConfigData.m_AdjChannelList.at(activationIt);
    m_MsgNrCmdList[m_adjustChannelInfoHash[name]->m_pPCBInterface->getRangeList(name)] = readrangelist;
}


void cAdjustmentModuleMeasProgram::readRangelistLoop()
{
    activationIt = (activationIt + 1) % m_ConfigData.m_nAdjustmentChannelCount;
    if (activationIt == 0)
        emit activationContinue();
    else
        emit activationLoop();
}


void cAdjustmentModuleMeasProgram::searchActualValues()
{
    bool error;

    error = false;

    for (int i = 0; i < m_ConfigData.m_nAdjustmentChannelCount; i++)
    {
        // we test if all configured actual value data exist
        QString chn;
        adjInfoType* adjInfo;

        chn = m_ConfigData.m_AdjChannelList.at(i);
        adjInfo = &(m_ConfigData.m_AdjChannelInfoHash[chn]->amplitudeAdjInfo);

        if (adjInfo->m_bAvail)
            if (!m_pModule->m_pStorageSystem->hasStoredValue(adjInfo->m_nEntity, adjInfo->m_sComponent))
            {
                error = true;
                break;
            }
    }

    if (error)
        emit activationError();

    else
        emit activationContinue();
}


void cAdjustmentModuleMeasProgram::activationDone()
{
    m_bActive = true;
    setInterfaceValidation();
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
    // wir nehmen den server aus unserer liste über computationIt;
    cAdjustChannelInfo* adjChannelinfo;
    adjChannelinfo = m_adjustChannelInfoHash[m_ConfigData.m_AdjChannelList.at(computationIt)];

    m_MsgNrCmdList[adjChannelinfo->m_pPCBInterface->adjustComputation()] = adjustcomputation;
}


void cAdjustmentModuleMeasProgram::computationTest()
{
    computationIt++;
    if (computationIt == m_ConfigData.m_nAdjustmentChannelCount)
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
    if (var.toInt() == 1)
    {
        storageIt = 0;
        m_storageMachine.start();
    }
}


void cAdjustmentModuleMeasProgram::storageStart()
{
    // we have to start saving adjustment data for all particpating pcbservers
    cAdjustChannelInfo* adjChannelinfo;
    adjChannelinfo = m_adjustChannelInfoHash[m_ConfigData.m_AdjChannelList.at(storageIt)];

    m_MsgNrCmdList[adjChannelinfo->m_pPCBInterface->adjustStorage()] = adjuststorage;
}


void cAdjustmentModuleMeasProgram::storageTest()
{
    storageIt++;
    if (storageIt == m_ConfigData.m_nAdjustmentChannelCount)
        emit storageDone();
    else
        emit storageContinue();
}


void cAdjustmentModuleMeasProgram::storageFinished()
{
    m_pPARStorage->setValue(QVariant(0));
}


void cAdjustmentModuleMeasProgram::setAdjustStatusStartCommand(QVariant var)
{
    QStringList sl;
    cAdjustChannelInfo* adjChannelinfo;

    sl = var.toString().split(';');
    adjChannelinfo = m_adjustChannelInfoHash[m_ConfigData.m_AdjChannelList.at(computationIt)];

    m_MsgNrCmdList[adjChannelinfo->m_pPCBInterface->setAdjustStatus(sl.at(0), sl.at(1), sl.at(2).toInt())] = setadjuststatus;
}


void cAdjustmentModuleMeasProgram::setAdjustInitStartCommand(QVariant var)
{
    QStringList sl;
    cAdjustChannelInfo* adjChannelinfo;

    sl = var.toString().split(';');
    adjChannelinfo = m_adjustChannelInfoHash[m_ConfigData.m_AdjChannelList.at(computationIt)];

    m_nAdjustGainIt = m_nAdjustOffsetIt = m_nAdjustPhaseIt = 0;
    m_MsgNrCmdList[adjChannelinfo->m_pPCBInterface->adjustInit(sl.at(0), sl.at(1))] = adjustinit;
}


void cAdjustmentModuleMeasProgram::setAdjustAmplitudeStartCommand(QVariant var)
{
    setAdjustEnvironment(var);
    m_adjustAmplitudeMachine.start();
}


void cAdjustmentModuleMeasProgram::adjustamplitudeGetCorr()
{
    m_AdjustActualValue = m_pModule->m_pStorageSystem->getStoredValue(m_AdjustEntity, m_AdjustComponent).toDouble();
    m_MsgNrCmdList[m_AdjustPCBInterface->getGainCorrection(m_sAdjustChannel, m_sAdjustRange, m_AdjustActualValue)] = getgaincorrection;
}


void cAdjustmentModuleMeasProgram::adjustamplitudeSetNode()
{
    double Corr;

    Corr = m_AdjustTargetValue / m_AdjustActualValue;
    m_MsgNrCmdList[m_AdjustPCBInterface->setGainNode(m_sAdjustChannel, m_sAdjustRange, m_nAdjustGainIt, Corr, m_AdjustTargetValue)] = setgainnode;
    m_nAdjustGainIt++;
}


void cAdjustmentModuleMeasProgram::setAdjustPhaseStartCommand(QVariant var)
{
    setAdjustEnvironment(var);
    m_adjustPhaseMachine.start();
}


void cAdjustmentModuleMeasProgram::adjustphaseGetCorr()
{
    m_AdjustActualValue = cmpPhase(m_pModule->m_pStorageSystem->getStoredValue(m_AdjustEntity, m_AdjustComponent));
    m_MsgNrCmdList[m_AdjustPCBInterface->getPhaseCorrection(m_sAdjustChannel, m_sAdjustRange, m_AdjustActualValue)] = getphasecorrection;
}


void cAdjustmentModuleMeasProgram::adjustphaseSetNode()
{
    double Corr;

    Corr = m_AdjustTargetValue - m_AdjustActualValue;
    m_MsgNrCmdList[m_AdjustPCBInterface->setPhaseNode(m_sAdjustChannel, m_sAdjustRange, m_nAdjustPhaseIt, Corr, m_AdjustTargetValue)] = setphasenode;
    m_nAdjustPhaseIt++;
}


void cAdjustmentModuleMeasProgram::setAdjustOffsetStartCommand(QVariant var)
{
    setAdjustEnvironment(var);
    m_adjustOffsetMachine.start();
}


void cAdjustmentModuleMeasProgram::adjustoffsetGetCorr()
{
    m_AdjustActualValue = m_pModule->m_pStorageSystem->getStoredValue(m_AdjustEntity, m_AdjustComponent).toDouble();
    m_MsgNrCmdList[m_AdjustPCBInterface->getOffsetCorrection(m_sAdjustChannel, m_sAdjustRange, m_AdjustActualValue)] = getoffsetcorrection;
}


void cAdjustmentModuleMeasProgram::adjustoffsetSetNode()
{
    double Corr;

    Corr = m_AdjustTargetValue - m_AdjustActualValue;
    m_MsgNrCmdList[m_AdjustPCBInterface->setOffsetNode(m_sAdjustChannel, m_sAdjustRange, m_nAdjustOffsetIt, Corr, m_AdjustTargetValue)] = setoffsetnode;
    m_nAdjustOffsetIt++;
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
                    n = m_ConfigData.m_nAdjustmentChannelCount;
                    for (int i = 0; i < n; i++)
                        allPresent = allPresent && s.contains(m_ConfigData.m_AdjChannelList.at(i));
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
                        m_chnPortHash[m_ConfigData.m_AdjChannelList.at(activationIt)] = port;
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
                    sysName = m_ConfigData.m_AdjChannelList.at(activationIt);
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
                    m_adjustChannelInfoHash[m_ConfigData.m_AdjChannelList.at(activationIt)]->m_sRangelist = answer.toStringList();
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
                    emit computationContinue();
                else
                {
                    emit errMsg((tr(adjustcomputationPCBErrMSG)));
#ifdef DEBUG
                    qDebug() << adjustcomputationPCBErrMSG;
#endif
                    emit executionError();
                }

            case adjuststorage:
                if (reply == ack)
                    emit storageContinue();
                else
                {
                    emit errMsg((tr(adjuststoragePCBErrMSG)));
#ifdef DEBUG
                    qDebug() << adjuststoragePCBErrMSG;
#endif
                    emit executionError();
                }

            case setadjuststatus:
                if (reply == ack)
                    {} // nothing here to do
                else
                {
                    emit errMsg((tr(adjuststatusPCBErrMSG)));
#ifdef DEBUG
                    qDebug() << adjuststatusPCBErrMSG;
#endif
                    emit executionError();
                }

            case adjustinit:
                if (reply == ack)
                    {} // nothing here to do
                else
                {
                    emit errMsg((tr(adjustinitPCBErrMSG)));
#ifdef DEBUG
                    qDebug() << adjustinitPCBErrMSG;
#endif
                    emit executionError();
                }

            case getgaincorrection:
                if (reply == ack)
                {
                    m_AdjustCorrection = answer.toDouble();
                    if (fabs(m_AdjustCorrection - 1.0) > 1e-7)
                    {
                        emit errMsg(tr(adjustinitPCBErrMSG));
                        emit adjustError();
                    }
                    else
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
                }

            case setgainnode:
                if (reply == ack)
                {
                    emit adjustamplitudeContinue();
                }
                else
                {
                    emit errMsg(setGainNodeErrMsg);
                    emit adjustError();
#ifdef DEBUG
                    qDebug() << setGainNodeErrMsg;
#endif
                    emit executionError();
                }

            case getoffsetcorrection:
                if (reply == ack)
                {
                    m_AdjustCorrection = answer.toDouble();
                    if (fabs(m_AdjustCorrection) > 1e-7)
                    {
                        emit errMsg(tr(adjustinitPCBErrMSG));
                        emit adjustError();
                    }
                    else
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
                }

            case setoffsetnode:
                if (reply == ack)
                {
                    emit adjustoffsetContinue();
                }
                else
                {
                    emit errMsg(setGainNodeErrMsg);
                    emit adjustError();
#ifdef DEBUG
                    qDebug() << setGainNodeErrMsg;
#endif
                    emit executionError();
                }

            case getphasecorrection:
                if (reply == ack)
                {
                    m_AdjustCorrection = answer.toDouble();
                    if (fabs(m_AdjustCorrection) > 1e-7)
                    {
                        emit errMsg(tr(adjustinitPCBErrMSG));
                        emit adjustError();
                    }
                    else
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
                }

            case setphasenode:
                if (reply == ack)
                {
                    emit adjustphaseContinue();
                }
                else
                {
                    emit errMsg(setPhaseNodeErrMsg);
                    emit adjustError();
#ifdef DEBUG
                    qDebug() << setGainNodeErrMsg;
#endif
                    emit executionError();
                }
            }
        }
    }
}

}



