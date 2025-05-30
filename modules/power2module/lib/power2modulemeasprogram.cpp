#include "power2modulemeasprogram.h"
#include "power2module.h"
#include "power2moduleconfiguration.h"
#include "power2dspcmdgenerator.h"
#include "measmodephasesetstrategy4wire.h"
#include "veinvalidatorphasestringgenerator.h"
#include <measmodecatalog.h>
#include <scpi.h>
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

cPower2ModuleMeasProgram::cPower2ModuleMeasProgram(cPower2Module* module, std::shared_ptr<BaseModuleConfiguration> pConfiguration) :
    cBaseDspMeasProgram(pConfiguration, module->getVeinModuleName()),
    m_pModule(module)
{
    m_dspInterface = m_pModule->getServiceInterfaceFactory()->createDspInterfacePower2(
        m_pModule->getEntityId(),
        &m_measModeSelector);

    m_IdentifyState.addTransition(this, &cPower2ModuleMeasProgram::activationContinue, &m_claimResourcesSourceState);

    m_claimResourcesSourceState.addTransition(this, &cPower2ModuleMeasProgram::activationContinue, &m_claimResourceSourceState);
    m_claimResourcesSourceState.addTransition(this, &cPower2ModuleMeasProgram::activationSkip, &m_pcbserverConnectState4measChannels);
    m_claimResourceSourceState.addTransition(this, &cPower2ModuleMeasProgram::activationContinue, &m_claimResourceSourceDoneState);
    m_claimResourceSourceDoneState.addTransition(this, &cPower2ModuleMeasProgram::activationContinue, &m_pcbserverConnectState4measChannels);
    m_claimResourceSourceDoneState.addTransition(this, &cPower2ModuleMeasProgram::activationLoop, &m_claimResourceSourceState);

    m_pcbserverConnectState4measChannels.addTransition(this, &cPower2ModuleMeasProgram::activationContinue, &m_pcbserverConnectState4freqChannels);
    m_pcbserverConnectState4freqChannels.addTransition(this, &cPower2ModuleMeasProgram::activationContinue, &m_readSourceChannelInformationState);

    m_readSourceChannelInformationState.addTransition(this, &cPower2ModuleMeasProgram::activationContinue, &m_readSourceChannelAliasState);
    m_readSourceChannelInformationState.addTransition(this, &cPower2ModuleMeasProgram::activationSkip, &m_dspserverConnectState);
    m_readSourceChannelAliasState.addTransition(this, &cPower2ModuleMeasProgram::activationContinue, &m_readSourceDspChannelState);
    m_readSourceDspChannelState.addTransition(this, &cPower2ModuleMeasProgram::activationContinue, &m_readSourceFormFactorState);
    m_readSourceFormFactorState.addTransition(this, &cPower2ModuleMeasProgram::activationContinue, &m_readSourceChannelInformationDoneState);
    m_readSourceChannelInformationDoneState.addTransition(this, &cPower2ModuleMeasProgram::activationLoop, &m_readSourceChannelAliasState);

    m_readSourceChannelInformationDoneState.addTransition(this, &cPower2ModuleMeasProgram::activationContinue, &m_setSenseChannelRangeNotifiersState);
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

    m_activationMachine.addState(&m_claimResourcesSourceState);
    m_activationMachine.addState(&m_claimResourceSourceState);
    m_activationMachine.addState(&m_claimResourceSourceDoneState);
    m_activationMachine.addState(&m_pcbserverConnectState4measChannels);
    m_activationMachine.addState(&m_pcbserverConnectState4freqChannels);

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

    m_activationMachine.setInitialState(&m_resourceManagerConnectState);

    connect(&m_resourceManagerConnectState, &QAbstractState::entered, this, &cPower2ModuleMeasProgram::resourceManagerConnect);
    connect(&m_IdentifyState, &QAbstractState::entered, this, &cPower2ModuleMeasProgram::sendRMIdent);

    connect(&m_dspserverConnectState, &QAbstractState::entered, this, &cPower2ModuleMeasProgram::dspserverConnect);
    connect(&m_claimResourcesSourceState, &QAbstractState::entered, this, &cPower2ModuleMeasProgram::claimResourcesSource);
    connect(&m_claimResourceSourceState, &QAbstractState::entered, this, &cPower2ModuleMeasProgram::claimResourceSource);
    connect(&m_claimResourceSourceDoneState, &QAbstractState::entered, this, &cPower2ModuleMeasProgram::claimResourceSourceDone);

    connect(&m_pcbserverConnectState4measChannels, &QAbstractState::entered, this, &cPower2ModuleMeasProgram::pcbserverConnect4measChannels);
    connect(&m_pcbserverConnectState4freqChannels, &QAbstractState::entered, this, &cPower2ModuleMeasProgram::pcbserverConnect4freqChannels);

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

    m_deactivationMachine.addState(&m_freePGRMemState);
    m_deactivationMachine.addState(&m_freeUSERMemState);

    m_deactivationMachine.addState(&m_freeFreqOutputsState);
    m_deactivationMachine.addState(&m_freeFreqOutputState);
    m_deactivationMachine.addState(&m_freeFreqOutputDoneState);

    m_deactivationMachine.addState(&m_resetNotifiersState);
    m_deactivationMachine.addState(&m_resetNotifierState);
    m_deactivationMachine.addState(&m_resetNotifierDoneState);

    m_deactivationMachine.addState(&m_unloadDSPDoneState);

    m_deactivationMachine.setInitialState(&m_freePGRMemState);

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


void cPower2ModuleMeasProgram::generateVeinInterface()
{
    QString key;
    VfModuleComponent *pActvalue;
    for (int i = 0; i < MeasPhaseCount+SumValueCount; i++) {
        pActvalue = new VfModuleComponent(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                            QString("ACT_PP%1").arg(i+1),
                                            QString("Actual value positive power"));
        m_veinActValueList.append(pActvalue); // we add the component for our measurement
        m_pModule->veinModuleActvalueList.append(pActvalue); // and for the modules interface

        pActvalue = new VfModuleComponent(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                            QString("ACT_PM%1").arg(i+1),
                                            QString("Actual value negative power"));
        m_veinActValueList.append(pActvalue); // we add the component for our measurement
        m_pModule->veinModuleActvalueList.append(pActvalue); // and for the modules interface

        QString strDescription;
        if(i<MeasPhaseCount)
            strDescription = QString("Actual power value phase %1").arg(i+1);
        else
            strDescription = QString("Actual power value sum all phases");
        pActvalue = new VfModuleComponent(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                            QString("ACT_P%1").arg(i+1),
                                            strDescription);
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
    m_pMeasuringmodeParameter = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                                         key = QString("PAR_MeasuringMode"),
                                                         QString("Measuring mode"),
                                                         QVariant(getConfData()->m_sMeasuringMode.m_sValue));

    m_pMeasuringmodeParameter->setScpiInfo("CONFIGURATION","MMODE", SCPI::isQuery|SCPI::isCmdwP, "PAR_MeasuringMode");
    cStringValidator *sValidator = new cStringValidator(getConfData()->m_sMeasmodeList);
    m_pMeasuringmodeParameter->setValidator(sValidator);
    m_pModule->m_veinModuleParameterMap[key] = m_pMeasuringmodeParameter; // for modules use

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

    m_pIntegrationParameter = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                                       key = QString("PAR_Interval"),
                                                       s,
                                                       val);
    m_pIntegrationParameter->setUnit(unit);
    if (btime) {
        m_pIntegrationParameter->setScpiInfo("CONFIGURATION","TINTEGRATION", SCPI::isQuery|SCPI::isCmdwP, "PAR_Interval");
        m_pIntegrationParameter->setValidator(new cDoubleValidator(1.0, 100.0, 0.5));
    }
    else {
        m_pIntegrationParameter->setScpiInfo("CONFIGURATION","TPERIOD", SCPI::isQuery|SCPI::isCmdwP, "PAR_Interval");
        m_pIntegrationParameter->setValidator(new cIntValidator(5, 5000, 1));
    }
    m_pModule->m_veinModuleParameterMap[key] = m_pIntegrationParameter; // for modules use

    m_pMeasureSignal = new VfModuleComponent(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                                QString("SIG_Measuring"),
                                                QString("Signal indicating measurement activity"),
                                                QVariant(0));

    m_pModule->veinModuleComponentList.append(m_pMeasureSignal);
}


void cPower2ModuleMeasProgram::setDspVarList()
{
    int samples = m_pModule->getSharedChannelRangeObserver()->getSamplesPerPeriod();
    // we fetch a handle for sampled data and other temporary values
    m_pTmpDataDsp = m_dspInterface->getMemHandle("TmpData");
    m_pTmpDataDsp->addDspVar("MEASSIGNAL1", samples, DSPDATA::vDspTemp); // we need 2 signals for our computations
    m_pTmpDataDsp->addDspVar("MEASSIGNAL2", samples, DSPDATA::vDspTemp); // we need 2 signals for our computations
    m_pTmpDataDsp->addDspVar("VALPOWER", 3*(MeasPhaseCount+SumValueCount), DSPDATA::vDspTemp); // p1+,p1-,p1, p2+,p2-,p2, p3+,p3-,p3, ps+,ps-,ps
    m_pTmpDataDsp->addDspVar("TEMP1", 2, DSPDATA::vDspTemp); // we need 2 temp. vars also for complex
    m_pTmpDataDsp->addDspVar("TEMP2", 2, DSPDATA::vDspTemp);
    m_pTmpDataDsp->addDspVar("FILTER", 2*3*(MeasPhaseCount+SumValueCount),DSPDATA::vDspTemp);
    m_pTmpDataDsp->addDspVar("N",1,DSPDATA::vDspTemp);

    // a handle for parameter
    m_pParameterDSP =  m_dspInterface->getMemHandle("Parameter");
    m_pParameterDSP->addDspVar("TIPAR",1, DSPDATA::vDspParam, DSPDATA::dInt); // integrationtime res = 1ms or period
    // we use tistart as parameter, so we can finish actual measuring interval bei setting 0
    m_pParameterDSP->addDspVar("TISTART",1, DSPDATA::vDspParam, DSPDATA::dInt);
    m_pParameterDSP->addDspVar("MMODE",1, DSPDATA::vDspParam, DSPDATA::dInt);

    // a handle for filtered actual values
    m_pActualValuesDSP = m_dspInterface->getMemHandle("ActualValues");
    m_pActualValuesDSP->addDspVar("VALPOWERF", 3*(MeasPhaseCount+SumValueCount), DSPDATA::vDspResult);

    // and one for the frequency output scale values, we need 1 value for each configured output
    m_pfreqScaleDSP = m_dspInterface->getMemHandle("FrequencyScale");
    m_pfreqScaleDSP->addDspVar("FREQSCALE", getConfData()->m_nFreqOutputCount, DSPDATA::vDspParam);

    // and one for nominal power in case that measuring mode is qref
    m_pNomPower = m_dspInterface->getMemHandle("QRefScale");
    m_pNomPower->addDspVar("NOMPOWER", 1, DSPDATA::vDspParam);

    m_ModuleActualValues.resize(m_pActualValuesDSP->getSize()); // we provide a vector for generated actual values
    m_nDspMemUsed = m_pTmpDataDsp->getSize() + m_pParameterDSP->getSize() + m_pActualValuesDSP->getSize();
}

QStringList cPower2ModuleMeasProgram::dspCmdInitVars(int dspInitialSelectCode)
{
    int samples = m_pModule->getSharedChannelRangeObserver()->getSamplesPerPeriod();
    QStringList dspCmdList;
    dspCmdList.append("STARTCHAIN(1,1,0x0101)"); // aktiv, prozessnr. (dummy),hauptkette 1 subkette 1 start
    dspCmdList.append(QString("CLEARN(%1,MEASSIGNAL1)").arg(samples) ); // clear meassignal
    dspCmdList.append(QString("CLEARN(%1,MEASSIGNAL2)").arg(samples) ); // clear meassignal
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
    ChannelRangeObserver::SystemObserverPtr observer = m_pModule->getSharedChannelRangeObserver();
    MeasSystemChannels measChannelPairList;
    cPower2ModuleConfigData *confdata = getConfData();
    for(const auto &measChannelPair : confdata->m_sMeasSystemList) {
        QStringList channelPairSplit = measChannelPair.split(',');
        MeasSystemChannel measChannel;
        measChannel.voltageChannel = observer->getChannel(channelPairSplit.at(0))->getDspChannel();
        measChannel.currentChannel = observer->getChannel(channelPairSplit.at(1))->getDspChannel();
        measChannelPairList.append(measChannel);
    }

    int measSytemCount = confdata->m_sMeasSystemList.count();
    //MeasModeBroker measBroker(Power2DspModeFunctionCatalog::get(measSytemCount));

    // we set up all our lists for wanted measuring modes, this gets much more performance
    int samples = m_pModule->getSharedChannelRangeObserver()->getSamplesPerPeriod();
    QStringList dspMModesCommandList;
    for (int i = 0; i < confdata->m_nMeasModeCount; i++) {
        cMeasModeInfo mInfo = MeasModeCatalog::getInfo(confdata->m_sMeasmodeList.at(i));
        measmodes measModeId = mInfo.getCode();
        switch (measModeId)
        {
        case m4lw:
            dspMModesCommandList.append(Power2DspCmdGenerator::getCmdsMMode4LW(measModeId, measChannelPairList, samples));
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
    m_dspInterface->addCycListItems(dspInitVarsList);
    m_dspInterface->addCycListItems(dspMModesCommandList);

    // so... let's now set our frequency outputs if he have some
    if (confdata->m_sFreqActualizationMode == "signalperiod") {
        if (confdata->m_nFreqOutputCount > 0) {
            for (int i = 0; i < confdata->m_nFreqOutputCount; i++) {
                // which actualvalue do we take as source (offset)
                quint8 actvalueIndex = cmpActualValIndex(confdata->m_FreqOutputConfList.at(i));

                QString foutSystemName =  confdata->m_FreqOutputConfList.at(i).m_sName;
                // here we set abs, plus or minus and which frequency output has to be set
                // we could also take absPower because we have dedicated values for + , - , abs

                quint16 freqpar = confdata->m_FreqOutputConfList.at(i).m_nFoutMode + (m_FoutInfoMap[foutSystemName].dspFoutChannel << 8);
                // frequenzausgang berechnen lassen
                m_dspInterface->addCycListItem(QString("CMPCLK(%1,VALPOWER+%2,FREQSCALE+%3)")
                                                 .arg(freqpar)
                                                 .arg(actvalueIndex)
                                                 .arg(i));
            }
        }
    }

    if (confdata->m_sIntegrationMode == "time") {
        m_dspInterface->addCycListItem("TESTTIMESKIPNEX(TISTART,TIPAR)");
        m_dspInterface->addCycListItem("ACTIVATECHAIN(1,0x0102)");

        m_dspInterface->addCycListItem("STARTCHAIN(0,1,0x0102)");
            m_dspInterface->addCycListItem("GETSTIME(TISTART)"); // set new system time
            m_dspInterface->addCycListItem(QString("CMPAVERAGE1(12,FILTER,VALPOWERF)"));
            m_dspInterface->addCycListItem(QString("CLEARN(%1,FILTER)").arg(2*12+1) );
            m_dspInterface->addCycListItem(QString("DSPINTTRIGGER(0x0,0x%1)").arg(irqNr)); // send interrupt to module

            if (confdata->m_sFreqActualizationMode == "integrationtime") {
                if (confdata->m_nFreqOutputCount > 0) {
                    for (int i = 0; i < confdata->m_nFreqOutputCount; i++) {
                        // which actualvalue do we take as source (offset)
                        quint8 actvalueIndex = cmpActualValIndex(confdata->m_FreqOutputConfList.at(i));

                        QString foutSystemName =  confdata->m_FreqOutputConfList.at(i).m_sName;
                        // here we set abs, plus or minus and which frequency output has to be set
                        // we could also take absPower because we have dedicated values for + , - , abs
                        quint16 freqpar = confdata->m_FreqOutputConfList.at(i).m_nFoutMode + (m_FoutInfoMap[foutSystemName].dspFoutChannel << 8);
                        // frequenzausgang berechnen lassen
                        m_dspInterface->addCycListItem(QString("CMPCLK(%1,VALPOWERF+%2,FREQSCALE+%3)")
                                                         .arg(freqpar)
                                                         .arg(actvalueIndex)
                                                         .arg(i));
                    }
                }
            }

            m_dspInterface->addCycListItem("DEACTIVATECHAIN(1,0x0102)");

        m_dspInterface->addCycListItem("STOPCHAIN(1,0x0102)"); // end processnr., mainchain 1 subchain 2
    }

    else { // otherwise it is period
        m_dspInterface->addCycListItem("TESTVVSKIPLT(N,TIPAR)");
        m_dspInterface->addCycListItem("ACTIVATECHAIN(1,0x0103)");
        m_dspInterface->addCycListItem("STARTCHAIN(0,1,0x0103)");
            m_dspInterface->addCycListItem("CMPAVERAGE1(12,FILTER,VALPOWERF)");
            m_dspInterface->addCycListItem(QString("CLEARN(%1,FILTER)").arg(2*12+1) );
            m_dspInterface->addCycListItem(QString("DSPINTTRIGGER(0x0,0x%1)").arg(irqNr)); // send interrupt to module

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
                        quint16 freqpar = confdata->m_FreqOutputConfList.at(i).m_nFoutMode + (m_FoutInfoMap[foutSystemName].dspFoutChannel << 8);
                        // frequenzausgang berechnen lassen
                        m_dspInterface->addCycListItem(QString("CMPCLK(%1,VALPOWERF+%2,FREQSCALE+%3)")
                                                         .arg(freqpar)
                                                         .arg(actvalueIndex)
                                                         .arg(i));
                    }
                }
            }

            m_dspInterface->addCycListItem("DEACTIVATECHAIN(1,0x0103)");

        m_dspInterface->addCycListItem("STOPCHAIN(1,0x0103)"); // end processnr., mainchain 1 subchain 2
    }
}


void cPower2ModuleMeasProgram::deleteDspCmdList()
{
    m_dspInterface->clearCmdList();
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
                if (reply == ack)
                    emit activationContinue();
                else
                    notifyError(rmidentErrMSG);
                break;

            case setchannelrangenotifier:
                if (reply == ack) { // we only continue pcb server acknowledges
                    m_NotifierInfoHash[m_notifierNr] = infoRead;
                    emit activationContinue();
                }
                else
                    notifyError(registerpcbnotifierErrMsg);
                break;

            case readurvalue:
                if (reply == ack) {
                    double urvalue = answer.toDouble(&ok);
                    cMeasChannelInfo mi = m_measChannelInfoHash.take(readUrvalueInfo);
                    mi.m_fUrValue = urvalue;
                    m_measChannelInfoHash[readUrvalueInfo] = mi;
                    emit activationContinue();
                }
                else
                    notifyError(readrangeurvalueErrMsg);
                break;

            case setqrefnominalpower:
                if (reply != ack) // we ignore ack
                    notifyError(writedspmemoryErrMsg);
                break;

            case claimpgrmem:
                if (reply == ack)
                    emit activationContinue();
                else
                    notifyError(claimresourceErrMsg);
                break;

            case claimusermem:
                if (reply == ack)
                    emit activationContinue();
                else
                    notifyError(claimresourceErrMsg);
                break;

            case varlist2dsp:
                if (reply == ack)
                    emit activationContinue();
                else
                    notifyError(dspvarlistwriteErrMsg);
                break;

            case cmdlist2dsp:
                if (reply == ack)
                    emit activationContinue();
                else
                    notifyError(dspcmdlistwriteErrMsg);
                break;

            case activatedsp:
                if (reply == ack)
                    emit activationContinue();
                else
                    notifyError(dspactiveErrMsg);
                break;

            case readresourcetypes:
                ok = false;
                if ((reply == ack) && (answer.toString().contains("SENSE"))) { // we need sense  at least
                    if (getConfData()->m_nFreqOutputCount > 0) {
                        if (answer.toString().contains("SOURCE")) // maybe we also need source
                            ok = true;
                    }
                    else
                        ok = true;
                }
                if (ok)
                    emit activationContinue();
                else
                    notifyError(resourcetypeErrMsg);
                break;

            case claimresourcesource:
                if (reply == ack)
                    emit activationContinue();
                else
                    notifyError(claimresourceErrMsg);
                break;

            case readsourcechannelalias:
                if (reply == ack) {
                    QString alias = answer.toString();
                    cFoutInfo fi = m_FoutInfoMap.take(infoRead);
                    fi.alias = alias;
                    m_FoutInfoMap[infoRead] = fi;
                    emit activationContinue();
                }
                else
                    notifyError(readaliasErrMsg);
                break;

            case readsourcechanneldspchannel:
                if (reply == ack) {
                    int chnnr = answer.toInt(&ok);
                    cFoutInfo fi = m_FoutInfoMap.take(infoRead);
                    fi.dspFoutChannel = chnnr;
                    m_FoutInfoMap[infoRead] = fi;
                    emit activationContinue();
                }
                else
                    notifyError(readdspchannelErrMsg);
                break;

            case readsourceformfactor:
                if (reply == ack) {
                    double ffac = answer.toDouble(&ok);
                    cFoutInfo fi = m_FoutInfoMap.take(infoRead);
                    fi.formFactor = ffac;
                    m_FoutInfoMap[infoRead] = fi;
                    emit activationContinue();
                }
                else
                    notifyError(readFormFactorErrMsg);
               break;

            case writeparameter:
                if (reply == ack) // we ignore ack
                    ;
                else
                    notifyError(writedspmemoryErrMsg);
                break;

            case deactivatedsp:
                if (reply == ack)
                    emit deactivationContinue();
                else
                    notifyError(dspdeactiveErrMsg);
                break;
            case freepgrmem:
                if (reply == ack)
                    emit deactivationContinue();
                else
                    notifyError(freeresourceErrMsg);
                break;

            case freeusermem:
            case freeresourcesource:
                if (reply == ack)
                    emit deactivationContinue();
                else
                    notifyError(freeresourceErrMsg);
                break;

            case unregisterrangenotifiers:
                if (reply == ack) // we only continue pcb server acknowledges
                    emit deactivationContinue();
                else
                    notifyError(unregisterpcbnotifierErrMsg);
                break;

            case dataaquistion:
                if (reply == ack)
                    emit dataAquisitionContinue();
                else {
                    m_dataAcquisitionMachine.stop();
                    notifyError(dataaquisitionErrMsg);
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
    for (int i = 0; i < 12; i++)
        m_veinActValueList.at(i)->setScpiInfo("MEASURE", m_veinActValueList.at(i)->getChannelName(), SCPI::isCmdwP, m_veinActValueList.at(i)->getName());
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
    for(auto &measSystem : getConfData()->m_sMeasSystemList) {
        QStringList sl = measSystem.split(',');
        for (int j = 0; j < sl.count(); j++) {
            QString s = sl.at(j);
            if (!m_measChannelInfoHash.contains(s)) // did we find a new measuring channel ?
                m_measChannelInfoHash[s] = mi; // then lets add it
        }
    }

    m_FoutInfoMap.clear();
    cFoutInfo fi;
    if (getConfData()->m_nFreqOutputCount > 0) {
        for (int i = 0; i < getConfData()->m_nFreqOutputCount; i++) {
            QString name = getConfData()->m_FreqOutputConfList.at(i).m_sName;
            if (!m_FoutInfoMap.contains(name))
                m_FoutInfoMap[name] = fi;
        }
    }

    m_NotifierInfoHash.clear();

    // we have to instantiate a working resource manager interface
    // so first we try to get a connection to resource manager over proxy
    m_rmClient = Zera::Proxy::getInstance()->getConnectionSmart(m_pModule->getNetworkConfig()->m_rmServiceConnectionInfo,
                                                                m_pModule->getNetworkConfig()->m_tcpNetworkFactory);
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

void cPower2ModuleMeasProgram::claimResourcesSource()
{
    infoReadList = m_FoutInfoMap.keys(); // we have to read information for all freq outputs in this list
    if(!infoReadList.isEmpty())
        emit activationContinue();
    else
        emit activationSkip();
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

void cPower2ModuleMeasProgram::pcbserverConnect4measChannels()
{
    // we have to connect to all ports....
    infoReadList = m_measChannelInfoHash.keys(); // so first we look for our different pcb sockets for sense
    m_nConnectionCount = infoReadList.count();

    for (int i = 0; i < infoReadList.count(); i++) {
        QString key = infoReadList.at(i);
        cMeasChannelInfo mi = m_measChannelInfoHash.take(key);
        Zera::ProxyClientPtr pcbClient = Zera::Proxy::getInstance()->getConnectionSmart(
            m_pModule->getNetworkConfig()->m_pcbServiceConnectionInfo,
            m_pModule->getNetworkConfig()->m_tcpNetworkFactory);
        Zera::PcbInterfacePtr pcbInterface = std::make_shared<Zera::cPCBInterface>();
        pcbInterface->setClientSmart(pcbClient);
        mi.pcbIFace = pcbInterface;
        m_measChannelInfoHash[key] = mi;
        connect(pcbClient.get(), &Zera::ProxyClient::connected, this, &cPower2ModuleMeasProgram::monitorConnection); // here we wait until all connections are established
        connect(pcbInterface.get(), &AbstractServerInterface::serverAnswer, this, &cPower2ModuleMeasProgram::catchInterfaceAnswer);
        Zera::Proxy::getInstance()->startConnectionSmart(pcbClient);
    }
}


void cPower2ModuleMeasProgram::pcbserverConnect4freqChannels()
{
    infoReadList = m_FoutInfoMap.keys(); // and then  we look for our different pcb sockets for source
    if (infoReadList.count() > 0) {
        m_nConnectionCount += infoReadList.count();
        for (int i = 0; i < infoReadList.count(); i++) {
            QString key = infoReadList.at(i);
            cFoutInfo fi = m_FoutInfoMap.take(key);
            Zera::ProxyClientPtr pcbClient = Zera::Proxy::getInstance()->getConnectionSmart(
                m_pModule->getNetworkConfig()->m_pcbServiceConnectionInfo,
                m_pModule->getNetworkConfig()->m_tcpNetworkFactory);
            Zera::PcbInterfacePtr pcbInterface = std::make_shared<Zera::cPCBInterface>();
            pcbInterface->setClientSmart(pcbClient);
            fi.pcbIFace = pcbInterface;
            fi.name = key;
            m_FoutInfoMap[key] = fi;
            connect(pcbClient.get(), &Zera::ProxyClient::connected, this, &cPower2ModuleMeasProgram::monitorConnection); // here we wait until all connections are established
            connect(pcbInterface.get(), &AbstractServerInterface::serverAnswer, this, &cPower2ModuleMeasProgram::catchInterfaceAnswer);
            Zera::Proxy::getInstance()->startConnectionSmart(pcbClient);
        }
    }
    else
        emit activationContinue();
}

void cPower2ModuleMeasProgram::readSenseChannelInformation()
{
    infoReadList = m_measChannelInfoHash.keys(); // we have to read information for all channels in this list
    emit activationContinue();
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
        infoReadList = m_FoutInfoMap.keys(); // we have to read information for all channels in this list
        emit activationContinue();
    }
    else
        emit activationSkip();
}


void cPower2ModuleMeasProgram::readSourceChannelAlias()
{
    infoRead = infoReadList.takeFirst();
    m_MsgNrCmdList[m_FoutInfoMap[infoRead].pcbIFace->getAliasSource(infoRead)] = readsourcechannelalias;
}


void cPower2ModuleMeasProgram::readSourceDspChannel()
{
    m_MsgNrCmdList[m_FoutInfoMap[infoRead].pcbIFace->getDSPChannelSource(infoRead)] = readsourcechanneldspchannel;
}


void cPower2ModuleMeasProgram::readSourceFormFactor()
{
    m_MsgNrCmdList[m_FoutInfoMap[infoRead].pcbIFace->getFormFactorSource(infoRead)] = readsourceformfactor;
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
    m_dspClient = Zera::Proxy::getInstance()->getConnectionSmart(m_pModule->getNetworkConfig()->m_dspServiceConnectionInfo,
                                                                 m_pModule->getNetworkConfig()->m_tcpNetworkFactory);
    m_dspInterface->setClientSmart(m_dspClient);
    m_dspserverConnectState.addTransition(m_dspClient.get(), &Zera::ProxyClient::connected, &m_claimPGRMemState);
    connect(m_dspInterface.get(), &AbstractServerInterface::serverAnswer, this, &cPower2ModuleMeasProgram::catchInterfaceAnswer);
    Zera::Proxy::getInstance()->startConnectionSmart(m_dspClient);
}


void cPower2ModuleMeasProgram::claimPGRMem()
{
    setDspVarList(); // first we set the var list for our dsp
    setDspCmdList(); // and the cmd list he has to work on
    m_MsgNrCmdList[m_rmInterface.setResource("DSP1", "PGRMEMC", m_dspInterface->cmdListCount())] = claimpgrmem;
}


void cPower2ModuleMeasProgram::claimUSERMem()
{
   m_MsgNrCmdList[m_rmInterface.setResource("DSP1", "USERMEM", m_nDspMemUsed)] = claimusermem;
}


void cPower2ModuleMeasProgram::varList2DSP()
{
    m_MsgNrCmdList[m_dspInterface->varList2Dsp()] = varlist2dsp;
}


void cPower2ModuleMeasProgram::cmdList2DSP()
{
    m_MsgNrCmdList[m_dspInterface->cmdList2Dsp()] = cmdlist2dsp;
}


void cPower2ModuleMeasProgram::activateDSP()
{
    m_MsgNrCmdList[m_dspInterface->activateInterface()] = activatedsp; // aktiviert die var- und cmd-listen im dsp
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
    if (!m_readUpperRangeValueMachine.isRunning())
        m_readUpperRangeValueMachine.start();

    emit activated();
}

void cPower2ModuleMeasProgram::freePGRMem()
{
    m_dataAcquisitionMachine.stop();
    m_bActive = false;
    Zera::Proxy::getInstance()->releaseConnectionSmart(m_dspClient);
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
        infoReadList = m_FoutInfoMap.keys(); // we have to read information for all channels in this list
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
    disconnect(&m_rmInterface, 0, this, 0);
    disconnect(m_dspInterface.get(), 0, this, 0);

    emit deactivated();
}


void cPower2ModuleMeasProgram::dataAcquisitionDSP()
{
    m_pMeasureSignal->setValue(QVariant(0));
    if(m_bActive)
        m_MsgNrCmdList[m_dspInterface->dataAcquisition(m_pActualValuesDSP)] = dataaquistion; // we start our data aquisition now
}


void cPower2ModuleMeasProgram::dataReadDSP()
{
    if (m_bActive) {
        m_ModuleActualValues = m_pActualValuesDSP->getData();
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
            cFoutInfo fi = m_FoutInfoMap[getConfData()->m_FreqOutputConfList.at(i).m_sName];
            frScale = fi.formFactor * constantImpulsePerWs;

            /*if(m_pScalingInputs.length() > i){
                if(m_pScalingInputs.at(i).first != nullptr && m_pScalingInputs.at(i).second != nullptr){
                    double scale=m_pScalingInputs.at(i).first->value().toDouble()*m_pScalingInputs.at(i).second->value().toDouble();
                    frScale=frScale*scale;
                }
            }*/
            datalist += QString("%1,").arg(frScale, 0, 'g', 9);
        }
        datalist.resize(datalist.size()-1);
        datalist += ";";
        m_pfreqScaleDSP->setVarData(datalist);
        m_MsgNrCmdList[m_dspInterface->dspMemoryWrite(m_pfreqScaleDSP)] = writeparameter;
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
        cFoutInfo fi = m_FoutInfoMap[key];
        if(isValidConstant) {
            m_MsgNrCmdList[fi.pcbIFace->setConstantSource(fi.name, constantImpulsePerKwh)] = writeparameter;
            //m_FoutConstParameterList.at(i)->setValue(constantImpulsePerKwh);
        }
        /*else
            m_FoutConstParameterList.at(i)->setValue(0.0);*/
    }

    double pmax = maxVals.maxU * maxVals.maxI; // MQREF
    QString datalist = QString("NOMPOWER:%1;").arg(pmax, 0, 'g', 9);
    m_pNomPower->setVarData(datalist);
    m_MsgNrCmdList[m_dspInterface->dspMemoryWrite(m_pNomPower)] = setqrefnominalpower;
}

void cPower2ModuleMeasProgram::setFoutPowerModes()
{
    QList<QString> keylist = m_FoutInfoMap.keys();
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
        cFoutInfo fi = m_FoutInfoMap[keylist.at(i)];
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
    m_pParameterDSP->setVarData(strVarData);
    m_MsgNrCmdList[m_dspInterface->dspMemoryWrite(m_pParameterDSP)] = writeparameter;
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

}
