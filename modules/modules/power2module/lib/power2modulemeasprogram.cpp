#include "power2modulemeasprogram.h"
#include "power2module.h"
#include "power2moduleconfiguration.h"
#include "power2dspcmdgenerator.h"
#include "measmodephasesetstrategy4wire.h"
#include "veinvalidatorphasestringgenerator.h"
#include <measmodecatalog.h>
#include <stringvalidator.h>
#include <doublevalidator.h>
#include <intvalidator.h>
#include <reply.h>
#include <proxy.h>
#include <errormessages.h>
#include <math.h>
#include <timerfactoryqt.h>

namespace POWER2MODULE
{

cPower2ModuleMeasProgram::cPower2ModuleMeasProgram(cPower2Module* module, std::shared_ptr<cBaseModuleConfiguration> pConfiguration) :
    cBaseDspMeasProgram(pConfiguration), m_pModule(module)
{
    m_pDSPInterFace = new Zera::cDSPInterface();

    m_IdentifyState.addTransition(this, &cPower2ModuleMeasProgram::activationContinue, &m_readResourceTypesState);
    m_readResourceTypesState.addTransition(this, &cPower2ModuleMeasProgram::activationContinue, &m_readResourceSenseState);

    m_readResourceSenseState.addTransition(this, &cPower2ModuleMeasProgram::activationContinue, &m_readResourceSenseInfosState);
    m_readResourceSenseInfosState.addTransition(this, &cPower2ModuleMeasProgram::activationContinue, &m_readResourceSenseInfoState);
    m_readResourceSenseInfoState.addTransition(this, &cPower2ModuleMeasProgram::activationContinue, &m_readResourceSenseInfoDoneState);
    m_readResourceSenseInfoDoneState.addTransition(this, &cPower2ModuleMeasProgram::activationContinue, &m_readResourceSourceState);
    m_readResourceSenseInfoDoneState.addTransition(this, &cPower2ModuleMeasProgram::activationLoop, &m_readResourceSenseInfoState);

    m_readResourceSourceState.addTransition(this, &cPower2ModuleMeasProgram::activationContinue, &m_readResourceSourceInfosState);
    m_readResourceSourceState.addTransition(this, &cPower2ModuleMeasProgram::activationSkip, &m_pcbserverConnectState4measChannels);
    m_readResourceSourceInfosState.addTransition(this, &cPower2ModuleMeasProgram::activationContinue, &m_readResourceSourceInfoState);
    m_readResourceSourceInfoState.addTransition(this, &cPower2ModuleMeasProgram::activationContinue, &m_readResourceSourceInfoDoneState);
    m_readResourceSourceInfoDoneState.addTransition(this, &cPower2ModuleMeasProgram::activationContinue, &m_claimResourcesSourceState);
    m_readResourceSourceInfoDoneState.addTransition(this, &cPower2ModuleMeasProgram::activationLoop, &m_readResourceSourceInfoState);

    m_claimResourcesSourceState.addTransition(this, &cPower2ModuleMeasProgram::activationContinue, &m_claimResourceSourceState);
    m_claimResourceSourceState.addTransition(this, &cPower2ModuleMeasProgram::activationContinue, &m_claimResourceSourceDoneState);
    m_claimResourceSourceDoneState.addTransition(this, &cPower2ModuleMeasProgram::activationContinue, &m_pcbserverConnectState4measChannels);
    m_claimResourceSourceDoneState.addTransition(this, &cPower2ModuleMeasProgram::activationLoop, &m_claimResourceSourceState);
    m_pcbserverConnectState4measChannels.addTransition(this, &cPower2ModuleMeasProgram::activationContinue, &m_pcbserverConnectState4freqChannels);
    m_pcbserverConnectState4freqChannels.addTransition(this, &cPower2ModuleMeasProgram::activationContinue, &m_readSampleRateState);
    m_readSampleRateState.addTransition(this, &cPower2ModuleMeasProgram::activationContinue, &m_readSenseChannelInformationState);
    m_readSenseChannelInformationState.addTransition(this, &cPower2ModuleMeasProgram::activationContinue, &m_readSenseChannelAliasState);
    m_readSenseChannelAliasState.addTransition(this, &cPower2ModuleMeasProgram::activationContinue, &m_readSenseChannelUnitState);
    m_readSenseChannelUnitState.addTransition(this, &cPower2ModuleMeasProgram::activationContinue, &m_readSenseDspChannelState);
    m_readSenseDspChannelState.addTransition(this, &cPower2ModuleMeasProgram::activationContinue, &m_readSenseChannelInformationDoneState);
    m_readSenseChannelInformationDoneState.addTransition(this, &cPower2ModuleMeasProgram::activationContinue, &m_readSourceChannelInformationState);
    m_readSenseChannelInformationDoneState.addTransition(this, &cPower2ModuleMeasProgram::activationLoop, &m_readSenseChannelAliasState);

    m_readSourceChannelInformationState.addTransition(this, &cPower2ModuleMeasProgram::activationContinue, &m_readSourceChannelAliasState);
    m_readSourceChannelInformationState.addTransition(this, &cPower2ModuleMeasProgram::activationSkip, &m_dspserverConnectState);
    m_readSourceChannelAliasState.addTransition(this, &cPower2ModuleMeasProgram::activationContinue, &m_readSourceDspChannelState);
    m_readSourceDspChannelState.addTransition(this, &cPower2ModuleMeasProgram::activationContinue, &m_readSourceFormFactorState);
    m_readSourceFormFactorState.addTransition(this, &cPower2ModuleMeasProgram::activationContinue, &m_readSourceChannelInformationDoneState);
    m_readSourceChannelInformationDoneState.addTransition(this, &cPower2ModuleMeasProgram::activationContinue, &m_setSenseChannelRangeNotifiersState);
    m_readSourceChannelInformationDoneState.addTransition(this, &cPower2ModuleMeasProgram::activationLoop, &m_readSourceChannelAliasState);

    m_setSenseChannelRangeNotifiersState.addTransition(this, &cPower2ModuleMeasProgram::activationContinue, &m_setSenseChannelRangeNotifierState);
    m_setSenseChannelRangeNotifierState.addTransition(this, &cPower2ModuleMeasProgram::activationContinue, &m_setSenseChannelRangeNotifierDoneState);
    m_setSenseChannelRangeNotifierDoneState.addTransition(this, &cPower2ModuleMeasProgram::activationContinue, &m_dspserverConnectState);
    m_setSenseChannelRangeNotifierDoneState.addTransition(this, &cPower2ModuleMeasProgram::activationLoop, &m_setSenseChannelRangeNotifierState);

    m_claimPGRMemState.addTransition(this, &cPower2ModuleMeasProgram::activationContinue, &m_claimUSERMemState);
    m_claimUSERMemState.addTransition(this, &cPower2ModuleMeasProgram::activationContinue, &m_var2DSPState);
    m_var2DSPState.addTransition(this, &cPower2ModuleMeasProgram::activationContinue, &m_cmd2DSPState);
    m_cmd2DSPState.addTransition(this, &cPower2ModuleMeasProgram::activationContinue, &m_activateDSPState);
    m_activateDSPState.addTransition(this, &cPower2ModuleMeasProgram::activationContinue, &m_loadDSPDoneState);

    m_activationMachine.addState(&m_resourceManagerConnectState);
    m_activationMachine.addState(&m_IdentifyState);
    m_activationMachine.addState(&m_readResourceTypesState);

    m_activationMachine.addState(&m_readResourceSenseState);
    m_activationMachine.addState(&m_readResourceSenseInfosState);
    m_activationMachine.addState(&m_readResourceSenseInfoState);
    m_activationMachine.addState(&m_readResourceSenseInfoDoneState);

    m_activationMachine.addState(&m_readResourceSourceState);
    m_activationMachine.addState(&m_readResourceSourceInfosState);
    m_activationMachine.addState(&m_readResourceSourceInfoState);
    m_activationMachine.addState(&m_readResourceSourceInfoDoneState);

    m_activationMachine.addState(&m_claimResourcesSourceState);
    m_activationMachine.addState(&m_claimResourceSourceState);
    m_activationMachine.addState(&m_claimResourceSourceDoneState);
    m_activationMachine.addState(&m_pcbserverConnectState4measChannels);
    m_activationMachine.addState(&m_pcbserverConnectState4freqChannels);
    m_activationMachine.addState(&m_readSampleRateState);

    m_activationMachine.addState(&m_readSenseChannelInformationState);
    m_activationMachine.addState(&m_readSenseChannelAliasState);
    m_activationMachine.addState(&m_readSenseChannelUnitState);
    m_activationMachine.addState(&m_readSenseDspChannelState);
    m_activationMachine.addState(&m_readSenseChannelInformationDoneState);

    m_activationMachine.addState(&m_readSourceChannelInformationState);
    m_activationMachine.addState(&m_readSourceChannelAliasState);
    m_activationMachine.addState(&m_readSourceDspChannelState);
    m_activationMachine.addState(&m_readSourceFormFactorState);
    m_activationMachine.addState(&m_readSourceChannelInformationDoneState);

    m_activationMachine.addState(&m_setSenseChannelRangeNotifiersState);
    m_activationMachine.addState(&m_setSenseChannelRangeNotifierState);
    m_activationMachine.addState(&m_setSenseChannelRangeNotifierDoneState);

    m_activationMachine.addState(&m_dspserverConnectState);
    m_activationMachine.addState(&m_claimPGRMemState);
    m_activationMachine.addState(&m_claimUSERMemState);
    m_activationMachine.addState(&m_var2DSPState);
    m_activationMachine.addState(&m_cmd2DSPState);
    m_activationMachine.addState(&m_activateDSPState);
    m_activationMachine.addState(&m_loadDSPDoneState);

    if(m_pModule->m_demo)
        m_activationMachine.setInitialState(&m_loadDSPDoneState);
    else
        m_activationMachine.setInitialState(&m_resourceManagerConnectState);

    connect(&m_resourceManagerConnectState, &QAbstractState::entered, this, &cPower2ModuleMeasProgram::resourceManagerConnect);
    connect(&m_IdentifyState, &QAbstractState::entered, this, &cPower2ModuleMeasProgram::sendRMIdent);
    connect(&m_readResourceTypesState, &QAbstractState::entered, this, &cPower2ModuleMeasProgram::readResourceTypes);
    connect(&m_readResourceSenseState, &QAbstractState::entered, this, &cPower2ModuleMeasProgram::readResourceSense);
    connect(&m_readResourceSenseInfosState, &QAbstractState::entered, this, &cPower2ModuleMeasProgram::readResourceSenseInfos);
    connect(&m_readResourceSenseInfoState, &QAbstractState::entered, this, &cPower2ModuleMeasProgram::readResourceSenseInfo);
    connect(&m_readResourceSenseInfoDoneState, &QAbstractState::entered, this, &cPower2ModuleMeasProgram::readResourceSenseInfoDone);

    connect(&m_readResourceSourceState, &QAbstractState::entered, this, &cPower2ModuleMeasProgram::readResourceSource);
    connect(&m_readResourceSourceInfosState, &QAbstractState::entered, this, &cPower2ModuleMeasProgram::readResourceSourceInfos);
    connect(&m_readResourceSourceInfoState, &QAbstractState::entered, this, &cPower2ModuleMeasProgram::readResourceSourceInfo);
    connect(&m_readResourceSourceInfoDoneState, &QAbstractState::entered, this, &cPower2ModuleMeasProgram::readResourceSourceInfoDone);

    connect(&m_dspserverConnectState, &QAbstractState::entered, this, &cPower2ModuleMeasProgram::dspserverConnect);
    connect(&m_claimResourcesSourceState, &QAbstractState::entered, this, &cPower2ModuleMeasProgram::claimResourcesSource);
    connect(&m_claimResourceSourceState, &QAbstractState::entered, this, &cPower2ModuleMeasProgram::claimResourceSource);
    connect(&m_claimResourceSourceDoneState, &QAbstractState::entered, this, &cPower2ModuleMeasProgram::claimResourceSourceDone);

    connect(&m_pcbserverConnectState4measChannels, &QAbstractState::entered, this, &cPower2ModuleMeasProgram::pcbserverConnect4measChannels);
    connect(&m_pcbserverConnectState4freqChannels, &QAbstractState::entered, this, &cPower2ModuleMeasProgram::pcbserverConnect4freqChannels);
    connect(&m_readSampleRateState, &QAbstractState::entered, this, &cPower2ModuleMeasProgram::readSampleRate);

    connect(&m_readSenseChannelInformationState, &QAbstractState::entered, this, &cPower2ModuleMeasProgram::readSenseChannelInformation);
    connect(&m_readSenseChannelAliasState, &QAbstractState::entered, this, &cPower2ModuleMeasProgram::readSenseChannelAlias);
    connect(&m_readSenseChannelUnitState, &QAbstractState::entered, this, &cPower2ModuleMeasProgram::readSenseChannelUnit);
    connect(&m_readSenseDspChannelState, &QAbstractState::entered, this, &cPower2ModuleMeasProgram::readSenseDspChannel);
    connect(&m_readSenseChannelInformationDoneState, &QAbstractState::entered, this, &cPower2ModuleMeasProgram::readSenseChannelInformationDone);

    connect(&m_readSourceChannelInformationState, &QAbstractState::entered, this, &cPower2ModuleMeasProgram::readSourceChannelInformation);
    connect(&m_readSourceChannelAliasState, &QAbstractState::entered, this, &cPower2ModuleMeasProgram::readSourceChannelAlias);
    connect(&m_readSourceDspChannelState, &QAbstractState::entered, this, &cPower2ModuleMeasProgram::readSourceDspChannel);
    connect(&m_readSourceFormFactorState, &QAbstractState::entered, this, &cPower2ModuleMeasProgram::readSourceFormFactor);
    connect(&m_readSourceChannelInformationDoneState, &QAbstractState::entered, this, &cPower2ModuleMeasProgram::readSourceChannelInformationDone);

    connect(&m_setSenseChannelRangeNotifiersState, &QAbstractState::entered, this, &cPower2ModuleMeasProgram::setSenseChannelRangeNotifiers);
    connect(&m_setSenseChannelRangeNotifierState, &QAbstractState::entered, this, &cPower2ModuleMeasProgram::setSenseChannelRangeNotifier);
    connect(&m_setSenseChannelRangeNotifierDoneState, &QAbstractState::entered, this, &cPower2ModuleMeasProgram::setSenseChannelRangeNotifierDone);

    connect(&m_claimPGRMemState, &QAbstractState::entered, this, &cPower2ModuleMeasProgram::claimPGRMem);
    connect(&m_claimUSERMemState, &QAbstractState::entered, this, &cPower2ModuleMeasProgram::claimUSERMem);
    connect(&m_var2DSPState, &QAbstractState::entered, this, &cPower2ModuleMeasProgram::varList2DSP);
    connect(&m_cmd2DSPState, &QAbstractState::entered, this, &cPower2ModuleMeasProgram::cmdList2DSP);
    connect(&m_activateDSPState, &QAbstractState::entered, this, &cPower2ModuleMeasProgram::activateDSP);
    connect(&m_loadDSPDoneState, &QAbstractState::entered, this, &cPower2ModuleMeasProgram::activateDSPdone);

    // setting up statemachine for unloading dsp and setting resources free
    m_deactivateDSPState.addTransition(this, &cPower2ModuleMeasProgram::deactivationContinue, &m_freePGRMemState);
    m_freePGRMemState.addTransition(this, &cPower2ModuleMeasProgram::deactivationContinue, &m_freeUSERMemState);
    m_freeUSERMemState.addTransition(this, &cPower2ModuleMeasProgram::deactivationContinue, &m_freeFreqOutputsState);

    m_freeFreqOutputsState.addTransition(this, &cPower2ModuleMeasProgram::deactivationContinue, &m_freeFreqOutputState);
    m_freeFreqOutputsState.addTransition(this, &cPower2ModuleMeasProgram::deactivationSkip, &m_resetNotifiersState);
    m_freeFreqOutputState.addTransition(this, &cPower2ModuleMeasProgram::deactivationContinue, &m_freeFreqOutputDoneState);
    m_freeFreqOutputDoneState.addTransition(this, &cPower2ModuleMeasProgram::deactivationContinue, &m_resetNotifiersState);
    m_freeFreqOutputDoneState.addTransition(this, &cPower2ModuleMeasProgram::deactivationLoop, &m_freeFreqOutputState);
    m_resetNotifiersState.addTransition(this, &cPower2ModuleMeasProgram::deactivationContinue, &m_resetNotifierState);
    m_resetNotifierState.addTransition(this, &cPower2ModuleMeasProgram::deactivationContinue, &m_resetNotifierDoneState);
    m_resetNotifierDoneState.addTransition(this, &cPower2ModuleMeasProgram::deactivationContinue, &m_unloadDSPDoneState);
    m_resetNotifierDoneState.addTransition(this, &cPower2ModuleMeasProgram::deactivationLoop, &m_resetNotifierState);

    m_deactivationMachine.addState(&m_deactivateDSPState);
    m_deactivationMachine.addState(&m_freePGRMemState);
    m_deactivationMachine.addState(&m_freeUSERMemState);

    m_deactivationMachine.addState(&m_freeFreqOutputsState);
    m_deactivationMachine.addState(&m_freeFreqOutputState);
    m_deactivationMachine.addState(&m_freeFreqOutputDoneState);

    m_deactivationMachine.addState(&m_resetNotifiersState);
    m_deactivationMachine.addState(&m_resetNotifierState);
    m_deactivationMachine.addState(&m_resetNotifierDoneState);

    m_deactivationMachine.addState(&m_unloadDSPDoneState);

    if(m_pModule->m_demo)
        m_deactivationMachine.setInitialState(&m_unloadDSPDoneState);
    else
        m_deactivationMachine.setInitialState(&m_deactivateDSPState);

    connect(&m_deactivateDSPState, &QAbstractState::entered, this, &cPower2ModuleMeasProgram::deactivateDSP);
    connect(&m_freePGRMemState, &QAbstractState::entered, this, &cPower2ModuleMeasProgram::freePGRMem);
    connect(&m_freeUSERMemState, &QAbstractState::entered, this, &cPower2ModuleMeasProgram::freeUSERMem);

    connect(&m_freeFreqOutputsState, &QAbstractState::entered, this, &cPower2ModuleMeasProgram::freeFreqOutputs);
    connect(&m_freeFreqOutputState, &QAbstractState::entered, this, &cPower2ModuleMeasProgram::freeFreqOutput);
    connect(&m_freeFreqOutputDoneState, &QAbstractState::entered, this, &cPower2ModuleMeasProgram::freeFreqOutputDone);
    connect(&m_resetNotifiersState, &QAbstractState::entered, this, &cPower2ModuleMeasProgram::resetNotifiers);
    connect(&m_resetNotifierState, &QAbstractState::entered, this, &cPower2ModuleMeasProgram::resetNotifier);
    connect(&m_resetNotifierDoneState, &QAbstractState::entered, this, &cPower2ModuleMeasProgram::resetNotifierDone);

    connect(&m_unloadDSPDoneState, &QAbstractState::entered, this, &cPower2ModuleMeasProgram::deactivateDSPdone);

    // setting up statemachine for data acquisition
    m_dataAcquisitionState.addTransition(this, &cPower2ModuleMeasProgram::dataAquisitionContinue, &m_dataAcquisitionDoneState);
    m_dataAcquisitionMachine.addState(&m_dataAcquisitionState);
    m_dataAcquisitionMachine.addState(&m_dataAcquisitionDoneState);
    m_dataAcquisitionMachine.setInitialState(&m_dataAcquisitionState);
    connect(&m_dataAcquisitionState, &QAbstractState::entered, this, &cPower2ModuleMeasProgram::dataAcquisitionDSP);
    connect(&m_dataAcquisitionDoneState, &QAbstractState::entered, this, &cPower2ModuleMeasProgram::dataReadDSP);

    // setting up statemachine for reading urvalues from channels that changed its range
    m_readUrvalueState.addTransition(this, &cPower2ModuleMeasProgram::activationContinue, &m_readUrvalueDoneState);
    m_readUrvalueDoneState.addTransition(this, &cPower2ModuleMeasProgram::activationContinue, &m_foutParamsToDsp);
    m_readUrvalueDoneState.addTransition(this, &cPower2ModuleMeasProgram::activationLoop, &m_readUrvalueState);
    m_foutParamsToDsp.addTransition(this, &cPower2ModuleMeasProgram::activationContinue, &m_setFoutConstantState);

    m_readUpperRangeValueMachine.addState(&m_readUrvalueState);
    m_readUpperRangeValueMachine.addState(&m_readUrvalueDoneState);
    m_readUpperRangeValueMachine.addState(&m_foutParamsToDsp);
    m_readUpperRangeValueMachine.addState(&m_setFoutConstantState);

    m_readUpperRangeValueMachine.setInitialState(&m_readUrvalueState);

    connect(&m_readUrvalueState, &QAbstractState::entered, this, &cPower2ModuleMeasProgram::readUrvalue);
    connect(&m_readUrvalueDoneState, &QAbstractState::entered, this, &cPower2ModuleMeasProgram::readUrvalueDone);
    connect(&m_foutParamsToDsp, &QAbstractState::entered, this, &cPower2ModuleMeasProgram::foutParamsToDsp);

    if(m_pModule->m_demo) {
        m_demoPeriodicTimer = TimerFactoryQt::createPeriodic(500);
        connect(m_demoPeriodicTimer.get(), &TimerTemplateQt::sigExpired,this, &cPower2ModuleMeasProgram::handleDemoActualValues);
    }
}


cPower2ModuleMeasProgram::~cPower2ModuleMeasProgram()
{
    delete m_pDSPInterFace;
}


void cPower2ModuleMeasProgram::start()
{
    if (getConfData()->m_bmovingWindow) {
        m_movingwindowFilter.setIntegrationtime(getConfData()->m_fMeasIntervalTime.m_fValue);
        connect(this, &cPower2ModuleMeasProgram::actualValues, &m_movingwindowFilter, &cMovingwindowFilter::receiveActualValues);
        connect(&m_movingwindowFilter, &cMovingwindowFilter::actualValues, this, &cPower2ModuleMeasProgram::setInterfaceActualValues);
    }
    else
        connect(this, &cPower2ModuleMeasProgram::actualValues, this, &cPower2ModuleMeasProgram::setInterfaceActualValues);
}


void cPower2ModuleMeasProgram::stop()
{
    disconnect(this, &cPower2ModuleMeasProgram::actualValues, 0, 0);
    disconnect(&m_movingwindowFilter, &cMovingwindowFilter::actualValues, this, 0);
}


void cPower2ModuleMeasProgram::generateInterface()
{
    QString key;
    VfModuleActvalue *pActvalue;
    for (int i = 0; i < MeasPhaseCount+SumValueCount; i++) {
        pActvalue = new VfModuleActvalue(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                            QString("ACT_PP%1").arg(i+1),
                                            QString("Actual value positive power"),
                                            QVariant(0.0) );
        m_veinActValueList.append(pActvalue); // we add the component for our measurement
        m_pModule->veinModuleActvalueList.append(pActvalue); // and for the modules interface

        pActvalue = new VfModuleActvalue(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                            QString("ACT_PM%1").arg(i+1),
                                            QString("Actual value negative power"),
                                            QVariant(0.0) );
        m_veinActValueList.append(pActvalue); // we add the component for our measurement
        m_pModule->veinModuleActvalueList.append(pActvalue); // and for the modules interface

        QString strDescription;
        if(i<MeasPhaseCount)
            strDescription = QString("Actual power value phase %1").arg(i+1);
        else
            strDescription = QString("Actual power value sum all phases");
        pActvalue = new VfModuleActvalue(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                            QString("ACT_P%1").arg(i+1),
                                            strDescription,
                                            QVariant(0.0) );
        m_veinActValueList.append(pActvalue); // we add the component for our measurement
        m_pModule->veinModuleActvalueList.append(pActvalue); // and for the modules interface

    }

    m_pPQSCountInfo = new VfModuleMetaData(QString("PQSCount"), QVariant(3*(MeasPhaseCount+SumValueCount)));
    m_pModule->veinModuleMetaDataList.append(m_pPQSCountInfo);
    m_pNomFrequencyInfo =  new VfModuleMetaData(QString("NominalFrequency"), QVariant(getConfData()->m_nNominalFrequency));
    m_pModule->veinModuleMetaDataList.append(m_pNomFrequencyInfo);
    m_pFoutCount = new VfModuleMetaData(QString("FOUTCount"), QVariant(getConfData()->m_nFreqOutputCount));
    m_pModule->veinModuleMetaDataList.append(m_pFoutCount);

    // our parameters we deal with
    m_pMeasuringmodeParameter = new VfModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                                         key = QString("PAR_MeasuringMode"),
                                                         QString("Measuring mode"),
                                                         QVariant(getConfData()->m_sMeasuringMode.m_sValue));

    m_pMeasuringmodeParameter->setSCPIInfo(new cSCPIInfo("CONFIGURATION","MMODE", "10", "PAR_MeasuringMode", "0", ""));
    cStringValidator *sValidator = new cStringValidator(getConfData()->m_sMeasmodeList);
    m_pMeasuringmodeParameter->setValidator(sValidator);
    m_pModule->veinModuleParameterHash[key] = m_pMeasuringmodeParameter; // for modules use

    QVariant val;
    QString s, unit;
    bool btime = (getConfData()->m_sIntegrationMode == "time");
    if (btime) {
        val = QVariant(getConfData()->m_fMeasIntervalTime.m_fValue);
        s = QString("Integration time");
        unit = QString("s");
    }
    else {
        val = QVariant(getConfData()->m_nMeasIntervalPeriod.m_nValue);
        s = QString("Integration period");
        unit = QString("period");
    }

    m_pIntegrationParameter = new VfModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                                       key = QString("PAR_Interval"),
                                                       s,
                                                       val);
    m_pIntegrationParameter->setUnit(unit);

    if (btime)
    {
        m_pIntegrationParameter->setSCPIInfo(new cSCPIInfo("CONFIGURATION","TINTEGRATION", "10", "PAR_Interval", "0", unit));
        cDoubleValidator *dValidator;
        dValidator = new cDoubleValidator(1.0, 100.0, 0.5);
        m_pIntegrationParameter->setValidator(dValidator);
    }
    else
    {
        m_pIntegrationParameter->setSCPIInfo(new cSCPIInfo("CONFIGURATION","TPERIOD", "10", "PAR_Interval", "0", unit));
        cIntValidator *iValidator;
        iValidator = new cIntValidator(5, 5000, 1);
        m_pIntegrationParameter->setValidator(iValidator);
    }

    m_pModule->veinModuleParameterHash[key] = m_pIntegrationParameter; // for modules use

    m_pMeasureSignal = new VfModuleComponent(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                                QString("SIG_Measuring"),
                                                QString("Signal indicating measurement activity"),
                                                QVariant(0));

    m_pModule->veinModuleComponentList.append(m_pMeasureSignal);
}


void cPower2ModuleMeasProgram::setDspVarList()
{
    // we fetch a handle for sampled data and other temporary values
    m_pTmpDataDsp = m_pDSPInterFace->getMemHandle("TmpData");
    m_pTmpDataDsp->addVarItem( new cDspVar("MEASSIGNAL1", m_nSRate, DSPDATA::vDspTemp)); // we need 2 signals for our computations
    m_pTmpDataDsp->addVarItem( new cDspVar("MEASSIGNAL2", m_nSRate, DSPDATA::vDspTemp)); // we need 2 signals for our computations
    m_pTmpDataDsp->addVarItem( new cDspVar("VALPOWER", 3*(MeasPhaseCount+SumValueCount), DSPDATA::vDspTemp)); // p1+,p1-,p1, p2+,p2-,p2, p3+,p3-,p3, ps+,ps-,ps
    m_pTmpDataDsp->addVarItem( new cDspVar("TEMP1", 2, DSPDATA::vDspTemp)); // we need 2 temp. vars also for complex
    m_pTmpDataDsp->addVarItem( new cDspVar("TEMP2", 2, DSPDATA::vDspTemp));
    m_pTmpDataDsp->addVarItem( new cDspVar("FILTER", 2*3*(MeasPhaseCount+SumValueCount),DSPDATA::vDspTemp));
    m_pTmpDataDsp->addVarItem( new cDspVar("N",1,DSPDATA::vDspTemp));

    // a handle for parameter
    m_pParameterDSP =  m_pDSPInterFace->getMemHandle("Parameter");
    m_pParameterDSP->addVarItem( new cDspVar("TIPAR",1, DSPDATA::vDspParam, DSPDATA::dInt)); // integrationtime res = 1ms or period
    // we use tistart as parameter, so we can finish actual measuring interval bei setting 0
    m_pParameterDSP->addVarItem( new cDspVar("TISTART",1, DSPDATA::vDspParam, DSPDATA::dInt));
    m_pParameterDSP->addVarItem( new cDspVar("MMODE",1, DSPDATA::vDspParam, DSPDATA::dInt));

    // a handle for filtered actual values
    m_pActualValuesDSP = m_pDSPInterFace->getMemHandle("ActualValues");
    m_pActualValuesDSP->addVarItem( new cDspVar("VALPOWERF", 3*(MeasPhaseCount+SumValueCount), DSPDATA::vDspResult));

    // and one for the frequency output scale values, we need 1 value for each configured output
    m_pfreqScaleDSP = m_pDSPInterFace->getMemHandle("FrequencyScale");
    m_pfreqScaleDSP->addVarItem( new cDspVar("FREQSCALE", getConfData()->m_nFreqOutputCount, DSPDATA::vDspParam));

    // and one for nominal power in case that measuring mode is qref
    m_pNomPower = m_pDSPInterFace->getMemHandle("QRefScale");
    m_pNomPower->addVarItem( new cDspVar("NOMPOWER", 1, DSPDATA::vDspParam));

    m_ModuleActualValues.resize(m_pActualValuesDSP->getSize()); // we provide a vector for generated actual values
    m_nDspMemUsed = m_pTmpDataDsp->getSize() + m_pParameterDSP->getSize() + m_pActualValuesDSP->getSize();
}


void cPower2ModuleMeasProgram::deleteDspVarList()
{
    m_pDSPInterFace->deleteMemHandle(m_pTmpDataDsp);
    m_pDSPInterFace->deleteMemHandle(m_pParameterDSP);
    m_pDSPInterFace->deleteMemHandle(m_pActualValuesDSP);
}

QStringList cPower2ModuleMeasProgram::dspCmdInitVars(int dspInitialSelectCode)
{
    QStringList dspCmdList;
    dspCmdList.append("STARTCHAIN(1,1,0x0101)"); // aktiv, prozessnr. (dummy),hauptkette 1 subkette 1 start
    dspCmdList.append(QString("CLEARN(%1,MEASSIGNAL1)").arg(m_nSRate) ); // clear meassignal
    dspCmdList.append(QString("CLEARN(%1,MEASSIGNAL2)").arg(m_nSRate) ); // clear meassignal
    dspCmdList.append(QString("CLEARN(%1,FILTER)").arg(2*3*(MeasPhaseCount+SumValueCount)+1) ); // clear the whole filter incl. count
    dspCmdList.append(QString("SETVAL(MMODE,%1)").arg(dspInitialSelectCode));
    double integrationTime = calcTiTime();
    bool intergrationModeTime = getConfData()->m_sIntegrationMode == "time";
    dspCmdList.append(QString("SETVAL(TIPAR,%1)").arg(integrationTime)); // initial ti time
    if(intergrationModeTime)
        dspCmdList.append("GETSTIME(TISTART)"); // einmal ti start setzen
    dspCmdList.append("DEACTIVATECHAIN(1,0x0101)"); // ende prozessnr., hauptkette 1 subkette 1
    dspCmdList.append("STOPCHAIN(1,0x0101)"); // ende prozessnr., hauptkette 1 subkette 1
    return dspCmdList;
}

void cPower2ModuleMeasProgram::setDspCmdList()
{
    MeasSystemChannels measChannelPairList;
    cPower2ModuleConfigData *confdata = getConfData();
    for(auto &measChannelPair : confdata->m_sMeasSystemList) {
        QStringList channelPairSplit = measChannelPair.split(',');
        MeasSystemChannel measChannel;
        measChannel.voltageChannel = m_measChannelInfoHash.value(channelPairSplit.at(0)).dspChannelNr;
        measChannel.currentChannel = m_measChannelInfoHash.value(channelPairSplit.at(1)).dspChannelNr;
        measChannelPairList.append(measChannel);
    }

    int measSytemCount = confdata->m_sMeasSystemList.count();
    //MeasModeBroker measBroker(Power2DspModeFunctionCatalog::get(measSytemCount));

    // we set up all our lists for wanted measuring modes, this gets much more performance
    QStringList dspMModesCommandList;
    //MeasModeBroker::BrokerReturn brokerReturn;
    for (int i = 0; i < confdata->m_nMeasModeCount; i++) {
        cMeasModeInfo mInfo = MeasModeCatalog::getInfo(confdata->m_sMeasmodeList.at(i));
        measmodes measModeId = mInfo.getCode();
        switch (measModeId)
        {
        case m4lw:
            dspMModesCommandList.append(Power2DspCmdGenerator::getCmdsMMode4LW(measModeId, measChannelPairList, m_nSRate));
            m_measModeSelector.addMode(std::make_shared<MeasMode>(mInfo.getName(),
                                                                  measModeId,
                                                                  measSytemCount,
                                                                  std::make_unique<MeasModePhaseSetStrategy4Wire>()));
            break;
        default:
            break;
        }
    }
    dspMModesCommandList.append(Power2DspCmdGenerator::getCmdsSumAndAverage());

    m_measModeSelector.tryChangeMode(confdata->m_sMeasuringMode.m_sValue);
    std::shared_ptr<MeasMode> mode = m_measModeSelector.getCurrMode();
    int dspSelectCodeFromConfig = mode->getDspSelectCode();
    QStringList dspInitVarsList = dspCmdInitVars(dspSelectCodeFromConfig);

    // sequence here is important
    m_pDSPInterFace->addCycListItems(dspInitVarsList);
    m_pDSPInterFace->addCycListItems(dspMModesCommandList);

    // so... let's now set our frequency outputs if he have some
    QString s;
    if (confdata->m_sFreqActualizationMode == "signalperiod")
    {
        if (confdata->m_nFreqOutputCount > 0)
        {
            for (int i = 0; i < confdata->m_nFreqOutputCount; i++)
            {
                // which actualvalue do we take as source (offset)
                quint8 actvalueIndex = cmpActualValIndex(confdata->m_FreqOutputConfList.at(i));

                QString foutSystemName =  confdata->m_FreqOutputConfList.at(i).m_sName;
                // here we set abs, plus or minus and which frequency output has to be set
                // we could also take absPower because we have dedicated values for + , - , abs

                quint16 freqpar = confdata->m_FreqOutputConfList.at(i).m_nFoutMode + (m_FoutInfoHash[foutSystemName].dspFoutChannel << 8);
                // frequenzausgang berechnen lassen
                m_pDSPInterFace->addCycListItem( s = QString("CMPCLK(%1,VALPOWER+%2,FREQSCALE+%3)")
                                                 .arg(freqpar)
                                                 .arg(actvalueIndex)
                                                 .arg(i));
            }
        }
    }

    if (confdata->m_sIntegrationMode == "time")
    {
        m_pDSPInterFace->addCycListItem( s = "TESTTIMESKIPNEX(TISTART,TIPAR)");
        m_pDSPInterFace->addCycListItem( s = "ACTIVATECHAIN(1,0x0102)");

        m_pDSPInterFace->addCycListItem( s = "STARTCHAIN(0,1,0x0102)");
            m_pDSPInterFace->addCycListItem( s = "GETSTIME(TISTART)"); // set new system time
            m_pDSPInterFace->addCycListItem( s = QString("CMPAVERAGE1(12,FILTER,VALPOWERF)"));
            m_pDSPInterFace->addCycListItem( s = QString("CLEARN(%1,FILTER)").arg(2*12+1) );
            m_pDSPInterFace->addCycListItem( s = QString("DSPINTTRIGGER(0x0,0x%1)").arg(irqNr)); // send interrupt to module

            if (confdata->m_sFreqActualizationMode == "integrationtime")
            {
                if (confdata->m_nFreqOutputCount > 0)
                {
                    for (int i = 0; i < confdata->m_nFreqOutputCount; i++)
                    {
                        // which actualvalue do we take as source (offset)
                        quint8 actvalueIndex = cmpActualValIndex(confdata->m_FreqOutputConfList.at(i));

                        QString foutSystemName =  confdata->m_FreqOutputConfList.at(i).m_sName;
                        // here we set abs, plus or minus and which frequency output has to be set
                        // we could also take absPower because we have dedicated values for + , - , abs
                        quint16 freqpar = confdata->m_FreqOutputConfList.at(i).m_nFoutMode + (m_FoutInfoHash[foutSystemName].dspFoutChannel << 8);
                        // frequenzausgang berechnen lassen
                        m_pDSPInterFace->addCycListItem( s = QString("CMPCLK(%1,VALPOWERF+%2,FREQSCALE+%3)")
                                                         .arg(freqpar)
                                                         .arg(actvalueIndex)
                                                         .arg(i));
                    }
                }
            }

            m_pDSPInterFace->addCycListItem( s = "DEACTIVATECHAIN(1,0x0102)");

        m_pDSPInterFace->addCycListItem( s = "STOPCHAIN(1,0x0102)"); // end processnr., mainchain 1 subchain 2

    }

    else // otherwise it is period
    {

        m_pDSPInterFace->addCycListItem( s = "TESTVVSKIPLT(N,TIPAR)");
        m_pDSPInterFace->addCycListItem( s = "ACTIVATECHAIN(1,0x0103)");
        m_pDSPInterFace->addCycListItem( s = "STARTCHAIN(0,1,0x0103)");
            m_pDSPInterFace->addCycListItem( s = QString("CMPAVERAGE1(12,FILTER,VALPOWERF)"));
            m_pDSPInterFace->addCycListItem( s = QString("CLEARN(%1,FILTER)").arg(2*12+1) );
            m_pDSPInterFace->addCycListItem( s = QString("DSPINTTRIGGER(0x0,0x%1)").arg(irqNr)); // send interrupt to module

            if (confdata->m_sFreqActualizationMode == "integrationtime")
            {
                if (confdata->m_nFreqOutputCount > 0)
                {
                    for (int i = 0; i < confdata->m_nFreqOutputCount; i++)
                    {
                        // which actualvalue do we take as source (offset)
                        quint8 actvalueIndex = cmpActualValIndex(confdata->m_FreqOutputConfList.at(i));
                        QString foutSystemName =  confdata->m_FreqOutputConfList.at(i).m_sName;
                        // here we set abs, plus or minus and which frequency output has to be set
                        quint16 freqpar = confdata->m_FreqOutputConfList.at(i).m_nFoutMode + (m_FoutInfoHash[foutSystemName].dspFoutChannel << 8);
                        // frequenzausgang berechnen lassen
                        m_pDSPInterFace->addCycListItem( s = QString("CMPCLK(%1,VALPOWERF+%2,FREQSCALE+%3)")
                                                         .arg(freqpar)
                                                         .arg(actvalueIndex)
                                                         .arg(i));
                    }
                }
            }

            m_pDSPInterFace->addCycListItem( s = "DEACTIVATECHAIN(1,0x0103)");

        m_pDSPInterFace->addCycListItem( s = "STOPCHAIN(1,0x0103)"); // end processnr., mainchain 1 subchain 2
    }

}


void cPower2ModuleMeasProgram::deleteDspCmdList()
{
    m_pDSPInterFace->clearCmdList();
}


void cPower2ModuleMeasProgram::catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer)
{
    bool ok;

    if (msgnr == 0) // 0 was reserved for async. messages
    {
        QString sintnr;
        sintnr = answer.toString().section(':', 1, 1);
        int service = sintnr.toInt(&ok);
        switch (service)
        {
        case irqNr:
            // we got an interrupt from our cmd chain and have to fetch our actual values
            // but we synchronize on ranging process
            if (m_bActive && !m_dataAcquisitionMachine.isRunning()) // in case of deactivation in progress, no dataaquisition
                m_dataAcquisitionMachine.start();
            break;
        case irqNr+1:
        case irqNr+2:
        case irqNr+3:
        case irqNr+4:
        case irqNr+5:
        case irqNr+6:
            // we got a sense:channel:range notifier
            // let's look what to do
        {
            QString s;
            s = m_NotifierInfoHash[service];
            readUrvalueList.append(s);
            if (!m_readUpperRangeValueMachine.isRunning())
                m_readUpperRangeValueMachine.start();
        }

            break;
        }
    }
    else
    {
        // maybe other objexts share the same dsp interface
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
                    emit activationError();
                }
                break;

            case setchannelrangenotifier:
                if (reply == ack) // we only continue pcb server acknowledges
                {
                    m_NotifierInfoHash[m_notifierNr] = infoRead;
                    emit activationContinue();
                }
                else
                {
                    emit errMsg((tr(registerpcbnotifierErrMsg)));
                    emit activationError();
                }
                break;

            case readurvalue:
            {
                double urvalue;
                cMeasChannelInfo mi;

                if (reply == ack)
                {
                    urvalue = answer.toDouble(&ok);
                    mi = m_measChannelInfoHash.take(readUrvalueInfo);
                    mi.m_fUrValue = urvalue;
                    m_measChannelInfoHash[readUrvalueInfo] = mi;
                    emit activationContinue();
                }
                else
                {
                    emit errMsg((tr(readrangeurvalueErrMsg)));
                    emit activationError();
                }
                break;
            }

            case setqrefnominalpower:
                if (reply != ack) // we ignore ack
                {
                    emit errMsg((tr(writedspmemoryErrMsg)));
                    emit executionError();
                }
                break;

            case claimpgrmem:
                if (reply == ack) // we only continue if resource manager acknowledges
                    emit activationContinue();
                else
                {
                    emit errMsg((tr(claimresourceErrMsg)));
                    emit activationError();
                }
                break;

            case claimusermem:
                if (reply == ack) // we only continue if resource manager acknowledges
                    emit activationContinue();
                else
                {
                    emit errMsg((tr(claimresourceErrMsg)));
                    emit activationError();
                }
                break;

            case varlist2dsp:
                if (reply == ack) // we only continue if resource manager acknowledges
                    emit activationContinue();
                else
                {
                    emit errMsg((tr(dspvarlistwriteErrMsg)));
                    emit activationError();
                }
                break;

            case cmdlist2dsp:
                if (reply == ack) // we only continue if resource manager acknowledges
                    emit activationContinue();
                else
                {
                    emit errMsg((tr(dspcmdlistwriteErrMsg)));
                    emit activationError();
                }
                break;

            case activatedsp:
                if (reply == ack) // we only continue if resource manager acknowledges
                    emit activationContinue();
                else
                {
                    emit errMsg((tr(dspactiveErrMsg)));
                    emit activationError();
                }
                break;

            case readresourcetypes:
                ok = false;
                if ((reply == ack) && (answer.toString().contains("SENSE"))) // we need sense  at least
                {
                    if (getConfData()->m_nFreqOutputCount > 0)
                    {
                        if (answer.toString().contains("SOURCE")) // maybe we also need source
                            ok = true;

                    }
                    else
                        ok = true;
                }

                if (ok)
                    emit activationContinue();
                else

                {
                    emit errMsg((tr(resourcetypeErrMsg)));
                    emit activationError();
                }
                break;

            case readresourcesense:
            {
                if (reply == ack)
                {
                    bool allfound = true;
                    QList<QString> sl = m_measChannelInfoHash.keys();
                    QString s = answer.toString();
                    for (int i = 0; i < sl.count(); i++)
                    {
                        if (!s.contains(sl.at(i)))
                            allfound = false;
                    }

                    if (allfound)
                        emit activationContinue();
                    else
                    {
                        emit errMsg((tr(resourceErrMsg)));
                        emit activationError();
                    }
                }
                else
                {
                    emit errMsg((tr(resourceErrMsg)));
                    emit activationError();
                }
                break;
            }

            case readresourcesenseinfo:
            {
                int port;
                QStringList sl;
                cMeasChannelInfo mi;

                sl = answer.toString().split(';');
                if ((reply == ack) && (sl.length() >= 4))
                {
                    port = sl.at(3).toInt(&ok); // we have to set the port where we can find our meas channel
                    if (ok)
                    {
                        mi = m_measChannelInfoHash.take(infoRead);
                        mi.pcbServersocket.m_nPort = port;
                        m_measChannelInfoHash[infoRead] = mi;
                        emit activationContinue();
                    }
                    else
                    {
                        emit errMsg((tr(resourceInfoErrMsg)));
                        emit activationError();
                    }
                }
                else
                {
                    emit errMsg((tr(resourceInfoErrMsg)));
                    emit activationError();
                }
                break;
            }

            case readresourcesource:
            {
                if (reply == ack)
                {
                    bool allfound = true;
                    QList<QString> sl = m_FoutInfoHash.keys();
                    QString s = answer.toString();
                    for (int i = 0; i < sl.count(); i++)
                    {
                        if (!s.contains(sl.at(i)))
                            allfound = false;
                    }

                    if (allfound)
                        emit activationContinue();
                    else
                    {
                        emit errMsg((tr(resourceErrMsg)));
                        emit activationError();
                    }
                }
                else
                {
                    emit errMsg((tr(resourceErrMsg)));
                    emit activationError();
                }
                break;
            }

            case readresourcesourceinfo:
            {
                int port, max, free;
                bool ok1, ok2, ok3;
                QStringList sl;
                cFoutInfo fi;

                sl = answer.toString().split(';');
                if ((reply == ack) && (sl.length() >= 4))
                {
                    max = sl.at(0).toInt(&ok1); // fixed position
                    free = sl.at(1).toInt(&ok2);
                    port = sl.at(3).toInt(&ok3);

                    if (ok1 && ok2 && ok3 && ((max == free) == 1))
                    {
                        fi = m_FoutInfoHash.take(infoRead);
                        fi.pcbServersocket.m_nPort = port;
                        m_FoutInfoHash[infoRead] = fi;
                        emit activationContinue();
                    }
                    else
                    {
                        emit errMsg((tr(resourceInfoErrMsg)));
                        emit activationError();
                    }
                }
                else
                {
                    emit errMsg((tr(resourceInfoErrMsg)));
                    emit activationError();
                }
                break;
            }

            case claimresourcesource:
            if (reply == ack)
                emit activationContinue();
            else
            {
                emit errMsg((tr(claimresourceErrMsg)));
                emit activationError();
            }
            break;

            case readsamplerate:
            if (reply == ack)
            {
                m_nSRate = answer.toInt(&ok);
                emit activationContinue();
            }
            else
            {
                emit errMsg((tr(readsamplerateErrMsg)));
                emit activationError();
            }
            break;

            case readsensechannelalias:
            {
                QString alias;
                cMeasChannelInfo mi;

                if (reply == ack)
                {
                    alias = answer.toString();
                    mi = m_measChannelInfoHash.take(infoRead);
                    mi.alias = alias;
                    m_measChannelInfoHash[infoRead] = mi;
                    emit activationContinue();
                }
                else
                {
                    emit errMsg((tr(readaliasErrMsg)));
                    emit activationError();
                }
                break;
            }

            case readsensechannelunit:
            {
                QString unit;
                cMeasChannelInfo mi;

                if (reply == ack)
                {
                    unit = answer.toString();
                    mi = m_measChannelInfoHash.take(infoRead);
                    mi.unit = unit;
                    m_measChannelInfoHash[infoRead] = mi;
                    emit activationContinue();
                }
                else
                {
                    emit errMsg((tr(readunitErrMsg)));
                    emit activationError();
                }
                break;
            }

            case readsensechanneldspchannel:
            {
                int chnnr;
                cMeasChannelInfo mi;

                if (reply == ack)
                {
                    chnnr = answer.toInt(&ok);
                    mi = m_measChannelInfoHash.take(infoRead);
                    mi.dspChannelNr = chnnr;
                    m_measChannelInfoHash[infoRead] = mi;
                    emit activationContinue();
                }
                else
                {
                    emit errMsg((tr(readdspchannelErrMsg)));
                    emit activationError();
                }
                break;
            break;
            }

            case readsourcechannelalias:
            {
                QString alias;
                cFoutInfo fi;

                if (reply == ack)
                {
                    alias = answer.toString();
                    fi = m_FoutInfoHash.take(infoRead);
                    fi.alias = alias;
                    m_FoutInfoHash[infoRead] = fi;
                    emit activationContinue();
                }
                else
                {
                    emit errMsg((tr(readaliasErrMsg)));
                    emit activationError();
                }
                break;
            }

            case readsourcechanneldspchannel:
            {
                int chnnr;
                cFoutInfo fi;

                if (reply == ack)
                {
                    chnnr = answer.toInt(&ok);
                    fi = m_FoutInfoHash.take(infoRead);
                    fi.dspFoutChannel = chnnr;
                    m_FoutInfoHash[infoRead] = fi;
                    emit activationContinue();
                }
                else
                {
                    emit errMsg((tr(readdspchannelErrMsg)));
                    emit activationError();
                }
                break;
            break;
            }

            case readsourceformfactor:
            {
                double ffac;
                cFoutInfo fi;

                if (reply == ack)
                {
                    ffac = answer.toDouble(&ok);
                    fi = m_FoutInfoHash.take(infoRead);
                    fi.formFactor = ffac;
                    m_FoutInfoHash[infoRead] = fi;
                    emit activationContinue();
                }
                else
                {
                    emit errMsg((tr(readFormFactorErrMsg)));
                    emit activationError();
                }
                break;
            }

            case writeparameter:
                if (reply == ack) // we ignore ack
                    ;
                else
                {
                    emit errMsg((tr(writedspmemoryErrMsg)));
                    emit executionError();
                }
                break;

            case deactivatedsp:
                if (reply == ack) // we only continue if resource manager acknowledges
                    emit deactivationContinue();
                else
                {
                    emit errMsg((tr(dspdeactiveErrMsg)));
                    emit deactivationError();
                }
                break;
            case freepgrmem:
                if (reply == ack) // we only continue if resource manager acknowledges
                    emit deactivationContinue();
                else
                {
                    emit errMsg((tr(freeresourceErrMsg)));
                    emit deactivationError();
                }
                break;

            case freeusermem:
            case freeresourcesource:
                if (reply == ack) // we only continue if resource manager acknowledges
                    emit deactivationContinue();
                else
                {
                    emit errMsg((tr(freeresourceErrMsg)));
                    emit deactivationError();
                }
                break;

            case unregisterrangenotifiers:
                if (reply == ack) // we only continue pcb server acknowledges
                    emit deactivationContinue();
                else
                {
                    emit errMsg((tr(unregisterpcbnotifierErrMsg)));
                    emit deactivationError();
                }
                break;

            case dataaquistion:
                if (reply == ack)
                    emit dataAquisitionContinue();
                else
                {
                    m_dataAcquisitionMachine.stop();
                    emit errMsg((tr(dataaquisitionErrMsg)));
                    emit executionError(); // but we send error message
                }
                break;
            }
        }
    }
}

cPower2ModuleConfigData *cPower2ModuleMeasProgram::getConfData()
{
    return qobject_cast<cPower2ModuleConfiguration*>(m_pConfiguration.get())->getConfigurationData();
}

void cPower2ModuleMeasProgram::setActualValuesNames()
{
    QString powIndicator = "123S";
    const cMeasModeInfo mminfo = MeasModeCatalog::getInfo(getConfData()->m_sMeasuringMode.m_sValue);
    for (int i = 0; i < 4; i++) {
        m_veinActValueList.at(i*3)->setChannelName(QString("+%1%2").arg(mminfo.getActvalName()).arg(powIndicator[i]));
        m_veinActValueList.at(i*3)->setUnit(mminfo.getUnitName());

        m_veinActValueList.at(i*3+1)->setChannelName(QString("-%1%2").arg(mminfo.getActvalName()).arg(powIndicator[i]));
        m_veinActValueList.at(i*3+1)->setUnit(mminfo.getUnitName());

        m_veinActValueList.at(i*3+2)->setChannelName(QString("%1%2").arg(mminfo.getActvalName()).arg(powIndicator[i]));
        m_veinActValueList.at(i*3+2)->setUnit(mminfo.getUnitName());
    }
    m_pModule->exportMetaData();
}

quint8 cPower2ModuleMeasProgram::cmpActualValIndex(freqoutconfiguration frconf)
{
    quint8 actvalueIndex = frconf.m_nSource * 3;

    switch (frconf.m_nFoutMode)
    {
    case absPower:
        actvalueIndex += 2;
        break;

    case negPower:
        actvalueIndex += 1;
        break;
    }

    return actvalueIndex;
}


void cPower2ModuleMeasProgram::setSCPIMeasInfo()
{
    cSCPIInfo* pSCPIInfo;

    for (int i = 0; i < 12; i++)
    {
        pSCPIInfo = new cSCPIInfo("MEASURE", m_veinActValueList.at(i)->getChannelName(), "8", m_veinActValueList.at(i)->getName(), "0", m_veinActValueList.at(i)->getUnit());
        m_veinActValueList.at(i)->setSCPIInfo(pSCPIInfo);
    }
}

void cPower2ModuleMeasProgram::setInterfaceActualValues(QVector<float> *actualValues)
{
    if (m_bActive) // maybe we are deactivating !!!!
    {
        for (int i = 0; i < 12; i++)
            m_veinActValueList.at(i)->setValue(QVariant((double)actualValues->at(i)));
    }
}


void cPower2ModuleMeasProgram::resourceManagerConnect()
{
    // as this is our entry point when activating the module, we do some initialization first
    m_measChannelInfoHash.clear(); // we build up a new channel info hash
    cMeasChannelInfo mi;
    mi.pcbServersocket = getConfData()->m_PCBServerSocket; // the default from configuration file
    for(auto &measSystem : getConfData()->m_sMeasSystemList) {
        QStringList sl = measSystem.split(',');
        for (int j = 0; j < sl.count(); j++) {
            QString s = sl.at(j);
            if (!m_measChannelInfoHash.contains(s)) // did we find a new measuring channel ?
                m_measChannelInfoHash[s] = mi; // then lets add it
        }
    }

    m_FoutInfoHash.clear();
    cFoutInfo fi;
    fi.pcbServersocket = getConfData()->m_PCBServerSocket; // the default from configuration file
    if (getConfData()->m_nFreqOutputCount > 0)
    {
        for (int i = 0; i < getConfData()->m_nFreqOutputCount; i++)
        {
            QString name = getConfData()->m_FreqOutputConfList.at(i).m_sName;
            if (!m_FoutInfoHash.contains(name))
                m_FoutInfoHash[name] = fi; //
        }
    }

    m_NotifierInfoHash.clear();

    // we have to instantiate a working resource manager interface
    // so first we try to get a connection to resource manager over proxy
    m_rmClient = Zera::Proxy::getInstance()->getConnectionSmart(getConfData()->m_RMSocket.m_sIP, getConfData()->m_RMSocket.m_nPort);
    // and then we set resource manager interface's connection
    m_rmInterface.setClientSmart(m_rmClient);
    m_resourceManagerConnectState.addTransition(m_rmClient.get(), &Zera::ProxyClient::connected, &m_IdentifyState);
    connect(&m_rmInterface, &AbstractServerInterface::serverAnswer, this, &cPower2ModuleMeasProgram::catchInterfaceAnswer);
    Zera::Proxy::getInstance()->startConnectionSmart(m_rmClient);
}


void cPower2ModuleMeasProgram::sendRMIdent()
{
    m_MsgNrCmdList[m_rmInterface.rmIdent(QString("Power2Module%1").arg(m_pModule->getModuleNr()))] = sendrmident;
}


void cPower2ModuleMeasProgram::readResourceTypes()
{
    m_MsgNrCmdList[m_rmInterface.getResourceTypes()] = readresourcetypes;
}


void cPower2ModuleMeasProgram::readResourceSense()
{
    m_MsgNrCmdList[m_rmInterface.getResources("SENSE")] = readresourcesense;
}


void cPower2ModuleMeasProgram::readResourceSenseInfos()
{
    infoReadList = m_measChannelInfoHash.keys(); // we have to read information for all channels in this list
    emit activationContinue();
}


void cPower2ModuleMeasProgram::readResourceSenseInfo()
{
    infoRead = infoReadList.takeLast();
    m_MsgNrCmdList[m_rmInterface.getResourceInfo("SENSE", infoRead)] = readresourcesenseinfo;
}


void cPower2ModuleMeasProgram::readResourceSenseInfoDone()
{
    if (infoReadList.isEmpty())
        emit activationContinue();
    else
        emit activationLoop();
}


void cPower2ModuleMeasProgram::readResourceSource()
{
    if (getConfData()->m_nFreqOutputCount > 0) // do we have any frequency output
        m_MsgNrCmdList[m_rmInterface.getResources("SOURCE")] = readresourcesource; // then we read the needed info
    else
        emit activationSkip(); // otherwise we will skip
}

void cPower2ModuleMeasProgram::claimResourcesSource()
{
    infoReadList = m_FoutInfoHash.keys(); // we have to read information for all freq outputs in this list
    emit activationContinue();
}


void cPower2ModuleMeasProgram::claimResourceSource()
{
    infoRead = infoReadList.takeLast();
    m_MsgNrCmdList[m_rmInterface.setResource("SOURCE", infoRead, 1)] = claimresourcesource;
}


void cPower2ModuleMeasProgram::claimResourceSourceDone()
{
    if (infoReadList.isEmpty())
        emit activationContinue();
    else
        emit activationLoop();
}


void cPower2ModuleMeasProgram::readResourceSourceInfos()
{
    infoReadList = m_FoutInfoHash.keys(); // we have to read information for all freq outputs in this list
    emit activationContinue();
}


void cPower2ModuleMeasProgram::readResourceSourceInfo()
{
    infoRead = infoReadList.takeLast();
    m_MsgNrCmdList[m_rmInterface.getResourceInfo("SOURCE", infoRead)] = readresourcesourceinfo;
}


void cPower2ModuleMeasProgram::readResourceSourceInfoDone()
{
    if (infoReadList.isEmpty())
        emit activationContinue();
    else
        emit activationLoop();
}


void cPower2ModuleMeasProgram::pcbserverConnect4measChannels()
{
    // we have to connect to all ports....
    infoReadList = m_measChannelInfoHash.keys(); // so first we look for our different pcb sockets for sense
    m_nConnectionCount = infoReadList.count();

    for (int i = 0; i < infoReadList.count(); i++)
    {
        QString key = infoReadList.at(i);
        cMeasChannelInfo mi = m_measChannelInfoHash.take(key);
        cSocket sock = mi.pcbServersocket;
        Zera::ProxyClient* pcbClient = Zera::Proxy::getInstance()->getConnection(sock.m_sIP, sock.m_nPort);
        m_pcbClientList.append(pcbClient);
        Zera::cPCBInterface* pcbIFace = new Zera::cPCBInterface();
        m_pcbIFaceList.append(pcbIFace);
        pcbIFace->setClient(pcbClient);
        mi.pcbIFace = pcbIFace;
        m_measChannelInfoHash[key] = mi;
        connect(pcbClient, &Zera::ProxyClient::connected, this, &cPower2ModuleMeasProgram::monitorConnection); // here we wait until all connections are established
        connect(pcbIFace, &AbstractServerInterface::serverAnswer, this, &cPower2ModuleMeasProgram::catchInterfaceAnswer);
        Zera::Proxy::getInstance()->startConnection(pcbClient);
    }
}


void cPower2ModuleMeasProgram::pcbserverConnect4freqChannels()
{
    infoReadList = m_FoutInfoHash.keys(); // and then  we look for our different pcb sockets for source
    if (infoReadList.count() > 0)
    {
        m_nConnectionCount += infoReadList.count();
        for (int i = 0; i < infoReadList.count(); i++)
        {
            QString key = infoReadList.at(i);
            cFoutInfo fi = m_FoutInfoHash.take(key);
            cSocket sock = fi.pcbServersocket;
            Zera::ProxyClient* pcbClient = Zera::Proxy::getInstance()->getConnection(sock.m_sIP, sock.m_nPort);
            m_pcbClientList.append(pcbClient);
            Zera::cPCBInterface* pcbIFace = new Zera::cPCBInterface();
            m_pcbIFaceList.append(pcbIFace);
            pcbIFace->setClient(pcbClient);
            fi.pcbIFace = pcbIFace;
            fi.name = key;
            m_FoutInfoHash[key] = fi;
            connect(pcbClient, &Zera::ProxyClient::connected, this, &cPower2ModuleMeasProgram::monitorConnection); // here we wait until all connections are established
            connect(pcbIFace, &AbstractServerInterface::serverAnswer, this, &cPower2ModuleMeasProgram::catchInterfaceAnswer);
            Zera::Proxy::getInstance()->startConnection(pcbClient);
        }
    }
    else
        emit activationContinue();
}


void cPower2ModuleMeasProgram::readSampleRate()
{
    // we always take the sample count from the first channels pcb server
    m_MsgNrCmdList[m_pcbIFaceList.at(0)->getSampleRate()] = readsamplerate;
}


void cPower2ModuleMeasProgram::readSenseChannelInformation()
{
    infoReadList = m_measChannelInfoHash.keys(); // we have to read information for all channels in this list
    emit activationContinue();
}


void cPower2ModuleMeasProgram::readSenseChannelAlias()
{
    infoRead = infoReadList.takeFirst();
    m_MsgNrCmdList[m_measChannelInfoHash[infoRead].pcbIFace->getAlias(infoRead)] = readsensechannelalias;
}


void cPower2ModuleMeasProgram::readSenseChannelUnit()
{
    m_MsgNrCmdList[m_measChannelInfoHash[infoRead].pcbIFace->getUnit(infoRead)] = readsensechannelunit;
}


void cPower2ModuleMeasProgram::readSenseDspChannel()
{
    m_MsgNrCmdList[m_measChannelInfoHash[infoRead].pcbIFace->getDSPChannel(infoRead)] = readsensechanneldspchannel;
}


void cPower2ModuleMeasProgram::readSenseChannelInformationDone()
{
    if (infoReadList.isEmpty())
        emit activationContinue();
    else
        emit activationLoop();
}


void cPower2ModuleMeasProgram::readSourceChannelInformation()
{
    if (getConfData()->m_nFreqOutputCount > 0) // we only have to read information if really configured
    {
        infoReadList = m_FoutInfoHash.keys(); // we have to read information for all channels in this list
        emit activationContinue();
    }
    else
        emit activationSkip();
}


void cPower2ModuleMeasProgram::readSourceChannelAlias()
{
    infoRead = infoReadList.takeFirst();
    m_MsgNrCmdList[m_FoutInfoHash[infoRead].pcbIFace->getAliasSource(infoRead)] = readsourcechannelalias;
}


void cPower2ModuleMeasProgram::readSourceDspChannel()
{
    m_MsgNrCmdList[m_FoutInfoHash[infoRead].pcbIFace->getDSPChannelSource(infoRead)] = readsourcechanneldspchannel;
}


void cPower2ModuleMeasProgram::readSourceFormFactor()
{
    m_MsgNrCmdList[m_FoutInfoHash[infoRead].pcbIFace->getFormFactorSource(infoRead)] = readsourceformfactor;
}


void cPower2ModuleMeasProgram::readSourceChannelInformationDone()
{
    if (infoReadList.isEmpty())
        emit activationContinue();
    else
        emit activationLoop();
}


void cPower2ModuleMeasProgram::setSenseChannelRangeNotifiers()
{
    m_notifierNr = irqNr;
    infoReadList = m_measChannelInfoHash.keys(); // we have to set notifier for each channel we are working on
    emit activationContinue();
}


void cPower2ModuleMeasProgram::setSenseChannelRangeNotifier()
{
    infoRead = infoReadList.takeFirst();
    m_notifierNr++;
    // we will get irq+1 .. irq+6 for notification if ranges change
    m_MsgNrCmdList[ m_measChannelInfoHash[infoRead].pcbIFace->registerNotifier(QString("sens:%1:rang?").arg(infoRead), m_notifierNr)] = setchannelrangenotifier;

}


void cPower2ModuleMeasProgram::setSenseChannelRangeNotifierDone()
{
    if (infoReadList.isEmpty())
        emit activationContinue();
    else
        emit activationLoop();
}


void cPower2ModuleMeasProgram::dspserverConnect()
{
    m_pDspClient = Zera::Proxy::getInstance()->getConnection(getConfData()->m_DSPServerSocket.m_sIP, getConfData()->m_DSPServerSocket.m_nPort);
    m_pDSPInterFace->setClient(m_pDspClient);
    m_dspserverConnectState.addTransition(m_pDspClient, &Zera::ProxyClient::connected, &m_claimPGRMemState);
    connect(m_pDSPInterFace, &Zera::cDSPInterface::serverAnswer, this, &cPower2ModuleMeasProgram::catchInterfaceAnswer);
    Zera::Proxy::getInstance()->startConnection(m_pDspClient);
}


void cPower2ModuleMeasProgram::claimPGRMem()
{
    setDspVarList(); // first we set the var list for our dsp
    setDspCmdList(); // and the cmd list he has to work on
    m_MsgNrCmdList[m_rmInterface.setResource("DSP1", "PGRMEMC", m_pDSPInterFace->cmdListCount())] = claimpgrmem;
}


void cPower2ModuleMeasProgram::claimUSERMem()
{
   m_MsgNrCmdList[m_rmInterface.setResource("DSP1", "USERMEM", m_nDspMemUsed)] = claimusermem;
}


void cPower2ModuleMeasProgram::varList2DSP()
{
    m_MsgNrCmdList[m_pDSPInterFace->varList2Dsp()] = varlist2dsp;
}


void cPower2ModuleMeasProgram::cmdList2DSP()
{
    m_MsgNrCmdList[m_pDSPInterFace->cmdList2Dsp()] = cmdlist2dsp;
}


void cPower2ModuleMeasProgram::activateDSP()
{
    m_MsgNrCmdList[m_pDSPInterFace->activateInterface()] = activatedsp; // aktiviert die var- und cmd-listen im dsp
}


void cPower2ModuleMeasProgram::activateDSPdone()
{
    m_bActive = true;

    setActualValuesNames();
    setSCPIMeasInfo();
    m_pMeasureSignal->setValue(QVariant(1));

    if (getConfData()->m_sIntegrationMode == "time")
        connect(m_pIntegrationParameter, &VfModuleComponent::sigValueChanged, this, &cPower2ModuleMeasProgram::newIntegrationtime);
    else
        connect(m_pIntegrationParameter, &VfModuleComponent::sigValueChanged, this, &cPower2ModuleMeasProgram::newIntegrationPeriod);
    connect(m_pMeasuringmodeParameter, &VfModuleComponent::sigValueChanged, this , &cPower2ModuleMeasProgram::newMeasMode);
    //connect(m_pMModePhaseSelectParameter, &VfModuleComponent::sigValueChanged, this, &cPower2ModuleMeasProgram::newPhaseList);
    connect(&m_measModeSelector, &MeasModeSelector::sigTransactionOk,
            this, &cPower2ModuleMeasProgram::onModeTransactionOk);
    readUrvalueList = m_measChannelInfoHash.keys(); // once we read all actual range urvalues
    if (!m_readUpperRangeValueMachine.isRunning() && !m_pModule->m_demo)
        m_readUpperRangeValueMachine.start();

    emit activated();
}


void cPower2ModuleMeasProgram::deactivateDSP()
{
    m_bActive = false;
    m_MsgNrCmdList[m_pDSPInterFace->deactivateInterface()] = deactivatedsp; // wat wohl
}


void cPower2ModuleMeasProgram::freePGRMem()
{
    Zera::Proxy::getInstance()->releaseConnection(m_pDspClient);
    deleteDspVarList();
    deleteDspCmdList();

    m_MsgNrCmdList[m_rmInterface.freeResource("DSP1", "PGRMEMC")] = freepgrmem;
}


void cPower2ModuleMeasProgram::freeUSERMem()
{
    m_MsgNrCmdList[m_rmInterface.freeResource("DSP1", "USERMEM")] = freeusermem;
}


void cPower2ModuleMeasProgram::freeFreqOutputs()
{
    if (getConfData()->m_nFreqOutputCount > 0) // we only have to read information if really configured
    {
        infoReadList = m_FoutInfoHash.keys(); // we have to read information for all channels in this list
        emit deactivationContinue();
    }
    else
        emit deactivationSkip();
}


void cPower2ModuleMeasProgram::freeFreqOutput()
{
    infoRead = infoReadList.takeFirst();
    m_MsgNrCmdList[m_rmInterface.freeResource("SOURCE", infoRead)] = freeresourcesource;
}


void cPower2ModuleMeasProgram::freeFreqOutputDone()
{
    if (infoReadList.isEmpty())
        emit deactivationContinue();
    else
        emit deactivationLoop();
}


void cPower2ModuleMeasProgram::resetNotifiers()
{
    infoReadList = m_measChannelInfoHash.keys();
    emit deactivationContinue();
}


void cPower2ModuleMeasProgram::resetNotifier()
{
    infoRead = infoReadList.takeFirst();
    m_MsgNrCmdList[m_measChannelInfoHash[infoRead].pcbIFace->unregisterNotifiers()] = unregisterrangenotifiers;
}


void cPower2ModuleMeasProgram::resetNotifierDone()
{
    if (infoReadList.isEmpty())
        emit deactivationContinue();
    else
        emit deactivationLoop();
}


void cPower2ModuleMeasProgram::deactivateDSPdone()
{
    if (m_pcbIFaceList.count() > 0)
    {
        for (int i = 0; i < m_pcbIFaceList.count(); i++)
        {
            Zera::Proxy::getInstance()->releaseConnection(m_pcbClientList.at(i));
            delete m_pcbIFaceList.at(i);
        }

        m_pcbClientList.clear();
        m_pcbIFaceList.clear();
    }

    disconnect(&m_rmInterface, 0, this, 0);
    disconnect(m_pDSPInterFace, 0, this, 0);

    emit deactivated();
}


void cPower2ModuleMeasProgram::dataAcquisitionDSP()
{
    m_pMeasureSignal->setValue(QVariant(0));
    m_MsgNrCmdList[m_pDSPInterFace->dataAcquisition(m_pActualValuesDSP)] = dataaquistion; // we start our data aquisition now
}


void cPower2ModuleMeasProgram::dataReadDSP()
{
    if (m_bActive)
    {
        m_pDSPInterFace->getData(m_pActualValuesDSP, m_ModuleActualValues); // we fetch our actual values
        emit actualValues(&m_ModuleActualValues); // and send them
        m_pMeasureSignal->setValue(QVariant(1)); // signal measuring
    }
}


void cPower2ModuleMeasProgram::readUrvalue()
{
    readUrvalueInfo = readUrvalueList.takeFirst(); // we have the channel information now
    m_MsgNrCmdList[m_measChannelInfoHash[readUrvalueInfo].pcbIFace->getUrvalue(readUrvalueInfo)] = readurvalue;
}


void cPower2ModuleMeasProgram::readUrvalueDone()
{
    if (readUrvalueList.isEmpty())
        emit activationContinue();
    else
        emit activationLoop();
}

cPower2ModuleMeasProgram::RangeMaxVals cPower2ModuleMeasProgram::calcMaxRangeValues(std::shared_ptr<MeasMode> mode)
{
    RangeMaxVals maxVals;
    for (int i = 0; i < getConfData()->m_sMeasSystemList.count(); i++) {
        if(mode->isPhaseActive(i)) {
            QStringList sl = getConfData()->m_sMeasSystemList.at(i).split(',');
            double rangeFullVal = m_measChannelInfoHash[sl.at(0)].m_fUrValue;
            if (rangeFullVal > maxVals.maxU)
                maxVals.maxU = rangeFullVal;
            rangeFullVal = m_measChannelInfoHash[sl.at(1)].m_fUrValue;
            if (rangeFullVal > maxVals.maxI)
                maxVals.maxI = rangeFullVal;
        }
    }
    return maxVals;
}

void cPower2ModuleMeasProgram::foutParamsToDsp()
{
    std::shared_ptr<MeasMode> mode = m_measModeSelector.getCurrMode();
    RangeMaxVals maxVals = calcMaxRangeValues(mode);
    double cfak = mode->getActiveMeasSysCount();
    double constantImpulsePerWs = getConfData()->m_nNominalFrequency / (cfak * maxVals.maxU * maxVals.maxI);
    bool isValidConstant = !isinf(constantImpulsePerWs);
    if (isValidConstant && getConfData()->m_nFreqOutputCount > 0) { // dsp-interface will crash for missing parts...
        QString datalist = "FREQSCALE:";
        for (int i = 0; i<getConfData()->m_nFreqOutputCount; i++) {
            double frScale;
            cFoutInfo fi = m_FoutInfoHash[getConfData()->m_FreqOutputConfList.at(i).m_sName];
            frScale = fi.formFactor * constantImpulsePerWs;

            /*if(m_pScalingInputs.length() > i){
                if(m_pScalingInputs.at(i).first != nullptr && m_pScalingInputs.at(i).second != nullptr){
                    double scale=m_pScalingInputs.at(i).first->value().toDouble()*m_pScalingInputs.at(i).second->value().toDouble();
                    frScale=frScale*scale;
                }
            }*/
            datalist += QString("%1,").arg(frScale, 0, 'g', 7);
        }
        datalist.resize(datalist.size()-1);
        datalist += ";";
        m_pDSPInterFace->setVarData(m_pfreqScaleDSP, datalist);
        m_MsgNrCmdList[m_pDSPInterFace->dspMemoryWrite(m_pfreqScaleDSP)] = writeparameter;
    }
    setFoutPowerModes();
    double constantImpulsePerKwh = 3600.0 * 1000.0 * constantImpulsePerWs; // imp./kwh
    for (int i = 0; i < getConfData()->m_nFreqOutputCount; i++) {
        // calculate prescaling factor for Fout
        /*if(m_pScalingInputs.length() > i){
            if(m_pScalingInputs.at(i).first != nullptr && m_pScalingInputs.at(i).second != nullptr){
                double scale = m_pScalingInputs.at(i).first->value().toDouble() * m_pScalingInputs.at(i).second->value().toDouble();
                constantImpulsePerKwh = constantImpulsePerKwh * scale;
            }
        }*/
        QString key = getConfData()->m_FreqOutputConfList.at(i).m_sName;
        cFoutInfo fi = m_FoutInfoHash[key];
        if(isValidConstant) {
            m_MsgNrCmdList[fi.pcbIFace->setConstantSource(fi.name, constantImpulsePerKwh)] = writeparameter;
            //m_FoutConstParameterList.at(i)->setValue(constantImpulsePerKwh);
        }
        /*else
            m_FoutConstParameterList.at(i)->setValue(0.0);*/
    }

    double pmax = maxVals.maxU * maxVals.maxI; // MQREF
    QString datalist = QString("NOMPOWER:%1;").arg(pmax, 0, 'g', 7);
    m_pDSPInterFace->setVarData(m_pNomPower, datalist);
    m_MsgNrCmdList[m_pDSPInterFace->dspMemoryWrite(m_pNomPower)] = setqrefnominalpower;
}

void cPower2ModuleMeasProgram::setFoutPowerModes()
{
    QList<QString> keylist = m_FoutInfoHash.keys();
    for (int i = 0; i < keylist.count(); i++) {
        QString powtype;
        int foutmode = getConfData()->m_FreqOutputConfList.at(i).m_nFoutMode;
        switch (foutmode)
        {
        case posPower:
            powtype = "+";
            break;
        case negPower:
            powtype = "-";
            break;
        default:
            powtype = "";
        }
        powtype += MeasModeCatalog::getInfo(getConfData()->m_sMeasuringMode.m_sValue).getActvalName();
        cFoutInfo fi = m_FoutInfoHash[keylist.at(i)];
        m_MsgNrCmdList[fi.pcbIFace->setPowTypeSource(fi.name, powtype)] = writeparameter;
    }
}

void cPower2ModuleMeasProgram::dspSetParamsTiMModePhase(int tiTimeOrPeriods)
{
    QString strVarData = QString("TIPAR:%1;TISTART:0;MMODE:%2")
                             .arg(tiTimeOrPeriods)
                             .arg(m_measModeSelector.getCurrMode()->getDspSelectCode());
    /*QString phaseVarSet = dspGetSetPhasesVar();
    if(!phaseVarSet.isEmpty())
        strVarData += ";" + dspGetSetPhasesVar();*/
    m_pDSPInterFace->setVarData(m_pParameterDSP, strVarData, DSPDATA::dInt);
    m_MsgNrCmdList[m_pDSPInterFace->dspMemoryWrite(m_pParameterDSP)] = writeparameter;
}

void cPower2ModuleMeasProgram::handleMModeParamChange()
{
    dspSetParamsTiMModePhase(calcTiTime());
    emit m_pModule->parameterChanged();
}

void cPower2ModuleMeasProgram::updatesForMModeChange()
{
    setActualValuesNames();
    foutParamsToDsp();
}

void cPower2ModuleMeasProgram::handleMovingWindowIntTimeChange()
{
    m_movingwindowFilter.setIntegrationtime(getConfData()->m_fMeasIntervalTime.m_fValue);
    emit m_pModule->parameterChanged();
}

void cPower2ModuleMeasProgram::newIntegrationtime(QVariant ti)
{
    getConfData()->m_fMeasIntervalTime.m_fValue = ti.toDouble();
    if (getConfData()->m_bmovingWindow)
        handleMovingWindowIntTimeChange();
    else
        handleMModeParamChange(); // is this ever reached?
}

void cPower2ModuleMeasProgram::newIntegrationPeriod(QVariant period)
{
    // there is no moving window for period
    getConfData()->m_nMeasIntervalPeriod.m_nValue = period.toInt();
    handleMModeParamChange();
}

void cPower2ModuleMeasProgram::newMeasMode(QVariant mm)
{
    m_measModeSelector.tryChangeMode(mm.toString());
}

double cPower2ModuleMeasProgram::calcTiTime()
{
    double tiTime;
    if (getConfData()->m_sIntegrationMode == "time") {
        if (getConfData()->m_bmovingWindow)
            tiTime = getConfData()->m_fmovingwindowInterval*1000.0;
        else
            tiTime = getConfData()->m_fMeasIntervalTime.m_fValue*1000.0;
    }
    else // period (just if/else for now)
        tiTime = getConfData()->m_nMeasIntervalPeriod.m_nValue;
    return tiTime;
}

void cPower2ModuleMeasProgram::setPhaseMaskValidator(std::shared_ptr<MeasMode> mode)
{
    QStringList allPhaseMasks = VeinValidatorPhaseStringGenerator::generate(getConfData()->m_sMeasSystemList.count());
    QStringList allowedPhaseMasks;
    for(auto &mask : allPhaseMasks)
        if(mode->canChangeMask(mask))
            allowedPhaseMasks.append(mask);
    //m_MModePhaseSelectValidator->setValidator(allowedPhaseMasks);
    m_pModule->exportMetaData();
}

void cPower2ModuleMeasProgram::updatePhaseMaskVeinComponents(std::shared_ptr<MeasMode> mode)
{
    QString newPhaseMask = mode->getCurrentMask();
    //m_pMModePhaseSelectParameter->setValue(newPhaseMask);
    setPhaseMaskValidator(mode);
}

void cPower2ModuleMeasProgram::onModeTransactionOk()
{
    std::shared_ptr<MeasMode> mode = m_measModeSelector.getCurrMode();
    updatePhaseMaskVeinComponents(mode);
    QString newMeasMode = mode->getName();
    getConfData()->m_sMeasuringMode.m_sValue = newMeasMode;
    handleMModeParamChange();
    updatesForMModeChange();
}

void cPower2ModuleMeasProgram::handleDemoActualValues()
{
    QVector<float> valuesDemo;
    int current = 10;
    int voltage = 230;
    for(int phase=0; phase<MeasPhaseCount; phase++) {
        double val = 0.0;
        double randPlusMinusOne = 2.0 * (double)rand() / RAND_MAX - 1.0;
        val = current*voltage + randPlusMinusOne;
        valuesDemo.append(val);
    }
    valuesDemo.append(valuesDemo[0] + valuesDemo[1] + valuesDemo[2]);
    Q_ASSERT(valuesDemo.size() == m_ModuleActualValues.size());
    m_ModuleActualValues = valuesDemo;
    emit actualValues(&m_ModuleActualValues);
}

}
