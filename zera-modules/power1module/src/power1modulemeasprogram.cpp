#include <QString>
#include <QStateMachine>
#include <QJsonObject>
#include <QJsonArray>

#include <rminterface.h>
#include <dspinterface.h>
#include <pcbinterface.h>
#include <movingwindowfilter.h>
#include <proxy.h>
#include <proxyclient.h>
#include <scpiinfo.h>
#include <veinmodulemetadata.h>
#include <veinmodulecomponent.h>
#include <veinmoduleparameter.h>
#include <veinmoduleactvalue.h>
#include <modulevalidator.h>
#include <stringvalidator.h>
#include <doublevalidator.h>
#include <intvalidator.h>

#include "debug.h"
#include "errormessages.h"
#include "reply.h"
#include "measmodeinfo.h"
#include "power1module.h"
#include "power1moduleconfigdata.h"
#include "power1modulemeasprogram.h"


namespace POWER1MODULE
{

cPower1ModuleMeasProgram::cPower1ModuleMeasProgram(cPower1Module* module, Zera::Proxy::cProxy* proxy, cPower1ModuleConfigData &configdata)
    :cBaseDspMeasProgram(proxy), m_pModule(module), m_ConfigData(configdata)
{
    m_pRMInterface = new Zera::Server::cRMInterface();
    m_pDSPInterFace = new Zera::Server::cDSPInterface();
    m_pMovingwindowFilter = new cMovingwindowFilter(1.0);

    m_IdentifyState.addTransition(this, SIGNAL(activationContinue()), &m_readResourceTypesState);
    m_readResourceTypesState.addTransition(this, SIGNAL(activationContinue()), &m_readResourceSenseState);

    m_readResourceSenseState.addTransition(this, SIGNAL(activationContinue()), &m_readResourceSenseInfosState);
    m_readResourceSenseInfosState.addTransition(this, SIGNAL(activationContinue()), &m_readResourceSenseInfoState);
    m_readResourceSenseInfoState.addTransition(this, SIGNAL(activationContinue()), &m_readResourceSenseInfoDoneState);
    m_readResourceSenseInfoDoneState.addTransition(this, SIGNAL(activationContinue()), &m_readResourceSourceState);
    m_readResourceSenseInfoDoneState.addTransition(this, SIGNAL(activationLoop()), &m_readResourceSenseInfoState);

    m_readResourceSourceState.addTransition(this, SIGNAL(activationContinue()), &m_readResourceSourceInfosState);
    m_readResourceSourceState.addTransition(this, SIGNAL(activationSkip()), &m_pcbserverConnectState4measChannels);
    m_readResourceSourceInfosState.addTransition(this, SIGNAL(activationContinue()), &m_readResourceSourceInfoState);
    m_readResourceSourceInfoState.addTransition(this, SIGNAL(activationContinue()), &m_readResourceSourceInfoDoneState);
    m_readResourceSourceInfoDoneState.addTransition(this, SIGNAL(activationContinue()), &m_claimResourcesSourceState);
    m_readResourceSourceInfoDoneState.addTransition(this, SIGNAL(activationLoop()), &m_readResourceSourceInfoState);

    m_claimResourcesSourceState.addTransition(this, SIGNAL(activationContinue()), &m_claimResourceSourceState);
    m_claimResourceSourceState.addTransition(this, SIGNAL(activationContinue()), &m_claimResourceSourceDoneState);
    m_claimResourceSourceDoneState.addTransition(this, SIGNAL(activationContinue()), &m_pcbserverConnectState4measChannels);
    m_claimResourceSourceDoneState.addTransition(this, SIGNAL(activationLoop()), &m_claimResourceSourceState);
    m_pcbserverConnectState4measChannels.addTransition(this, SIGNAL(activationContinue()), &m_pcbserverConnectState4freqChannels);
    m_pcbserverConnectState4freqChannels.addTransition(this, SIGNAL(activationContinue()), &m_readSampleRateState);
    m_readSampleRateState.addTransition(this, SIGNAL(activationContinue()), &m_readSenseChannelInformationState);
    m_readSenseChannelInformationState.addTransition(this, SIGNAL(activationContinue()), &m_readSenseChannelAliasState);
    m_readSenseChannelAliasState.addTransition(this, SIGNAL(activationContinue()), &m_readSenseChannelUnitState);
    m_readSenseChannelUnitState.addTransition(this, SIGNAL(activationContinue()), &m_readSenseDspChannelState);
    m_readSenseDspChannelState.addTransition(this, SIGNAL(activationContinue()), &m_readSenseChannelInformationDoneState);
    m_readSenseChannelInformationDoneState.addTransition(this, SIGNAL(activationContinue()), &m_readSourceChannelInformationState);
    m_readSenseChannelInformationDoneState.addTransition(this, SIGNAL(activationLoop()), &m_readSenseChannelAliasState);

    m_readSourceChannelInformationState.addTransition(this, SIGNAL(activationContinue()), &m_readSourceChannelAliasState);
    m_readSourceChannelInformationState.addTransition(this, SIGNAL(activationSkip()), &m_dspserverConnectState);
    m_readSourceChannelAliasState.addTransition(this, SIGNAL(activationContinue()), &m_readSourceDspChannelState);
    m_readSourceDspChannelState.addTransition(this, SIGNAL(activationContinue()), &m_readSourceFormFactorState);
    m_readSourceFormFactorState.addTransition(this, SIGNAL(activationContinue()), &m_readSourceChannelInformationDoneState);
    m_readSourceChannelInformationDoneState.addTransition(this, SIGNAL(activationContinue()), &m_setSenseChannelRangeNotifiersState);
    m_readSourceChannelInformationDoneState.addTransition(this, SIGNAL(activationLoop()), &m_readSourceChannelAliasState);

    m_setSenseChannelRangeNotifiersState.addTransition(this, SIGNAL(activationContinue()), &m_setSenseChannelRangeNotifierState);
    m_setSenseChannelRangeNotifierState.addTransition(this, SIGNAL(activationContinue()), &m_setSenseChannelRangeNotifierDoneState);
    m_setSenseChannelRangeNotifierDoneState.addTransition(this, SIGNAL(activationContinue()), &m_dspserverConnectState);
    m_setSenseChannelRangeNotifierDoneState.addTransition(this, SIGNAL(activationLoop()), &m_setSenseChannelRangeNotifierState);

    m_claimPGRMemState.addTransition(this, SIGNAL(activationContinue()), &m_claimUSERMemState);
    m_claimUSERMemState.addTransition(this, SIGNAL(activationContinue()), &m_var2DSPState);
    m_var2DSPState.addTransition(this, SIGNAL(activationContinue()), &m_cmd2DSPState);
    m_cmd2DSPState.addTransition(this, SIGNAL(activationContinue()), &m_activateDSPState);
    m_activateDSPState.addTransition(this, SIGNAL(activationContinue()), &m_loadDSPDoneState);

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

    m_activationMachine.setInitialState(&m_resourceManagerConnectState);

    connect(&m_resourceManagerConnectState, SIGNAL(entered()), SLOT(resourceManagerConnect()));
    connect(&m_IdentifyState, SIGNAL(entered()), SLOT(sendRMIdent()));
    connect(&m_readResourceTypesState, SIGNAL(entered()), SLOT(readResourceTypes()));
    connect(&m_readResourceSenseState, SIGNAL(entered()), SLOT(readResourceSense()));
    connect(&m_readResourceSenseInfosState, SIGNAL(entered()), SLOT(readResourceSenseInfos()));
    connect(&m_readResourceSenseInfoState, SIGNAL(entered()), SLOT(readResourceSenseInfo()));
    connect(&m_readResourceSenseInfoDoneState, SIGNAL(entered()), SLOT(readResourceSenseInfoDone()));

    connect(&m_readResourceSourceState, SIGNAL(entered()), SLOT(readResourceSource()));
    connect(&m_readResourceSourceInfosState, SIGNAL(entered()), SLOT(readResourceSourceInfos()));
    connect(&m_readResourceSourceInfoState, SIGNAL(entered()), SLOT(readResourceSourceInfo()));
    connect(&m_readResourceSourceInfoDoneState, SIGNAL(entered()), SLOT(readResourceSourceInfoDone()));

    connect(&m_dspserverConnectState, SIGNAL(entered()), SLOT(dspserverConnect()));
    connect(&m_claimResourcesSourceState, SIGNAL(entered()), SLOT(claimResourcesSource()));
    connect(&m_claimResourceSourceState, SIGNAL(entered()), SLOT(claimResourceSource()));
    connect(&m_claimResourceSourceDoneState, SIGNAL(entered()), SLOT(claimResourceSourceDone()));

    connect(&m_pcbserverConnectState4measChannels, SIGNAL(entered()), SLOT(pcbserverConnect4measChannels()));
    connect(&m_pcbserverConnectState4freqChannels, SIGNAL(entered()), SLOT(pcbserverConnect4freqChannels()));
    connect(&m_readSampleRateState, SIGNAL(entered()), SLOT(readSampleRate()));

    connect(&m_readSenseChannelInformationState, SIGNAL(entered()), SLOT(readSenseChannelInformation()));
    connect(&m_readSenseChannelAliasState, SIGNAL(entered()), SLOT(readSenseChannelAlias()));
    connect(&m_readSenseChannelUnitState, SIGNAL(entered()), SLOT(readSenseChannelUnit()));
    connect(&m_readSenseDspChannelState, SIGNAL(entered()), SLOT(readSenseDspChannel()));
    connect(&m_readSenseChannelInformationDoneState, SIGNAL(entered()), SLOT(readSenseChannelInformationDone()));

    connect(&m_readSourceChannelInformationState, SIGNAL(entered()), SLOT(readSourceChannelInformation()));
    connect(&m_readSourceChannelAliasState, SIGNAL(entered()), SLOT(readSourceChannelAlias()));
    connect(&m_readSourceDspChannelState, SIGNAL(entered()), SLOT(readSourceDspChannel()));
    connect(&m_readSourceFormFactorState, SIGNAL(entered()), SLOT(readSourceFormFactor()));
    connect(&m_readSourceChannelInformationDoneState, SIGNAL(entered()), SLOT(readSourceChannelInformationDone()));

    connect(&m_setSenseChannelRangeNotifiersState, SIGNAL(entered()), SLOT(setSenseChannelRangeNotifiers()));
    connect(&m_setSenseChannelRangeNotifierState, SIGNAL(entered()), SLOT(setSenseChannelRangeNotifier()));
    connect(&m_setSenseChannelRangeNotifierDoneState, SIGNAL(entered()), SLOT(setSenseChannelRangeNotifierDone()));

    connect(&m_claimPGRMemState, SIGNAL(entered()), SLOT(claimPGRMem()));
    connect(&m_claimUSERMemState, SIGNAL(entered()), SLOT(claimUSERMem()));
    connect(&m_var2DSPState, SIGNAL(entered()), SLOT(varList2DSP()));
    connect(&m_cmd2DSPState, SIGNAL(entered()), SLOT(cmdList2DSP()));
    connect(&m_activateDSPState, SIGNAL(entered()), SLOT(activateDSP()));
    connect(&m_loadDSPDoneState, SIGNAL(entered()), SLOT(activateDSPdone()));

    // setting up statemachine for unloading dsp and setting resources free
    m_deactivateDSPState.addTransition(this, SIGNAL(deactivationContinue()), &m_freePGRMemState);
    m_freePGRMemState.addTransition(this, SIGNAL(deactivationContinue()), &m_freeUSERMemState);
    m_freeUSERMemState.addTransition(this, SIGNAL(deactivationContinue()), &m_freeFreqOutputsState);

    m_freeFreqOutputsState.addTransition(this, SIGNAL(deactivationContinue()), &m_freeFreqOutputState);
    m_freeFreqOutputsState.addTransition(this, SIGNAL(deactivationSkip()), &m_resetNotifiersState);
    m_freeFreqOutputState.addTransition(this, SIGNAL(deactivationContinue()), &m_freeFreqOutputDoneState);
    m_freeFreqOutputDoneState.addTransition(this, SIGNAL(deactivationContinue()), &m_resetNotifiersState);
    m_freeFreqOutputDoneState.addTransition(this, SIGNAL(deactivationLoop()), &m_freeFreqOutputState);
    m_resetNotifiersState.addTransition(this, SIGNAL(deactivationContinue()), &m_resetNotifierState);
    m_resetNotifierState.addTransition(this, SIGNAL(deactivationContinue()), &m_resetNotifierDoneState);
    m_resetNotifierDoneState.addTransition(this, SIGNAL(deactivationContinue()), &m_unloadDSPDoneState);
    m_resetNotifierDoneState.addTransition(this, SIGNAL(deactivationLoop()), &m_resetNotifierState);

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

    m_deactivationMachine.setInitialState(&m_deactivateDSPState);

    connect(&m_deactivateDSPState, SIGNAL(entered()), SLOT(deactivateDSP()));
    connect(&m_freePGRMemState, SIGNAL(entered()), SLOT(freePGRMem()));
    connect(&m_freeUSERMemState, SIGNAL(entered()), SLOT(freeUSERMem()));

    connect(&m_freeFreqOutputsState, SIGNAL(entered()), SLOT(freeFreqOutputs()));
    connect(&m_freeFreqOutputState, SIGNAL(entered()), SLOT(freeFreqOutput()));
    connect(&m_freeFreqOutputDoneState, SIGNAL(entered()), SLOT(freeFreqOutputDone()));
    connect(&m_resetNotifiersState, SIGNAL(entered()), SLOT(resetNotifiers()));
    connect(&m_resetNotifierState, SIGNAL(entered()), SLOT(resetNotifier()));
    connect(&m_resetNotifierDoneState, SIGNAL(entered()), SLOT(resetNotifierDone()));

    connect(&m_unloadDSPDoneState, SIGNAL(entered()), SLOT(deactivateDSPdone()));

    // setting up statemachine for data acquisition
    m_dataAcquisitionState.addTransition(this, SIGNAL(dataAquisitionContinue()), &m_dataAcquisitionDoneState);
    m_dataAcquisitionMachine.addState(&m_dataAcquisitionState);
    m_dataAcquisitionMachine.addState(&m_dataAcquisitionDoneState);
    m_dataAcquisitionMachine.setInitialState(&m_dataAcquisitionState);
    connect(&m_dataAcquisitionState, SIGNAL(entered()), SLOT(dataAcquisitionDSP()));
    connect(&m_dataAcquisitionDoneState, SIGNAL(entered()), SLOT(dataReadDSP()));

    // setting up statemachine for reading urvalues from channels that changed its range
    m_readUrvalueState.addTransition(this, SIGNAL(activationContinue()), &m_readUrvalueDoneState);
    m_readUrvalueDoneState.addTransition(this, SIGNAL(activationContinue()), &m_setFrequencyScalesState);
    m_readUrvalueDoneState.addTransition(this, SIGNAL(activationLoop()), &m_readUrvalueState);
    m_setFrequencyScalesState.addTransition(this, SIGNAL(activationContinue()), &m_setFoutConstantState);

    m_readUrValueMachine.addState(&m_readUrvalueState);
    m_readUrValueMachine.addState(&m_readUrvalueDoneState);
    m_readUrValueMachine.addState(&m_setFrequencyScalesState);
    m_readUrValueMachine.addState(&m_setFoutConstantState);

    m_readUrValueMachine.setInitialState(&m_readUrvalueState);

    connect(&m_readUrvalueState, SIGNAL(entered()), SLOT(readUrvalue()));
    connect(&m_readUrvalueDoneState, SIGNAL(entered()), SLOT(readUrvalueDone()));
    connect(&m_setFrequencyScalesState, SIGNAL(entered()), SLOT(setFrequencyScales()));
    connect(&m_setFoutConstantState, SIGNAL(entered()), SLOT(setFoutConstants()));
}


cPower1ModuleMeasProgram::~cPower1ModuleMeasProgram()
{
    delete m_pRMInterface;
    delete m_pDSPInterFace;
    delete m_pMovingwindowFilter;
}


void cPower1ModuleMeasProgram::start()
{
    if (m_ConfigData.m_bmovingWindow)
    {
        m_pMovingwindowFilter->setIntegrationtime(m_ConfigData.m_fMeasIntervalTime.m_fValue);
        connect(this, SIGNAL(actualValues(QVector<float>*)), m_pMovingwindowFilter, SLOT(receiveActualValues(QVector<float>*)));
        connect(m_pMovingwindowFilter, SIGNAL(actualValues(QVector<float>*)), this, SLOT(setInterfaceActualValues(QVector<float>*)));
    }
    else
        connect(this, SIGNAL(actualValues(QVector<float>*)), this, SLOT(setInterfaceActualValues(QVector<float>*)));
}


void cPower1ModuleMeasProgram::stop()
{
    disconnect(this, SIGNAL(actualValues(QVector<float>*)), this, 0);
    disconnect(m_pMovingwindowFilter, 0, 0, 0);}


void cPower1ModuleMeasProgram::generateInterface()
{
    QString key;
    cVeinModuleActvalue *pActvalue;

    for (int i = 0; i < 4; i++) // we have fixed number of power values (4)
    {
        pActvalue = new cVeinModuleActvalue(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                            QString("ACT_PQS%1").arg(i+1),
                                            QString("Component forwards the power actual value"),
                                            QVariant(0.0) );
        m_ActValueList.append(pActvalue); // we add the component for our measurement
        m_pModule->veinModuleActvalueList.append(pActvalue); // and for the modules interface
    }

    m_pPQSCountInfo = new cVeinModuleMetaData(QString("PQSCount"), QVariant(4));
    m_pModule->veinModuleMetaDataList.append(m_pPQSCountInfo);


    // a list with all possible measuring modes
    m_MeasuringModeInfoHash["4LW"] = cMeasModeInfo(tr("4LW"), "P", "W", actPower, m4lw);
    m_MeasuringModeInfoHash["4LB"] = cMeasModeInfo(tr("4LB"), "Q", "Var", reactPower, m4lb);
    m_MeasuringModeInfoHash["4LBK"] = cMeasModeInfo(tr("4LBK"), "Q", "Var", reactPower, m4lbk);
    m_MeasuringModeInfoHash["4LS"] = cMeasModeInfo(tr("4LS"), "S", "VA", appPower, m4ls);
    m_MeasuringModeInfoHash["4LSg"] = cMeasModeInfo(tr("4LSg"), "S", "W", appPower, m4lsg);
    m_MeasuringModeInfoHash["3LW"] = cMeasModeInfo(tr("3LW"), "P", "W", actPower, m3lw);
    m_MeasuringModeInfoHash["3LB"] = cMeasModeInfo(tr("3LB"), "Q", "Var", reactPower, m3lb);
    m_MeasuringModeInfoHash["2LW"] = cMeasModeInfo(tr("2LW"), "P", "W", actPower, m2lw);
    m_MeasuringModeInfoHash["2LB"] = cMeasModeInfo(tr("2LB"), "Q", "Var", reactPower, m2lb);
    m_MeasuringModeInfoHash["2LS"] = cMeasModeInfo(tr("2LS"), "S", "VA", appPower, m2ls);
    m_MeasuringModeInfoHash["2LSg"] = cMeasModeInfo(tr("2LSg"), "S", "VA", appPower, m2lsg);

    // our parameters we deal with
    m_pMeasuringmodeParameter = new cVeinModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                                         key = QString("PAR_MeasuringMode"),
                                                               QString("Component for setting the modules measuring mode"),
                                                               QVariant(m_ConfigData.m_sMeasuringMode.m_sValue));

    m_pMeasuringmodeParameter->setSCPIInfo(new cSCPIInfo("CONFIGURATION","MMODE", "10", "PAR_MeasuringMode", "0", ""));

    cStringValidator *sValidator;
    sValidator = new cStringValidator(m_ConfigData.m_sMeasmodeList);
    m_pMeasuringmodeParameter->setValidator(sValidator);

    m_pModule->veinModuleParameterHash[key] = m_pMeasuringmodeParameter; // for modules use

    m_pIntegrationTimeParameter = new cVeinModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                                           key = QString("PAR_IntegrationTime"),
                                                           QString("Component for setting the modules integration time"),
                                                           QVariant(m_ConfigData.m_fMeasIntervalTime.m_fValue));
    m_pIntegrationTimeParameter->setUnit("sec");
    m_pIntegrationTimeParameter->setSCPIInfo(new cSCPIInfo("CONFIGURATION","TINTEGRATION", "10", "PAR_IntegrationTime", "0", "sec"));

    cDoubleValidator *dValidator;
    dValidator = new cDoubleValidator(0.1, 100.0, 0.1);
    m_pIntegrationTimeParameter->setValidator(dValidator);

    m_pModule->veinModuleParameterHash[key] = m_pIntegrationTimeParameter; // for modules use

    m_pIntegrationPeriodParameter = new cVeinModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                                             key = QString("PAR_IntegrationPeriod"),
                                                             QString("Component for setting the modules integration period"),
                                                             QVariant(m_ConfigData.m_nMeasIntervalPeriod.m_nValue));
    m_pIntegrationPeriodParameter->setSCPIInfo(new cSCPIInfo("CONFIGURATION","TPERIOD", "10", "PAR_IntegrationPeriod", "0", ""));

    cIntValidator *iValidator;
    iValidator = new cIntValidator(5, 5000, 1);
    m_pIntegrationPeriodParameter->setValidator(iValidator);

    m_pModule->veinModuleParameterHash[key] = m_pIntegrationPeriodParameter; // for modules use

    m_pMeasureSignal = new cVeinModuleComponent(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                                QString("SIG_Measuring"),
                                                QString("Component forwards a signal indicating measurement activity"),
                                                QVariant(0));

    m_pModule->veinModuleComponentList.append(m_pMeasureSignal);
}


void cPower1ModuleMeasProgram::deleteInterface()
{
}


void cPower1ModuleMeasProgram::setDspVarList()
{
    // we fetch a handle for sampled data and other temporary values
    m_pTmpDataDsp = m_pDSPInterFace->getMemHandle("TmpData");
    m_pTmpDataDsp->addVarItem( new cDspVar("MEASSIGNAL1", m_nSRate, DSPDATA::vDspTemp)); // we need 2 signals for our computations
    m_pTmpDataDsp->addVarItem( new cDspVar("MEASSIGNAL2", m_nSRate, DSPDATA::vDspTemp));
    m_pTmpDataDsp->addVarItem( new cDspVar("VALPQS", 4, DSPDATA::vDspTemp)); // here x1, x2, x3 , xs will land
    m_pTmpDataDsp->addVarItem( new cDspVar("TEMP1", 2, DSPDATA::vDspTemp)); // we need 2 temp. vars also for complex
    m_pTmpDataDsp->addVarItem( new cDspVar("TEMP2", 2, DSPDATA::vDspTemp));
    m_pTmpDataDsp->addVarItem( new cDspVar("FAK", 1, DSPDATA::vDspTemp));
    m_pTmpDataDsp->addVarItem( new cDspVar("FILTER", 2*4,DSPDATA::vDspTemp));
    m_pTmpDataDsp->addVarItem( new cDspVar("N",1,DSPDATA::vDspTemp));

    // a handle for parameter
    m_pParameterDSP =  m_pDSPInterFace->getMemHandle("Parameter");
    m_pParameterDSP->addVarItem( new cDspVar("TIPAR",1, DSPDATA::vDspParam, DSPDATA::dInt)); // integrationtime res = 1ms or period
    // we use tistart as parameter, so we can finish actual measuring interval bei setting 0
    m_pParameterDSP->addVarItem( new cDspVar("TISTART",1, DSPDATA::vDspParam, DSPDATA::dInt));
    m_pParameterDSP->addVarItem( new cDspVar("MMODE",1, DSPDATA::vDspParam, DSPDATA::dInt));

    // a handle for filtered actual values
    m_pActualValuesDSP = m_pDSPInterFace->getMemHandle("ActualValues");
    m_pActualValuesDSP->addVarItem( new cDspVar("VALPQSF", 4, DSPDATA::vDspResult));

    // and one for the frequency output scale values, we need 1 value for each configured output
    m_pfreqScaleDSP = m_pDSPInterFace->getMemHandle("FrequencyScale");
    m_pfreqScaleDSP->addVarItem( new cDspVar("FREQSCALE", m_ConfigData.m_nFreqOutputCount, DSPDATA::vDspParam));

    m_ModuleActualValues.resize(m_pActualValuesDSP->getSize()); // we provide a vector for generated actual values
    m_nDspMemUsed = m_pTmpDataDsp->getSize() + m_pParameterDSP->getSize() + m_pActualValuesDSP->getSize();
}


void cPower1ModuleMeasProgram::deleteDspVarList()
{
    m_pDSPInterFace->deleteMemHandle(m_pTmpDataDsp);
    m_pDSPInterFace->deleteMemHandle(m_pParameterDSP);
    m_pDSPInterFace->deleteMemHandle(m_pActualValuesDSP);
}


void cPower1ModuleMeasProgram::setDspCmdList()
{
    QString s;
    QStringList sl1, sl2, sl3;

    m_pDSPInterFace->addCycListItem( s = "STARTCHAIN(1,1,0x0101)"); // aktiv, prozessnr. (dummy),hauptkette 1 subkette 1 start
        m_pDSPInterFace->addCycListItem( s = QString("CLEARN(%1,MEASSIGNAL1)").arg(m_nSRate) ); // clear meassignal
        m_pDSPInterFace->addCycListItem( s = QString("CLEARN(%1,MEASSIGNAL2)").arg(m_nSRate) ); // clear meassignal
        m_pDSPInterFace->addCycListItem( s = QString("CLEARN(%1,FILTER)").arg(2*4+1) ); // clear the whole filter incl. count
        m_pDSPInterFace->addCycListItem( s = QString("SETVAL(MMODE,%1)").arg(m_MeasuringModeInfoHash[m_ConfigData.m_sMeasuringMode.m_sValue].getCode())); // initial measuring mode
        m_pDSPInterFace->addCycListItem( s = QString("SETVAL(FAK,0.5)"));

        if (m_ConfigData.m_sIntegrationMode == "time")
        {

            if (m_ConfigData.m_bmovingWindow)
                m_pDSPInterFace->addCycListItem( s = QString("SETVAL(TIPAR,%1)").arg(m_ConfigData.m_fmovingwindowInterval*1000.0)); // initial ti time
            else
                m_pDSPInterFace->addCycListItem( s = QString("SETVAL(TIPAR,%1)").arg(m_ConfigData.m_fMeasIntervalTime.m_fValue*1000.0)); // initial ti time

            m_pDSPInterFace->addCycListItem( s = "GETSTIME(TISTART)"); // einmal ti start setzen
        }
        else
        {
            m_pDSPInterFace->addCycListItem( s = QString("SETVAL(TIPAR,%1)").arg(m_ConfigData.m_nMeasIntervalPeriod.m_nValue)); // initial ti time
        }

        m_pDSPInterFace->addCycListItem( s = "DEACTIVATECHAIN(1,0x0101)"); // ende prozessnr., hauptkette 1 subkette 1
    m_pDSPInterFace->addCycListItem( s = "STOPCHAIN(1,0x0101)"); // ende prozessnr., hauptkette 1 subkette 1

    // we set up all our lists for wanted measuring modes, this gets much more performance

    for (int i = 0; i < m_ConfigData.m_nMeasModeCount; i++)
    {
        QStringList sl;
        int mmode = m_MeasuringModeInfoHash[m_ConfigData.m_sMeasmodeList.at(i)].getCode();
        switch (mmode)
        {
        case m4lw:
            m_pDSPInterFace->addCycListItem( s = "ACTIVATECHAIN(1,0x0110)");
            m_pDSPInterFace->addCycListItem( s = QString("TESTVCSKIPEQ(MMODE,%1)").arg(mmode));
            m_pDSPInterFace->addCycListItem( s = "DEACTIVATECHAIN(1,0x0110)");
            m_pDSPInterFace->addCycListItem( s = "STARTCHAIN(0,1,0x0110)"); // inaktiv, prozessnr. (dummy),hauptkette 1 subkette 1 start

            sl = m_ConfigData.m_sMeasSystemList.at(0).split(',');
            // our first measuring system
            m_pDSPInterFace->addCycListItem( s = QString("COPYDATA(CH%1,0,MEASSIGNAL1)").arg(m_measChannelInfoHash.value(sl.at(0)).dspChannelNr));
            m_pDSPInterFace->addCycListItem( s = QString("COPYDATA(CH%1,0,MEASSIGNAL2)").arg(m_measChannelInfoHash.value(sl.at(1)).dspChannelNr));
            m_pDSPInterFace->addCycListItem( s = "MULCCV(MEASSIGNAL1,MEASSIGNAL2,VALPQS)");

            sl = m_ConfigData.m_sMeasSystemList.at(1).split(',');
            // our second measuring system
            m_pDSPInterFace->addCycListItem( s = QString("COPYDATA(CH%1,0,MEASSIGNAL1)").arg(m_measChannelInfoHash.value(sl.at(0)).dspChannelNr));
            m_pDSPInterFace->addCycListItem( s = QString("COPYDATA(CH%1,0,MEASSIGNAL2)").arg(m_measChannelInfoHash.value(sl.at(1)).dspChannelNr));
            m_pDSPInterFace->addCycListItem( s = "MULCCV(MEASSIGNAL1,MEASSIGNAL2,VALPQS+1)");

            sl = m_ConfigData.m_sMeasSystemList.at(2).split(',');
            // our third measuring system
            m_pDSPInterFace->addCycListItem( s = QString("COPYDATA(CH%1,0,MEASSIGNAL1)").arg(m_measChannelInfoHash.value(sl.at(0)).dspChannelNr));
            m_pDSPInterFace->addCycListItem( s = QString("COPYDATA(CH%1,0,MEASSIGNAL2)").arg(m_measChannelInfoHash.value(sl.at(1)).dspChannelNr));
            m_pDSPInterFace->addCycListItem( s = "MULCCV(MEASSIGNAL1,MEASSIGNAL2,VALPQS+2)");

            m_pDSPInterFace->addCycListItem( s = "STOPCHAIN(1,0x0110)"); // ende prozessnr., hauptkette 1 subkette 1
            break;

        case m4lb:
            m_pDSPInterFace->addCycListItem( s = "ACTIVATECHAIN(1,0x0111)");
            // m_pDSPInterFace->addCycListItem( s = "BREAK(0)"); // breakpoint wenn /taster
            m_pDSPInterFace->addCycListItem( s = QString("TESTVCSKIPEQ(MMODE,%1)").arg(mmode));
            m_pDSPInterFace->addCycListItem( s = "DEACTIVATECHAIN(1,0x0111)");
            m_pDSPInterFace->addCycListItem( s = "STARTCHAIN(0,1,0x0111)"); // inaktiv, prozessnr. (dummy),hauptkette 1 subkette 1 start

            sl = m_ConfigData.m_sMeasSystemList.at(0).split(',');
            // our first measuring system
            m_pDSPInterFace->addCycListItem( s = QString("COPYDATA(CH%1,0,MEASSIGNAL1)").arg(m_measChannelInfoHash.value(sl.at(0)).dspChannelNr));
            m_pDSPInterFace->addCycListItem( s = QString("COPYDATA(CH%1,0,MEASSIGNAL2)").arg(m_measChannelInfoHash.value(sl.at(1)).dspChannelNr));
            m_pDSPInterFace->addCycListItem( s = QString("DFT(1,MEASSIGNAL1,TEMP1)"));
            m_pDSPInterFace->addCycListItem( s = QString("DFT(1,MEASSIGNAL2,TEMP2)"));
            m_pDSPInterFace->addCycListItem( s = QString("MULVVV(TEMP1,TEMP2+1,VALPQS)"));
            m_pDSPInterFace->addCycListItem( s = QString("MULVVV(TEMP2,TEMP1+1,TEMP1)"));
            m_pDSPInterFace->addCycListItem( s = QString("SUBVVV(TEMP1,VALPQS,VALPQS)"));
            m_pDSPInterFace->addCycListItem( s = QString("MULVVV(FAK,VALPQS,VALPQS)"));

            // instead her all harmonics
            //m_pDSPInterFace->addCycListItem( s = QString("ROTATE(MEASSIGNAL2,270.0)"));
            //m_pDSPInterFace->addCycListItem( s = "MULCCV(MEASSIGNAL1,MEASSIGNAL2,VALPQS)");

            sl = m_ConfigData.m_sMeasSystemList.at(1).split(',');
            // our second measuring system
            m_pDSPInterFace->addCycListItem( s = QString("COPYDATA(CH%1,0,MEASSIGNAL1)").arg(m_measChannelInfoHash.value(sl.at(0)).dspChannelNr));
            m_pDSPInterFace->addCycListItem( s = QString("COPYDATA(CH%1,0,MEASSIGNAL2)").arg(m_measChannelInfoHash.value(sl.at(1)).dspChannelNr));

            m_pDSPInterFace->addCycListItem( s = QString("DFT(1,MEASSIGNAL1,TEMP1)"));
            m_pDSPInterFace->addCycListItem( s = QString("DFT(1,MEASSIGNAL2,TEMP2)"));
            m_pDSPInterFace->addCycListItem( s = QString("MULVVV(TEMP1,TEMP2+1,VALPQS+1)"));
            m_pDSPInterFace->addCycListItem( s = QString("MULVVV(TEMP2,TEMP1+1,TEMP1)"));
            m_pDSPInterFace->addCycListItem( s = QString("SUBVVV(TEMP1,VALPQS+1,VALPQS+1)"));
            m_pDSPInterFace->addCycListItem( s = QString("MULVVV(FAK,VALPQS+1,VALPQS+1)"));

            //m_pDSPInterFace->addCycListItem( s = QString("ROTATE(MEASSIGNAL2,270.0)"));
            //m_pDSPInterFace->addCycListItem( s = "MULCCV(MEASSIGNAL1,MEASSIGNAL2,VALPQS+1)");

            sl = m_ConfigData.m_sMeasSystemList.at(2).split(',');
            // our third measuring system
            m_pDSPInterFace->addCycListItem( s = QString("COPYDATA(CH%1,0,MEASSIGNAL1)").arg(m_measChannelInfoHash.value(sl.at(0)).dspChannelNr));
            m_pDSPInterFace->addCycListItem( s = QString("COPYDATA(CH%1,0,MEASSIGNAL2)").arg(m_measChannelInfoHash.value(sl.at(1)).dspChannelNr));

            m_pDSPInterFace->addCycListItem( s = QString("DFT(1,MEASSIGNAL1,TEMP1)"));
            m_pDSPInterFace->addCycListItem( s = QString("DFT(1,MEASSIGNAL2,TEMP2)"));
            m_pDSPInterFace->addCycListItem( s = QString("MULVVV(TEMP1,TEMP2+1,VALPQS+2)"));
            m_pDSPInterFace->addCycListItem( s = QString("MULVVV(TEMP2,TEMP1+1,TEMP1)"));
            m_pDSPInterFace->addCycListItem( s = QString("SUBVVV(TEMP1,VALPQS+2,VALPQS+2)"));
            m_pDSPInterFace->addCycListItem( s = QString("MULVVV(FAK,VALPQS+2,VALPQS+2)"));

            //m_pDSPInterFace->addCycListItem( s = QString("ROTATE(MEASSIGNAL2,270.0)"));
            //m_pDSPInterFace->addCycListItem( s = "MULCCV(MEASSIGNAL1,MEASSIGNAL2,VALPQS+2)");

            m_pDSPInterFace->addCycListItem( s = "STOPCHAIN(1,0x0111)"); // ende prozessnr., hauptkette 1 subkette 1
            break;

        case m4lbk:
            m_pDSPInterFace->addCycListItem( s = "ACTIVATECHAIN(1,0x0112)");
            m_pDSPInterFace->addCycListItem( s = QString("TESTVCSKIPEQ(MMODE,%1)").arg(mmode));
            m_pDSPInterFace->addCycListItem( s = "DEACTIVATECHAIN(1,0x0112)");
            m_pDSPInterFace->addCycListItem( s = "STARTCHAIN(0,1,0x0112)"); // inaktiv, prozessnr. (dummy),hauptkette 1 subkette 1 start

            // we need the information of all our system at the same time
            sl1 = m_ConfigData.m_sMeasSystemList.at(0).split(',');
            sl2 = m_ConfigData.m_sMeasSystemList.at(1).split(',');
            sl3 = m_ConfigData.m_sMeasSystemList.at(2).split(',');

            // our first measuring system
            m_pDSPInterFace->addCycListItem( s = QString("COPYDIFF(CH%1,CH%2,MEASSIGNAL1)")
                                                    .arg(m_measChannelInfoHash.value(sl2.at(0)).dspChannelNr)
                                                    .arg(m_measChannelInfoHash.value(sl3.at(0)).dspChannelNr));
            m_pDSPInterFace->addCycListItem( s = "MULCV(MEASSIGNAL1,0.57735027)"); // we correct 1/sqrt(3)
            m_pDSPInterFace->addCycListItem( s = QString("COPYDATA(CH%1,0,MEASSIGNAL2)").arg(m_measChannelInfoHash.value(sl1.at(1)).dspChannelNr));

            m_pDSPInterFace->addCycListItem( s = QString("DFT(1,MEASSIGNAL1,TEMP1)"));
            m_pDSPInterFace->addCycListItem( s = QString("DFT(1,MEASSIGNAL2,TEMP2)"));
            m_pDSPInterFace->addCycListItem( s = QString("MULVVV(TEMP1,TEMP2,VALPQS)"));
            m_pDSPInterFace->addCycListItem( s = QString("MULVVV(TEMP2+1,TEMP1+1,TEMP1)"));
            m_pDSPInterFace->addCycListItem( s = QString("ADDVVV(TEMP1,VALPQS,VALPQS)"));
            m_pDSPInterFace->addCycListItem( s = QString("MULVVV(FAK,VALPQS,VALPQS)"));

            //m_pDSPInterFace->addCycListItem( s = "MULCCV(MEASSIGNAL1,MEASSIGNAL2,VALPQS)");

            // our second measuring system
            m_pDSPInterFace->addCycListItem( s = QString("COPYDIFF(CH%1,CH%2,MEASSIGNAL1)")
                                                    .arg(m_measChannelInfoHash.value(sl3.at(0)).dspChannelNr)
                                                    .arg(m_measChannelInfoHash.value(sl1.at(0)).dspChannelNr));
            m_pDSPInterFace->addCycListItem( s = "MULCV(MEASSIGNAL1,0.57735027)"); // we correct 1/sqrt(3)
            m_pDSPInterFace->addCycListItem( s = QString("COPYDATA(CH%1,0,MEASSIGNAL2)").arg(m_measChannelInfoHash.value(sl2.at(1)).dspChannelNr));
            m_pDSPInterFace->addCycListItem( s = QString("DFT(1,MEASSIGNAL1,TEMP1)"));
            m_pDSPInterFace->addCycListItem( s = QString("DFT(1,MEASSIGNAL2,TEMP2)"));
            m_pDSPInterFace->addCycListItem( s = QString("MULVVV(TEMP1,TEMP2,VALPQS+1)"));
            m_pDSPInterFace->addCycListItem( s = QString("MULVVV(TEMP2+1,TEMP1+1,TEMP1)"));
            m_pDSPInterFace->addCycListItem( s = QString("ADDVVV(TEMP1,VALPQS+1,VALPQS+1)"));
            m_pDSPInterFace->addCycListItem( s = QString("MULVVV(FAK,VALPQS+1,VALPQS+1)"));

            //m_pDSPInterFace->addCycListItem( s = "MULCCV(MEASSIGNAL1,MEASSIGNAL2,VALPQS+1)");

            // our third measuring system
            m_pDSPInterFace->addCycListItem( s = QString("COPYDIFF(CH%1,CH%2,MEASSIGNAL1)")
                                                    .arg(m_measChannelInfoHash.value(sl1.at(0)).dspChannelNr)
                                                    .arg(m_measChannelInfoHash.value(sl2.at(0)).dspChannelNr));
            m_pDSPInterFace->addCycListItem( s = "MULCV(MEASSIGNAL1,0.57735027)"); // we correct 1/sqrt(3)
            m_pDSPInterFace->addCycListItem( s = QString("COPYDATA(CH%1,0,MEASSIGNAL2)").arg(m_measChannelInfoHash.value(sl3.at(1)).dspChannelNr));

            m_pDSPInterFace->addCycListItem( s = QString("DFT(1,MEASSIGNAL1,TEMP1)"));
            m_pDSPInterFace->addCycListItem( s = QString("DFT(1,MEASSIGNAL2,TEMP2)"));
            m_pDSPInterFace->addCycListItem( s = QString("MULVVV(TEMP1,TEMP2,VALPQS+2)"));
            m_pDSPInterFace->addCycListItem( s = QString("MULVVV(TEMP2+1,TEMP1+1,TEMP1)"));
            m_pDSPInterFace->addCycListItem( s = QString("ADDVVV(TEMP1,VALPQS+2,VALPQS+2)"));
            m_pDSPInterFace->addCycListItem( s = QString("MULVVV(FAK,VALPQS+2,VALPQS+2)"));

            //m_pDSPInterFace->addCycListItem( s = "MULCCV(MEASSIGNAL1,MEASSIGNAL2,VALPQS+2)");

            m_pDSPInterFace->addCycListItem( s = "STOPCHAIN(1,0x0112)"); // ende prozessnr., hauptkette 1 subkette 1
            break;

        case m4ls:
            m_pDSPInterFace->addCycListItem( s = "ACTIVATECHAIN(1,0x0113)");
            m_pDSPInterFace->addCycListItem( s = QString("TESTVCSKIPEQ(MMODE,%1)").arg(mmode));
            m_pDSPInterFace->addCycListItem( s = "DEACTIVATECHAIN(1,0x0113)");
            m_pDSPInterFace->addCycListItem( s = "STARTCHAIN(0,1,0x0113)"); // inaktiv, prozessnr. (dummy),hauptkette 1 subkette 1 start

            sl = m_ConfigData.m_sMeasSystemList.at(0).split(',');
            // our first measuring system
            m_pDSPInterFace->addCycListItem( s = QString("COPYDATA(CH%1,0,MEASSIGNAL1)").arg(m_measChannelInfoHash.value(sl.at(0)).dspChannelNr));
            m_pDSPInterFace->addCycListItem( s = "RMS(MEASSIGNAL1,TEMP1)");
            m_pDSPInterFace->addCycListItem( s = QString("COPYDATA(CH%1,0,MEASSIGNAL1)").arg(m_measChannelInfoHash.value(sl.at(1)).dspChannelNr));
            m_pDSPInterFace->addCycListItem( s = "RMS(MEASSIGNAL1,TEMP2)");
            m_pDSPInterFace->addCycListItem( s = "MULVVV(TEMP1,TEMP2,VALPQS)");

            sl = m_ConfigData.m_sMeasSystemList.at(1).split(',');
            // our second measuring system
            m_pDSPInterFace->addCycListItem( s = QString("COPYDATA(CH%1,0,MEASSIGNAL1)").arg(m_measChannelInfoHash.value(sl.at(0)).dspChannelNr));
            m_pDSPInterFace->addCycListItem( s = "RMS(MEASSIGNAL1,TEMP1)");
            m_pDSPInterFace->addCycListItem( s = QString("COPYDATA(CH%1,0,MEASSIGNAL1)").arg(m_measChannelInfoHash.value(sl.at(1)).dspChannelNr));
            m_pDSPInterFace->addCycListItem( s = "RMS(MEASSIGNAL1,TEMP2)");
            m_pDSPInterFace->addCycListItem( s = "MULVVV(TEMP1,TEMP2,VALPQS+1)");

            sl = m_ConfigData.m_sMeasSystemList.at(2).split(',');
            // our third measuring system
            m_pDSPInterFace->addCycListItem( s = QString("COPYDATA(CH%1,0,MEASSIGNAL1)").arg(m_measChannelInfoHash.value(sl.at(0)).dspChannelNr));
            m_pDSPInterFace->addCycListItem( s = "RMS(MEASSIGNAL1,TEMP1)");
            m_pDSPInterFace->addCycListItem( s = QString("COPYDATA(CH%1,0,MEASSIGNAL1)").arg(m_measChannelInfoHash.value(sl.at(1)).dspChannelNr));
            m_pDSPInterFace->addCycListItem( s = "RMS(MEASSIGNAL1,TEMP2)");
            m_pDSPInterFace->addCycListItem( s = "MULVVV(TEMP1,TEMP2,VALPQS+2)");

            m_pDSPInterFace->addCycListItem( s = "STOPCHAIN(1,0x0113)"); // ende prozessnr., hauptkette 1 subkette 1
            break;

        case m4lsg:
            m_pDSPInterFace->addCycListItem( s = "ACTIVATECHAIN(1,0x0114)");
            m_pDSPInterFace->addCycListItem( s = QString("TESTVCSKIPEQ(MMODE,%1)").arg(mmode));
            m_pDSPInterFace->addCycListItem( s = "DEACTIVATECHAIN(1,0x0114)");
            m_pDSPInterFace->addCycListItem( s = "STARTCHAIN(0,1,0x0114)"); // inaktiv, prozessnr. (dummy),hauptkette 1 subkette 1 start

            sl = m_ConfigData.m_sMeasSystemList.at(0).split(',');
            // our first measuring system
            m_pDSPInterFace->addCycListItem( s = QString("COPYDATA(CH%1,0,MEASSIGNAL1)").arg(m_measChannelInfoHash.value(sl.at(0)).dspChannelNr));
            m_pDSPInterFace->addCycListItem( s = QString("COPYDATA(CH%1,0,MEASSIGNAL2)").arg(m_measChannelInfoHash.value(sl.at(1)).dspChannelNr));
            m_pDSPInterFace->addCycListItem( s = "MULCCV(MEASSIGNAL1,MEASSIGNAL2,TEMP1)"); // P
            m_pDSPInterFace->addCycListItem( s = "ROTATE(MEASSIGNAL2,270.0)");
            m_pDSPInterFace->addCycListItem( s = "MULCCV(MEASSIGNAL1,MEASSIGNAL2,TEMP2)"); // Q
            m_pDSPInterFace->addCycListItem( s = "ADDVVG(TEMP1,TEMP2,VALPQS)"); // geometrical sum is our actual value

            sl = m_ConfigData.m_sMeasSystemList.at(1).split(',');
            // our second measuring system
            m_pDSPInterFace->addCycListItem( s = QString("COPYDATA(CH%1,0,MEASSIGNAL1)").arg(m_measChannelInfoHash.value(sl.at(0)).dspChannelNr));
            m_pDSPInterFace->addCycListItem( s = QString("COPYDATA(CH%1,0,MEASSIGNAL2)").arg(m_measChannelInfoHash.value(sl.at(1)).dspChannelNr));
            m_pDSPInterFace->addCycListItem( s = "MULCCV(MEASSIGNAL1,MEASSIGNAL2,TEMP1)"); // P
            m_pDSPInterFace->addCycListItem( s = "ROTATE(MEASSIGNAL2,270.0)");
            m_pDSPInterFace->addCycListItem( s = "MULCCV(MEASSIGNAL1,MEASSIGNAL2,TEMP2)"); // Q
            m_pDSPInterFace->addCycListItem( s = "ADDVVG(TEMP1,TEMP2,VALPQS+1)"); // geometrical sum is our actual value

            sl = m_ConfigData.m_sMeasSystemList.at(2).split(',');
            // our third measuring system
            m_pDSPInterFace->addCycListItem( s = QString("COPYDATA(CH%1,0,MEASSIGNAL1)").arg(m_measChannelInfoHash.value(sl.at(0)).dspChannelNr));
            m_pDSPInterFace->addCycListItem( s = QString("COPYDATA(CH%1,0,MEASSIGNAL2)").arg(m_measChannelInfoHash.value(sl.at(1)).dspChannelNr));
            m_pDSPInterFace->addCycListItem( s = "MULCCV(MEASSIGNAL1,MEASSIGNAL2,TEMP1)"); // P
            m_pDSPInterFace->addCycListItem( s = "ROTATE(MEASSIGNAL2,270.0)");
            m_pDSPInterFace->addCycListItem( s = "MULCCV(MEASSIGNAL1,MEASSIGNAL2,TEMP2)"); // Q
            m_pDSPInterFace->addCycListItem( s = "ADDVVG(TEMP1,TEMP2,VALPQS+2)"); // geometrical sum is our actual value

            m_pDSPInterFace->addCycListItem( s = "STOPCHAIN(1,0x0114)"); // ende prozessnr., hauptkette 1 subkette 1
            break;

        case m3lw:
            m_pDSPInterFace->addCycListItem( s = "ACTIVATECHAIN(1,0x0115)");
            m_pDSPInterFace->addCycListItem( s = QString("TESTVCSKIPEQ(MMODE,%1)").arg(mmode));
            m_pDSPInterFace->addCycListItem( s = "DEACTIVATECHAIN(1,0x0115)");
            m_pDSPInterFace->addCycListItem( s = "STARTCHAIN(0,1,0x0115)"); // inaktiv, prozessnr. (dummy),hauptkette 1 subkette 1 start

            // we need the information of all our system at the same time
            sl1 = m_ConfigData.m_sMeasSystemList.at(0).split(',');
            sl2 = m_ConfigData.m_sMeasSystemList.at(1).split(',');
            sl3 = m_ConfigData.m_sMeasSystemList.at(2).split(',');

            // our first measuring system
            m_pDSPInterFace->addCycListItem( s = QString("COPYDIFF(CH%1,CH%2,MEASSIGNAL1)")
                                                    .arg(m_measChannelInfoHash.value(sl1.at(0)).dspChannelNr)
                                                    .arg(m_measChannelInfoHash.value(sl2.at(0)).dspChannelNr));
            m_pDSPInterFace->addCycListItem( s = QString("COPYDATA(CH%1,0,MEASSIGNAL2)").arg(m_measChannelInfoHash.value(sl1.at(1)).dspChannelNr));
            m_pDSPInterFace->addCycListItem( s = "MULCCV(MEASSIGNAL1,MEASSIGNAL2,VALPQS)");

            // our second measuring system
            m_pDSPInterFace->addCycListItem( s = "SETVAL(VALPQS+1,0.0)"); // is 0 output

            // our third measuring system
            m_pDSPInterFace->addCycListItem( s = QString("COPYDIFF(CH%1,CH%2,MEASSIGNAL1)")
                                                    .arg(m_measChannelInfoHash.value(sl3.at(0)).dspChannelNr)
                                                    .arg(m_measChannelInfoHash.value(sl2.at(0)).dspChannelNr));
            m_pDSPInterFace->addCycListItem( s = QString("COPYDATA(CH%1,0,MEASSIGNAL2)").arg(m_measChannelInfoHash.value(sl3.at(1)).dspChannelNr));
            m_pDSPInterFace->addCycListItem( s = "MULCCV(MEASSIGNAL1,MEASSIGNAL2,VALPQS+2)");

            m_pDSPInterFace->addCycListItem( s = "STOPCHAIN(1,0x0115)"); // ende prozessnr., hauptkette 1 subkette 1
            break;

        case m3lb:
            m_pDSPInterFace->addCycListItem( s = "ACTIVATECHAIN(1,0x0116)");
            m_pDSPInterFace->addCycListItem( s = QString("TESTVCSKIPEQ(MMODE,%1)").arg(mmode));
            m_pDSPInterFace->addCycListItem( s = "DEACTIVATECHAIN(1,0x0116)");
            m_pDSPInterFace->addCycListItem( s = "STARTCHAIN(0,1,0x0116)"); // inaktiv, prozessnr. (dummy),hauptkette 1 subkette 1 start

            // we need the information of all our system at the same time
            sl1 = m_ConfigData.m_sMeasSystemList.at(0).split(',');
            sl2 = m_ConfigData.m_sMeasSystemList.at(1).split(',');
            sl3 = m_ConfigData.m_sMeasSystemList.at(2).split(',');

            // our first measuring system
            m_pDSPInterFace->addCycListItem( s = QString("COPYDIFF(CH%1,CH%2,MEASSIGNAL1)")
                                                    .arg(m_measChannelInfoHash.value(sl1.at(0)).dspChannelNr)
                                                    .arg(m_measChannelInfoHash.value(sl2.at(0)).dspChannelNr));
            m_pDSPInterFace->addCycListItem( s = QString("COPYDATA(CH%1,0,MEASSIGNAL2)").arg(m_measChannelInfoHash.value(sl1.at(1)).dspChannelNr));
            m_pDSPInterFace->addCycListItem( s = QString("DFT(1,MEASSIGNAL1,TEMP1)"));
            m_pDSPInterFace->addCycListItem( s = QString("DFT(1,MEASSIGNAL2,TEMP2)"));
            m_pDSPInterFace->addCycListItem( s = QString("MULVVV(TEMP1,TEMP2+1,VALPQS)"));
            m_pDSPInterFace->addCycListItem( s = QString("MULVVV(TEMP2,TEMP1+1,TEMP1)"));
            m_pDSPInterFace->addCycListItem( s = QString("SUBVVV(TEMP1,VALPQS,VALPQS)"));
            m_pDSPInterFace->addCycListItem( s = QString("MULVVV(FAK,VALPQS,VALPQS)"));

            //m_pDSPInterFace->addCycListItem( s = "ROTATE(MEASSIGNAL2,270.0)");
            //m_pDSPInterFace->addCycListItem( s = "MULCCV(MEASSIGNAL1,MEASSIGNAL2,VALPQS)");

            // our second measuring system
            m_pDSPInterFace->addCycListItem( s = "SETVAL(VALPQS+1,0.0)"); // is 0 output

            // our third measuring system
            m_pDSPInterFace->addCycListItem( s = QString("COPYDIFF(CH%1,CH%2,MEASSIGNAL1)")
                                                    .arg(m_measChannelInfoHash.value(sl3.at(0)).dspChannelNr)
                                                    .arg(m_measChannelInfoHash.value(sl2.at(0)).dspChannelNr));
            m_pDSPInterFace->addCycListItem( s = QString("COPYDATA(CH%1,0,MEASSIGNAL2)").arg(m_measChannelInfoHash.value(sl3.at(1)).dspChannelNr));
            m_pDSPInterFace->addCycListItem( s = QString("DFT(1,MEASSIGNAL1,TEMP1)"));
            m_pDSPInterFace->addCycListItem( s = QString("DFT(1,MEASSIGNAL2,TEMP2)"));
            m_pDSPInterFace->addCycListItem( s = QString("MULVVV(TEMP1,TEMP2+1,VALPQS+2)"));
            m_pDSPInterFace->addCycListItem( s = QString("MULVVV(TEMP2,TEMP1+1,TEMP1)"));
            m_pDSPInterFace->addCycListItem( s = QString("SUBVVV(TEMP1,VALPQS+2,VALPQS+2)"));
            m_pDSPInterFace->addCycListItem( s = QString("MULVVV(FAK,VALPQS+2,VALPQS+2)"));

            //m_pDSPInterFace->addCycListItem( s = "ROTATE(MEASSIGNAL2,270.0)");
            //m_pDSPInterFace->addCycListItem( s = "MULCCV(MEASSIGNAL1,MEASSIGNAL2,VALPQS+2)");

            m_pDSPInterFace->addCycListItem( s = "STOPCHAIN(1,0x0116)"); // ende prozessnr., hauptkette 1 subkette 1
            break;

        case m2lw:
        {
            m_pDSPInterFace->addCycListItem( s = "ACTIVATECHAIN(1,0x0117)");
            m_pDSPInterFace->addCycListItem( s = QString("TESTVCSKIPEQ(MMODE,%1)").arg(mmode));
            m_pDSPInterFace->addCycListItem( s = "DEACTIVATECHAIN(1,0x0117)");
            m_pDSPInterFace->addCycListItem( s = "STARTCHAIN(0,1,0x0117)"); // inaktiv, prozessnr. (dummy),hauptkette 1 subkette 1 start

            m_nPMSIndex = 0;

            sl = m_ConfigData.m_sMeasSystemList.at(0).split(','); // our default system for 2 wire mode
            if (m_ConfigData.m_sM2WSystem == "pms2")
            {
                sl = m_ConfigData.m_sMeasSystemList.at(1).split(',');
                m_nPMSIndex = 1;
            }
            if (m_ConfigData.m_sM2WSystem == "pms3")
            {
                sl = m_ConfigData.m_sMeasSystemList.at(2).split(',');
                m_nPMSIndex = 2;
            }

            // first we set all our actual values to 0
            m_pDSPInterFace->addCycListItem( s = "SETVAL(VALPQS,0.0)");
            m_pDSPInterFace->addCycListItem( s = "SETVAL(VALPQS+1,0.0)");
            m_pDSPInterFace->addCycListItem( s = "SETVAL(VALPQS+2,0.0)");

            // and then we compute the 2 wire power values for the selected system
            m_pDSPInterFace->addCycListItem( s = QString("COPYDATA(CH%1,0,MEASSIGNAL1)").arg(m_measChannelInfoHash.value(sl.at(0)).dspChannelNr));
            m_pDSPInterFace->addCycListItem( s = QString("COPYDATA(CH%1,0,MEASSIGNAL2)").arg(m_measChannelInfoHash.value(sl.at(1)).dspChannelNr));
            m_pDSPInterFace->addCycListItem( s = QString("MULCCV(MEASSIGNAL1,MEASSIGNAL2,VALPQS+%1)").arg(m_nPMSIndex));

            m_pDSPInterFace->addCycListItem( s = "STOPCHAIN(1,0x0117)"); // ende prozessnr., hauptkette 1 subkette 1
            break;
        }

        case m2lb:
        {
            m_pDSPInterFace->addCycListItem( s = "ACTIVATECHAIN(1,0x0118)");
            m_pDSPInterFace->addCycListItem( s = QString("TESTVCSKIPEQ(MMODE,%1)").arg(mmode));
            m_pDSPInterFace->addCycListItem( s = "DEACTIVATECHAIN(1,0x0118)");
            m_pDSPInterFace->addCycListItem( s = "STARTCHAIN(0,1,0x0118)"); // inaktiv, prozessnr. (dummy),hauptkette 1 subkette 1 start

            m_nPMSIndex = 0;

            sl = m_ConfigData.m_sMeasSystemList.at(0).split(','); // our default system for 2 wire mode
            if (m_ConfigData.m_sM2WSystem == "pms2")
            {
                sl = m_ConfigData.m_sMeasSystemList.at(1).split(',');
                m_nPMSIndex = 1;
            }
            if (m_ConfigData.m_sM2WSystem == "pms3")
            {
                sl = m_ConfigData.m_sMeasSystemList.at(2).split(',');
                m_nPMSIndex = 2;
            }

            // first we set all our actual values to 0
            m_pDSPInterFace->addCycListItem( s = "SETVAL(VALPQS,0.0)");
            m_pDSPInterFace->addCycListItem( s = "SETVAL(VALPQS+1,0.0)");
            m_pDSPInterFace->addCycListItem( s = "SETVAL(VALPQS+2,0.0)");

            // and then we compute the 2 wire power values for the selected system
            m_pDSPInterFace->addCycListItem( s = QString("COPYDATA(CH%1,0,MEASSIGNAL1)").arg(m_measChannelInfoHash.value(sl.at(0)).dspChannelNr));
            m_pDSPInterFace->addCycListItem( s = QString("COPYDATA(CH%1,0,MEASSIGNAL2)").arg(m_measChannelInfoHash.value(sl.at(1)).dspChannelNr));

            m_pDSPInterFace->addCycListItem( s = QString("DFT(1,MEASSIGNAL1,TEMP1)"));
            m_pDSPInterFace->addCycListItem( s = QString("DFT(1,MEASSIGNAL2,TEMP2)"));
            m_pDSPInterFace->addCycListItem( s = QString("MULVVV(TEMP1,TEMP2+1,VALPQS+%1)").arg(m_nPMSIndex));
            m_pDSPInterFace->addCycListItem( s = QString("MULVVV(TEMP2,TEMP1+1,TEMP1)"));
            m_pDSPInterFace->addCycListItem( s = QString("SUBVVV(TEMP1,VALPQS+%1,VALPQS+%2)").arg(m_nPMSIndex).arg(m_nPMSIndex));
            m_pDSPInterFace->addCycListItem( s = QString("MULVVV(FAK,VALPQS+%1,VALPQS+%2)").arg(m_nPMSIndex).arg(m_nPMSIndex));

            //m_pDSPInterFace->addCycListItem( s = "ROTATE(MEASSIGNAL2,270.0)");
            //m_pDSPInterFace->addCycListItem( s = QString("MULCCV(MEASSIGNAL1,MEASSIGNAL2,VALPQS+%1)").arg(m_nPMSIndex));

            m_pDSPInterFace->addCycListItem( s = "STOPCHAIN(1,0x0118)"); // ende prozessnr., hauptkette 1 subkette 1
            break;
        }

        case m2ls:
        {
            m_pDSPInterFace->addCycListItem( s = "ACTIVATECHAIN(1,0x0119)");
            m_pDSPInterFace->addCycListItem( s = QString("TESTVCSKIPEQ(MMODE,%1)").arg(mmode));
            m_pDSPInterFace->addCycListItem( s = "DEACTIVATECHAIN(1,0x0119)");
            m_pDSPInterFace->addCycListItem( s = "STARTCHAIN(0,1,0x0119)"); // inaktiv, prozessnr. (dummy),hauptkette 1 subkette 1 start

            QStringList sl;
            m_nPMSIndex = 0;

            sl = m_ConfigData.m_sMeasSystemList.at(0).split(','); // our default system for 2 wire mode
            if (m_ConfigData.m_sM2WSystem == "pms2")
            {
                sl = m_ConfigData.m_sMeasSystemList.at(1).split(',');
                m_nPMSIndex = 1;
            }
            if (m_ConfigData.m_sM2WSystem == "pms3")
            {
                sl = m_ConfigData.m_sMeasSystemList.at(2).split(',');
                m_nPMSIndex = 2;
            }

            // first we set all our actual values to 0
            m_pDSPInterFace->addCycListItem( s = "SETVAL(VALPQS,0.0)");
            m_pDSPInterFace->addCycListItem( s = "SETVAL(VALPQS+1,0.0)");
            m_pDSPInterFace->addCycListItem( s = "SETVAL(VALPQS+2,0.0)");

            // and then we compute the 2 wire power values for the selected system
            m_pDSPInterFace->addCycListItem( s = QString("COPYDATA(CH%1,0,MEASSIGNAL1)").arg(m_measChannelInfoHash.value(sl.at(0)).dspChannelNr));
            m_pDSPInterFace->addCycListItem( s = "RMS(MEASSIGNAL1,TEMP1)");
            m_pDSPInterFace->addCycListItem( s = QString("COPYDATA(CH%1,0,MEASSIGNAL1)").arg(m_measChannelInfoHash.value(sl.at(1)).dspChannelNr));
            m_pDSPInterFace->addCycListItem( s = "RMS(MEASSIGNAL1,TEMP2)");
            m_pDSPInterFace->addCycListItem( s = QString("MULVVV(TEMP1,TEMP2,VALPQS+%1)").arg(m_nPMSIndex));

            m_pDSPInterFace->addCycListItem( s = "STOPCHAIN(1,0x0119)"); // ende prozessnr., hauptkette 1 subkette 1
            break;
        }

        case m2lsg:
        {
            m_pDSPInterFace->addCycListItem( s = "ACTIVATECHAIN(1,0x0120)");
            m_pDSPInterFace->addCycListItem( s = QString("TESTVCSKIPEQ(MMODE,%1)").arg(mmode));
            m_pDSPInterFace->addCycListItem( s = "DEACTIVATECHAIN(1,0x0120)");
            m_pDSPInterFace->addCycListItem( s = "STARTCHAIN(0,1,0x0120)"); // inaktiv, prozessnr. (dummy),hauptkette 1 subkette 1 start

            QStringList sl;
            m_nPMSIndex = 0;

            sl = m_ConfigData.m_sMeasSystemList.at(0).split(','); // our default system for 2 wire mode
            if (m_ConfigData.m_sM2WSystem == "pms2")
            {
                sl = m_ConfigData.m_sMeasSystemList.at(1).split(',');
                m_nPMSIndex = 1;
            }
            if (m_ConfigData.m_sM2WSystem == "pms3")
            {
                sl = m_ConfigData.m_sMeasSystemList.at(2).split(',');
                m_nPMSIndex = 2;
            }

            // first we set all our actual values to 0
            m_pDSPInterFace->addCycListItem( s = "SETVAL(VALPQS,0.0)");
            m_pDSPInterFace->addCycListItem( s = "SETVAL(VALPQS+1,0.0)");
            m_pDSPInterFace->addCycListItem( s = "SETVAL(VALPQS+2,0.0)");

            // and then we compute the 2 wire power values for the selected system
            m_pDSPInterFace->addCycListItem( s = QString("COPYDATA(CH%1,0,MEASSIGNAL1)").arg(m_measChannelInfoHash.value(sl.at(0)).dspChannelNr));
            m_pDSPInterFace->addCycListItem( s = QString("COPYDATA(CH%1,0,MEASSIGNAL2)").arg(m_measChannelInfoHash.value(sl.at(1)).dspChannelNr));
            m_pDSPInterFace->addCycListItem( s = "MULCCV(MEASSIGNAL1,MEASSIGNAL2,TEMP1)"); // P
            m_pDSPInterFace->addCycListItem( s = "ROTATE(MEASSIGNAL2,270.0)");
            m_pDSPInterFace->addCycListItem( s = "MULCCV(MEASSIGNAL1,MEASSIGNAL2,TEMP1)"); // Q
            m_pDSPInterFace->addCycListItem( s = QString("ADDVVG(TEMP1,TEMP2,VALPQS+%1)").arg(m_nPMSIndex));

            m_pDSPInterFace->addCycListItem( s = "STOPCHAIN(1,0x0120)"); // ende prozessnr., hauptkette 1 subkette 1
            break;
        }
        }

    }

    // we have to compute sum of our power systems
    m_pDSPInterFace->addCycListItem( s = "ADDVVV(VALPQS,VALPQS+1,VALPQS+3)");
    m_pDSPInterFace->addCycListItem( s = "ADDVVV(VALPQS+2,VALPQS+3,VALPQS+3)");

    // and filter all our values
    m_pDSPInterFace->addCycListItem( s = QString("AVERAGE1(4,VALPQS,FILTER)")); // we add results to filter


    // so... let's now set our frequency outputs if he have some
    if (m_ConfigData.m_sFreqActualizationMode == "signalperiod")
    {
        if (m_ConfigData.m_nFreqOutputCount > 0)
        {
            for (int i = 0; i < m_ConfigData.m_nFreqOutputCount; i++)
            {
                // which actualvalue do we take as source (offset)
                quint8 actvalueIndex = m_ConfigData.m_FreqOutputConfList.at(i).m_nSource;
                QString foutSystemName =  m_ConfigData.m_FreqOutputConfList.at(i).m_sName;
                // here we set abs, plus or minus and which frequency output has to be set
                quint16 freqpar = m_ConfigData.m_FreqOutputConfList.at(i).m_nFoutMode + (m_FoutInfoHash[foutSystemName].dspFoutChannel << 8);
                // frequenzausgang berechnen lassen
                m_pDSPInterFace->addCycListItem( s = QString("CMPCLK(%1,VALPQS+%2,FREQSCALE+%3)")
                                                 .arg(freqpar)
                                                 .arg(actvalueIndex)
                                                 .arg(i));
            }
        }
    }

    if (m_ConfigData.m_sIntegrationMode == "time")
    {
        m_pDSPInterFace->addCycListItem( s = "TESTTIMESKIPNEX(TISTART,TIPAR)");
        m_pDSPInterFace->addCycListItem( s = "ACTIVATECHAIN(1,0x0102)");

        m_pDSPInterFace->addCycListItem( s = "STARTCHAIN(0,1,0x0102)");
            m_pDSPInterFace->addCycListItem( s = "GETSTIME(TISTART)"); // set new system time
            m_pDSPInterFace->addCycListItem( s = QString("CMPAVERAGE1(4,FILTER,VALPQSF)"));
            m_pDSPInterFace->addCycListItem( s = QString("CLEARN(%1,FILTER)").arg(2*4+1) );
            m_pDSPInterFace->addCycListItem( s = QString("DSPINTTRIGGER(0x0,0x%1)").arg(irqNr)); // send interrupt to module

            if (m_ConfigData.m_sFreqActualizationMode == "integrationtime")
            {
                if (m_ConfigData.m_nFreqOutputCount > 0)
                {
                    for (int i = 0; i < m_ConfigData.m_nFreqOutputCount; i++)
                    {
                        // which actualvalue do we take as source (offset)
                        quint8 actvalueIndex = m_ConfigData.m_FreqOutputConfList.at(i).m_nSource;
                        QString foutSystemName =  m_ConfigData.m_FreqOutputConfList.at(i).m_sName;
                        // here we set abs, plus or minus and which frequency output has to be set
                        quint16 freqpar = m_ConfigData.m_FreqOutputConfList.at(i).m_nFoutMode + (m_FoutInfoHash[foutSystemName].dspFoutChannel << 8);
                        // frequenzausgang berechnen lassen
                        m_pDSPInterFace->addCycListItem( s = QString("CMPCLK(%1,VALPQSF+%2,FREQSCALE+%3)")
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
            m_pDSPInterFace->addCycListItem( s = QString("CMPAVERAGE1(4,FILTER,VALPQSF)"));
            m_pDSPInterFace->addCycListItem( s = QString("CLEARN(%1,FILTER)").arg(2*4+1) );
            m_pDSPInterFace->addCycListItem( s = QString("DSPINTTRIGGER(0x0,0x%1)").arg(irqNr)); // send interrupt to module

            if (m_ConfigData.m_sFreqActualizationMode == "integrationtime")
            {
                if (m_ConfigData.m_nFreqOutputCount > 0)
                {
                    for (int i = 0; i < m_ConfigData.m_nFreqOutputCount; i++)
                    {
                        // which actualvalue do we take as source (offset)
                        quint8 actvalueIndex = m_ConfigData.m_FreqOutputConfList.at(i).m_nSource;
                        QString foutSystemName =  m_ConfigData.m_FreqOutputConfList.at(i).m_sName;
                        // here we set abs, plus or minus and which frequency output has to be set
                        quint16 freqpar = m_ConfigData.m_FreqOutputConfList.at(i).m_nFoutMode + (m_FoutInfoHash[foutSystemName].dspFoutChannel << 8);
                        // frequenzausgang berechnen lassen
                        m_pDSPInterFace->addCycListItem( s = QString("CMPCLK(%1,VALPQSF+%2,FREQSCALE+%3)")
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


void cPower1ModuleMeasProgram::deleteDspCmdList()
{
    m_pDSPInterFace->clearCmdList();
}


void cPower1ModuleMeasProgram::catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer)
{
    bool ok;

    if (msgnr == 0) // 0 was reserved for async. messages
    {
        QString sintnr;
        // qDebug() << "meas program interrupt";
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
            if (!m_readUrValueMachine.isRunning())
                m_readUrValueMachine.start();
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
#ifdef DEBUG
                    qDebug() << rmidentErrMSG;
#endif
                    emit activationError();
                }
                break;

            case setchannelrangenotifier:
                if (reply == ack) // we only continue pcb server acknowledges
                {
                    m_NotifierInfoHash[notifierNr] = infoRead;
                    emit activationContinue();
                }
                else
                {
                    emit errMsg((tr(registerpcbnotifierErrMsg)));
    #ifdef DEBUG
                    qDebug() << registerpcbnotifierErrMsg;
    #endif
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
#ifdef DEBUG
                    qDebug() << readrangeurvalueErrMsg;
#endif
                    emit activationError();
                }
                break;
            }

            case setfrequencyscales:
                if (reply == ack) // we ignore ack
                    emit activationContinue();
                else
                {
                    emit errMsg((tr(writedspmemoryErrMsg)));
#ifdef DEBUG
                    qDebug() << writedspmemoryErrMsg;
#endif
                    emit executionError();
                }
                break;

            case claimpgrmem:
                if (reply == ack) // we only continue if resource manager acknowledges
                    emit activationContinue();
                else
                {
                    emit errMsg((tr(claimresourceErrMsg)));
#ifdef DEBUG
                    qDebug() << claimresourceErrMsg;
#endif
                    emit activationError();
                }
                break;

            case claimusermem:
                if (reply == ack) // we only continue if resource manager acknowledges
                    emit activationContinue();
                else
                {
                    emit errMsg((tr(claimresourceErrMsg)));
#ifdef DEBUG
                    qDebug() << claimresourceErrMsg;
#endif
                    emit activationError();
                }
                break;

            case varlist2dsp:
                if (reply == ack) // we only continue if resource manager acknowledges
                    emit activationContinue();
                else
                {
                    emit errMsg((tr(dspvarlistwriteErrMsg)));
#ifdef DEBUG
                    qDebug() << dspvarlistwriteErrMsg;
#endif
                    emit activationError();
                }
                break;

            case cmdlist2dsp:
                if (reply == ack) // we only continue if resource manager acknowledges
                    emit activationContinue();
                else
                {
                    emit errMsg((tr(dspcmdlistwriteErrMsg)));
#ifdef DEBUG
                    qDebug() << dspcmdlistwriteErrMsg;
#endif
                    emit activationError();
                }
                break;

            case activatedsp:
                if (reply == ack) // we only continue if resource manager acknowledges
                    emit activationContinue();
                else
                {
                    emit errMsg((tr(dspactiveErrMsg)));
#ifdef DEBUG
                    qDebug() << dspactiveErrMsg;
#endif
                    emit activationError();
                }
                break;

            case readresourcetypes:
                ok = false;
                if ((reply == ack) && (answer.toString().contains("SENSE"))) // we need sense  at least
                {
                    if (m_ConfigData.m_nFreqOutputCount > 0)
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
#ifdef DEBUG
                    qDebug() << resourcetypeErrMsg;
#endif
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
#ifdef DEBUG
                        qDebug() << resourceErrMsg;
#endif
                        emit activationError();
                    }
                }
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
#ifdef DEBUG
                        qDebug() << resourceErrMsg;
#endif
                        emit activationError();
                    }
                }
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

            case claimresourcesource:
            if (reply == ack)
                emit activationContinue();
            else
            {
                emit errMsg((tr(claimresourceErrMsg)));
#ifdef DEBUG
                qDebug() << claimresourceErrMsg;
#endif
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
    #ifdef DEBUG
                qDebug() << readsamplerateErrMsg;
    #endif
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
#ifdef DEBUG
                    qDebug() << readaliasErrMsg;
#endif
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
#ifdef DEBUG
                    qDebug() << readunitErrMsg;
#endif
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
#ifdef DEBUG
                    qDebug() << readdspchannelErrMsg;
#endif
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
#ifdef DEBUG
                    qDebug() << readaliasErrMsg;
#endif
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
#ifdef DEBUG
                    qDebug() << readdspchannelErrMsg;
#endif
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
#ifdef DEBUG
                    qDebug() << readFormFactorErrMsg;
#endif
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
#ifdef DEBUG
                    qDebug() << writedspmemoryErrMsg;
#endif
                    emit executionError();
                }
                break;

            case deactivatedsp:
                if (reply == ack) // we only continue if resource manager acknowledges
                    emit deactivationContinue();
                else
                {
                    emit errMsg((tr(dspdeactiveErrMsg)));
#ifdef DEBUG
                    qDebug() << dspdeactiveErrMsg;
#endif
                    emit deactivationError();
                }
                break;
            case freepgrmem:
                if (reply == ack) // we only continue if resource manager acknowledges
                    emit deactivationContinue();
                else
                {
                    emit errMsg((tr(freeresourceErrMsg)));
#ifdef DEBUG
                    qDebug() << freeresourceErrMsg;
#endif
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
#ifdef DEBUG
                    qDebug() << freeresourceErrMsg;
#endif
                    emit deactivationError();
                }
                break;

            case unregisterrangenotifiers:
                if (reply == ack) // we only continue pcb server acknowledges
                    emit deactivationContinue();
                else
                {
                    emit errMsg((tr(unregisterpcbnotifierErrMsg)));
    #ifdef DEBUG
                    qDebug() << unregisterpcbnotifierErrMsg;
    #endif
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
#ifdef DEBUG
                    qDebug() << dataaquisitionErrMsg;
#endif
                    emit executionError(); // but we send error message
                }
                break;
            }
        }
    }
}


void cPower1ModuleMeasProgram::setActualValuesNames()
{
    QString powIndicator = "123S";

    cMeasModeInfo mminfo = m_MeasuringModeInfoHash[m_ConfigData.m_sMeasuringMode.m_sValue];

    for (int i = 0; i < 4; i++)
    {
        m_ActValueList.at(i)->setChannelName(QString("%1%2").arg(mminfo.getActvalName()).arg(powIndicator[i]));
        m_ActValueList.at(i)->setUnit(mminfo.getUnitName());
    }
}


void cPower1ModuleMeasProgram::setSCPIMeasInfo()
{
    cSCPIInfo* pSCPIInfo;

    for (int i = 0; i < 4; i++)
    {
        pSCPIInfo = new cSCPIInfo("MEASURE", m_ActValueList.at(i)->getChannelName(), "8", m_ActValueList.at(i)->getName(), "0", m_ActValueList.at(i)->getUnit());
        m_ActValueList.at(i)->setSCPIInfo(pSCPIInfo);
    }
}


bool cPower1ModuleMeasProgram::is2WireMode()
{
    int mm = m_MeasuringModeInfoHash[m_ConfigData.m_sMeasuringMode.m_sValue].getCode();
    return ((mm == m2lw) || (mm == m2lb) || (mm == m2ls) || (mm == m2lsg));
}


void cPower1ModuleMeasProgram::setInterfaceActualValues(QVector<float> *actualValues)
{
    if (m_bActive) // maybe we are deactivating !!!!
    {
        for (int i = 0; i < 4; i++)
            m_ActValueList.at(i)->setValue(QVariant((double)actualValues->at(i)));
    }
}


void cPower1ModuleMeasProgram::resourceManagerConnect()
{
    // as this is our entry point when activating the module, we do some initialization first
    m_measChannelInfoHash.clear(); // we build up a new channel info hash
    cMeasChannelInfo mi;
    mi.pcbServersocket = m_ConfigData.m_PCBServerSocket; // the default from configuration file
    for (int i = 0; i < m_ConfigData.m_sMeasSystemList.count(); i++)
    {
        QStringList sl = m_ConfigData.m_sMeasSystemList.at(i).split(',');
        for (int j = 0; j < sl.count(); j++)
        {
            QString s = sl.at(j);
            if (!m_measChannelInfoHash.contains(s)) // did we find a new measuring channel ?
                m_measChannelInfoHash[s] = mi; // then lets add it
        }
    }

    m_FoutInfoHash.clear();
    cFoutInfo fi;
    fi.pcbServersocket = m_ConfigData.m_PCBServerSocket; // the default from configuration file
    if (m_ConfigData.m_nFreqOutputCount > 0)
    {
        for (int i = 0; i < m_ConfigData.m_nFreqOutputCount; i++)
        {
            QString name = m_ConfigData.m_FreqOutputConfList.at(i).m_sName;
            if (!m_FoutInfoHash.contains(name))
                m_FoutInfoHash[name] = fi; //
        }
    }

    m_NotifierInfoHash.clear();

    // we have to instantiate a working resource manager interface
    // so first we try to get a connection to resource manager over proxy
    m_pRMClient = m_pProxy->getConnection(m_ConfigData.m_RMSocket.m_sIP, m_ConfigData.m_RMSocket.m_nPort);
    // and then we set resource manager interface's connection
    m_pRMInterface->setClient(m_pRMClient);
    m_resourceManagerConnectState.addTransition(m_pRMClient, SIGNAL(connected()), &m_IdentifyState);
    connect(m_pRMInterface, SIGNAL(serverAnswer(quint32, quint8, QVariant)), this, SLOT(catchInterfaceAnswer(quint32, quint8, QVariant)));
    m_pProxy->startConnection(m_pRMClient);
}


void cPower1ModuleMeasProgram::sendRMIdent()
{
    m_MsgNrCmdList[m_pRMInterface->rmIdent(QString("Power1Module%1").arg(m_pModule->getModuleNr()))] = sendrmident;
}


void cPower1ModuleMeasProgram::readResourceTypes()
{
    m_MsgNrCmdList[m_pRMInterface->getResourceTypes()] = readresourcetypes;
}


void cPower1ModuleMeasProgram::readResourceSense()
{
    m_MsgNrCmdList[m_pRMInterface->getResources("SENSE")] = readresourcesense;
}


void cPower1ModuleMeasProgram::readResourceSenseInfos()
{
    infoReadList = m_measChannelInfoHash.keys(); // we have to read information for all channels in this list
    emit activationContinue();
}


void cPower1ModuleMeasProgram::readResourceSenseInfo()
{
    infoRead = infoReadList.takeLast();
    m_MsgNrCmdList[m_pRMInterface->getResourceInfo("SENSE", infoRead)] = readresourcesenseinfo;
}


void cPower1ModuleMeasProgram::readResourceSenseInfoDone()
{
    if (infoReadList.isEmpty())
        emit activationContinue();
    else
        emit activationLoop();
}


void cPower1ModuleMeasProgram::readResourceSource()
{
    if (m_ConfigData.m_nFreqOutputCount > 0) // do we have any frequency output
        m_MsgNrCmdList[m_pRMInterface->getResources("SOURCE")] = readresourcesource; // then we read the needed info
    else
        emit activationSkip(); // otherwise we will skip
}

void cPower1ModuleMeasProgram::claimResourcesSource()
{
    infoReadList = m_FoutInfoHash.keys(); // we have to read information for all freq outputs in this list
    emit activationContinue();
}


void cPower1ModuleMeasProgram::claimResourceSource()
{
    infoRead = infoReadList.takeLast();
    m_MsgNrCmdList[m_pRMInterface->setResource("SOURCE", infoRead, 1)] = claimresourcesource;
}


void cPower1ModuleMeasProgram::claimResourceSourceDone()
{
    if (infoReadList.isEmpty())
        emit activationContinue();
    else
        emit activationLoop();
}


void cPower1ModuleMeasProgram::readResourceSourceInfos()
{
    infoReadList = m_FoutInfoHash.keys(); // we have to read information for all freq outputs in this list
    emit activationContinue();
}


void cPower1ModuleMeasProgram::readResourceSourceInfo()
{
    infoRead = infoReadList.takeLast();
    m_MsgNrCmdList[m_pRMInterface->getResourceInfo("SOURCE", infoRead)] = readresourcesourceinfo;
}


void cPower1ModuleMeasProgram::readResourceSourceInfoDone()
{
    if (infoReadList.isEmpty())
        emit activationContinue();
    else
        emit activationLoop();
}


void cPower1ModuleMeasProgram::pcbserverConnect4measChannels()
{
    // we have to connect to all ports....
    infoReadList = m_measChannelInfoHash.keys(); // so first we look for our different pcb sockets for sense
    m_nConnectionCount = infoReadList.count();

    for (int i = 0; i < infoReadList.count(); i++)
    {
        QString key = infoReadList.at(i);
        cMeasChannelInfo mi = m_measChannelInfoHash.take(key);
        cSocket sock = mi.pcbServersocket;
        Zera::Proxy::cProxyClient* pcbClient = m_pProxy->getConnection(sock.m_sIP, sock.m_nPort);
        m_pcbClientList.append(pcbClient);
        Zera::Server::cPCBInterface* pcbIFace = new Zera::Server::cPCBInterface();
        m_pcbIFaceList.append(pcbIFace);
        pcbIFace->setClient(pcbClient);
        mi.pcbIFace = pcbIFace;
        m_measChannelInfoHash[key] = mi;
        connect(pcbClient, SIGNAL(connected()), this, SLOT(monitorConnection())); // here we wait until all connections are established
        connect(pcbIFace, SIGNAL(serverAnswer(quint32, quint8, QVariant)), this, SLOT(catchInterfaceAnswer(quint32, quint8, QVariant)));
        m_pProxy->startConnection(pcbClient);
    }
}


void cPower1ModuleMeasProgram::pcbserverConnect4freqChannels()
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
            Zera::Proxy::cProxyClient* pcbClient = m_pProxy->getConnection(sock.m_sIP, sock.m_nPort);
            m_pcbClientList.append(pcbClient);
            Zera::Server::cPCBInterface* pcbIFace = new Zera::Server::cPCBInterface();
            m_pcbIFaceList.append(pcbIFace);
            pcbIFace->setClient(pcbClient);
            fi.pcbIFace = pcbIFace;
            fi.name = key;
            m_FoutInfoHash[key] = fi;
            connect(pcbClient, SIGNAL(connected()), this, SLOT(monitorConnection())); // here we wait until all connections are established
            connect(pcbIFace, SIGNAL(serverAnswer(quint32, quint8, QVariant)), this, SLOT(catchInterfaceAnswer(quint32, quint8, QVariant)));
            m_pProxy->startConnection(pcbClient);
        }
    }
    else
        emit activationContinue();
}


void cPower1ModuleMeasProgram::readSampleRate()
{
    // we always take the sample count from the first channels pcb server
    m_MsgNrCmdList[m_pcbIFaceList.at(0)->getSampleRate()] = readsamplerate;
}


void cPower1ModuleMeasProgram::readSenseChannelInformation()
{
    infoReadList = m_measChannelInfoHash.keys(); // we have to read information for all channels in this list
    emit activationContinue();
}


void cPower1ModuleMeasProgram::readSenseChannelAlias()
{
    infoRead = infoReadList.takeFirst();
    m_MsgNrCmdList[m_measChannelInfoHash[infoRead].pcbIFace->getAlias(infoRead)] = readsensechannelalias;
}


void cPower1ModuleMeasProgram::readSenseChannelUnit()
{
    m_MsgNrCmdList[m_measChannelInfoHash[infoRead].pcbIFace->getUnit(infoRead)] = readsensechannelunit;
}


void cPower1ModuleMeasProgram::readSenseDspChannel()
{
    m_MsgNrCmdList[m_measChannelInfoHash[infoRead].pcbIFace->getDSPChannel(infoRead)] = readsensechanneldspchannel;
}


void cPower1ModuleMeasProgram::readSenseChannelInformationDone()
{
    if (infoReadList.isEmpty())
        emit activationContinue();
    else
        emit activationLoop();
}


void cPower1ModuleMeasProgram::readSourceChannelInformation()
{
    if (m_ConfigData.m_nFreqOutputCount > 0) // we only have to read information if really configured
    {
        infoReadList = m_FoutInfoHash.keys(); // we have to read information for all channels in this list
        emit activationContinue();
    }
    else
        emit activationSkip();
}


void cPower1ModuleMeasProgram::readSourceChannelAlias()
{
    infoRead = infoReadList.takeFirst();
    m_MsgNrCmdList[m_FoutInfoHash[infoRead].pcbIFace->getAliasSource(infoRead)] = readsourcechannelalias;
}


void cPower1ModuleMeasProgram::readSourceDspChannel()
{
    m_MsgNrCmdList[m_FoutInfoHash[infoRead].pcbIFace->getDSPChannelSource(infoRead)] = readsourcechanneldspchannel;
}


void cPower1ModuleMeasProgram::readSourceFormFactor()
{
    m_MsgNrCmdList[m_FoutInfoHash[infoRead].pcbIFace->getFormFactorSource(infoRead)] = readsourceformfactor;
}


void cPower1ModuleMeasProgram::readSourceChannelInformationDone()
{
    if (infoReadList.isEmpty())
        emit activationContinue();
    else
        emit activationLoop();
}


void cPower1ModuleMeasProgram::setSenseChannelRangeNotifiers()
{
    notifierNr = irqNr;
    infoReadList = m_measChannelInfoHash.keys(); // we have to set notifier for each channel we are working on
    emit activationContinue();
}


void cPower1ModuleMeasProgram::setSenseChannelRangeNotifier()
{
    infoRead = infoReadList.takeFirst();
    notifierNr++;
    // we will get irq+1 .. irq+6 for notification if ranges change
    m_MsgNrCmdList[ m_measChannelInfoHash[infoRead].pcbIFace->registerNotifier(QString("sens:%1:rang?").arg(infoRead), QString("%1").arg(notifierNr))] = setchannelrangenotifier;

}


void cPower1ModuleMeasProgram::setSenseChannelRangeNotifierDone()
{
    if (infoReadList.isEmpty())
        emit activationContinue();
    else
        emit activationLoop();
}


void cPower1ModuleMeasProgram::dspserverConnect()
{
    m_pDspClient = m_pProxy->getConnection(m_ConfigData.m_DSPServerSocket.m_sIP, m_ConfigData.m_DSPServerSocket.m_nPort);
    m_pDSPInterFace->setClient(m_pDspClient);
    m_dspserverConnectState.addTransition(m_pDspClient, SIGNAL(connected()), &m_claimPGRMemState);
    connect(m_pDSPInterFace, SIGNAL(serverAnswer(quint32, quint8, QVariant)), this, SLOT(catchInterfaceAnswer(quint32, quint8, QVariant)));
    m_pProxy->startConnection(m_pDspClient);
}


void cPower1ModuleMeasProgram::claimPGRMem()
{
    setDspVarList(); // first we set the var list for our dsp
    setDspCmdList(); // and the cmd list he has to work on
    m_MsgNrCmdList[m_pRMInterface->setResource("DSP1", "PGRMEMC", m_pDSPInterFace->cmdListCount())] = claimpgrmem;
}


void cPower1ModuleMeasProgram::claimUSERMem()
{
   m_MsgNrCmdList[m_pRMInterface->setResource("DSP1", "USERMEM", m_nDspMemUsed)] = claimusermem;
}


void cPower1ModuleMeasProgram::varList2DSP()
{
    m_MsgNrCmdList[m_pDSPInterFace->varList2Dsp()] = varlist2dsp;
}


void cPower1ModuleMeasProgram::cmdList2DSP()
{
    m_MsgNrCmdList[m_pDSPInterFace->cmdList2Dsp()] = cmdlist2dsp;
}


void cPower1ModuleMeasProgram::activateDSP()
{
    m_MsgNrCmdList[m_pDSPInterFace->activateInterface()] = activatedsp; // aktiviert die var- und cmd-listen im dsp
}


void cPower1ModuleMeasProgram::activateDSPdone()
{
    m_bActive = true;

    setActualValuesNames();
    setSCPIMeasInfo();

    m_pMeasureSignal->setValue(QVariant(1));

    connect(m_pIntegrationTimeParameter, SIGNAL(sigValueChanged(QVariant)), this, SLOT(newIntegrationtime(QVariant)));
    connect(m_pIntegrationPeriodParameter, SIGNAL(sigValueChanged(QVariant)), this, SLOT(newIntegrationPeriod(QVariant)));
    connect(m_pMeasuringmodeParameter, SIGNAL(sigValueChanged(QVariant)), this, SLOT(newMeasMode(QVariant)));

    readUrvalueList = m_measChannelInfoHash.keys(); // once we read all actual range urvalues
    if (!m_readUrValueMachine.isRunning())
        m_readUrValueMachine.start();

    emit activated();



}


void cPower1ModuleMeasProgram::deactivateDSP()
{
    m_bActive = false;
    m_MsgNrCmdList[m_pDSPInterFace->deactivateInterface()] = deactivatedsp; // wat wohl
}


void cPower1ModuleMeasProgram::freePGRMem()
{
    m_pProxy->releaseConnection(m_pDspClient);
    deleteDspVarList();
    deleteDspCmdList();

    m_MsgNrCmdList[m_pRMInterface->freeResource("DSP1", "PGRMEMC")] = freepgrmem;
}


void cPower1ModuleMeasProgram::freeUSERMem()
{
    m_MsgNrCmdList[m_pRMInterface->freeResource("DSP1", "USERMEM")] = freeusermem;
}


void cPower1ModuleMeasProgram::freeFreqOutputs()
{
    if (m_ConfigData.m_nFreqOutputCount > 0) // we only have to read information if really configured
    {
        infoReadList = m_FoutInfoHash.keys(); // we have to read information for all channels in this list
        emit deactivationContinue();
    }
    else
        emit deactivationSkip();
}


void cPower1ModuleMeasProgram::freeFreqOutput()
{
    infoRead = infoReadList.takeFirst();
    m_MsgNrCmdList[m_pRMInterface->freeResource("SOURCE", infoRead)] = freeresourcesource;
}


void cPower1ModuleMeasProgram::freeFreqOutputDone()
{
    if (infoReadList.isEmpty())
        emit deactivationContinue();
    else
        emit deactivationLoop();
}


void cPower1ModuleMeasProgram::resetNotifiers()
{
    m_pProxy->releaseConnection(m_pRMClient);
    infoReadList = m_measChannelInfoHash.keys();
    emit deactivationContinue();
}


void cPower1ModuleMeasProgram::resetNotifier()
{
    infoRead = infoReadList.takeFirst();
    m_MsgNrCmdList[m_measChannelInfoHash[infoRead].pcbIFace->unregisterNotifiers()] = unregisterrangenotifiers;
}


void cPower1ModuleMeasProgram::resetNotifierDone()
{
    if (infoReadList.isEmpty())
        emit deactivationContinue();
    else
        emit deactivationLoop();
}


void cPower1ModuleMeasProgram::deactivateDSPdone()
{
    if (m_pcbIFaceList.count() > 0)
    {
        for (int i = 0; i < m_pcbIFaceList.count(); i++)
        {
            m_pProxy->releaseConnection(m_pcbClientList.at(i));
            delete m_pcbIFaceList.at(i);
        }

        m_pcbClientList.clear();
        m_pcbIFaceList.clear();
    }

    disconnect(m_pRMInterface, 0, this, 0);
    disconnect(m_pDSPInterFace, 0, this, 0);

    emit deactivated();
}


void cPower1ModuleMeasProgram::dataAcquisitionDSP()
{
    m_pMeasureSignal->setValue(QVariant(0));
    m_MsgNrCmdList[m_pDSPInterFace->dataAcquisition(m_pActualValuesDSP)] = dataaquistion; // we start our data aquisition now
}


void cPower1ModuleMeasProgram::dataReadDSP()
{
    if (m_bActive)
    {
        m_pDSPInterFace->getData(m_pActualValuesDSP, m_ModuleActualValues); // we fetch our actual values
        emit actualValues(&m_ModuleActualValues); // and send them
        m_pMeasureSignal->setValue(QVariant(1)); // signal measuring

#ifdef DEBUG
        QString powIndicator = "123S";
        QString s, ts;

        for (int i = 0; i < 4; i++) // we have fixed 4 values
        {
            cMeasModeInfo mminfo  = m_MeasuringModeInfoHash[m_ConfigData.m_sMeasuringMode.m_sValue];
            ts = QString("%1%2:%3[%4];").arg(mminfo.getActvalName())
                    .arg(powIndicator[i])
                    .arg(m_ModuleActualValues.at(i))
                    .arg(mminfo.getUnitName());

            s += ts;
        }

        qDebug() << s;
#endif
    }
}


void cPower1ModuleMeasProgram::readUrvalue()
{
    readUrvalueInfo = readUrvalueList.takeFirst(); // we have the channel information now
    m_MsgNrCmdList[m_measChannelInfoHash[readUrvalueInfo].pcbIFace->getUrvalue(readUrvalueInfo)] = readurvalue;
}


void cPower1ModuleMeasProgram::readUrvalueDone()
{
    if (readUrvalueList.isEmpty())
        emit activationContinue();
    else
        emit activationLoop();
}


void cPower1ModuleMeasProgram::setFrequencyScales()
{
    double d;
    QStringList sl;
    umax = imax = 0.0;

    if (m_ConfigData.m_nFreqOutputCount > 0) // we only do something here if we really have a frequency output
    {

        if (is2WireMode()) // in case we are in 2 wire mode we take umax imyx from driving system
        {
            sl = m_ConfigData.m_sMeasSystemList.at(m_nPMSIndex).split(',');
            umax = m_measChannelInfoHash[sl.at(0)].m_fUrValue;
            imax = m_measChannelInfoHash[sl.at(1)].m_fUrValue;
        }
        else // we have to consider all channels
            for (int i = 0; i < m_ConfigData.m_sMeasSystemList.count(); i++)
            {

                sl = m_ConfigData.m_sMeasSystemList.at(i).split(',');
                if ((d = m_measChannelInfoHash[sl.at(0)].m_fUrValue) > umax)
                    umax = d;
                if ((d = m_measChannelInfoHash[sl.at(1)].m_fUrValue) > imax)
                    imax = d;
            }

        QString datalist = "FREQSCALE:";

        double cfak;
        if (is2WireMode())
            cfak = 1.0;
        else
            cfak = 3.0;

        for (int i = 0; i< m_ConfigData.m_nFreqOutputCount; i++)
        {
            double frScale;
            cFoutInfo fi = m_FoutInfoHash[m_ConfigData.m_FreqOutputConfList.at(i).m_sName];
            frScale = fi.formFactor * m_ConfigData.m_nNominalFrequency / (cfak * umax * imax);
            datalist += QString("%1,").arg(frScale, 0, 'g', 7);
        }

        datalist.resize(datalist.size()-1);
        datalist += ";";

        m_pDSPInterFace->setVarData(m_pfreqScaleDSP, datalist);
        m_MsgNrCmdList[m_pDSPInterFace->dspMemoryWrite(m_pfreqScaleDSP)] = setfrequencyscales;

    }
    else // otherwise we have to continue "manually"
        emit activationContinue();
}


void cPower1ModuleMeasProgram::setFoutConstants()
{
    double constant;
    double cfak;

    if (is2WireMode())
        cfak = 1.0;
    else
        cfak = 3.0;

    constant = m_ConfigData.m_nNominalFrequency * 3600.0 * 1000.0 / (cfak * umax * imax); // imp./kwh

    QList<QString> keylist = m_FoutInfoHash.keys();

    if (keylist.count() > 0)
        for (int i = 0; i < keylist.count(); i++)
        {
            cFoutInfo fi = m_FoutInfoHash[keylist.at(i)];
            m_MsgNrCmdList[fi.pcbIFace->setConstantSource(fi.name, constant)] = writeparameter;

        }
}


void cPower1ModuleMeasProgram::newIntegrationtime(QVariant ti)
{
    bool ok;
    m_ConfigData.m_fMeasIntervalTime.m_fValue = ti.toDouble(&ok);
    if (m_ConfigData.m_sIntegrationMode == "time")
    {
        if (m_ConfigData.m_bmovingWindow)
            m_pMovingwindowFilter->setIntegrationtime(m_ConfigData.m_fMeasIntervalTime.m_fValue);
        else
        {
            m_pDSPInterFace->setVarData(m_pParameterDSP, QString("TIPAR:%1;TISTART:%2;MMODE:%3")
                                                            .arg(m_ConfigData.m_fMeasIntervalTime.m_fValue*1000)
                                                            .arg(0)
                                                            .arg(m_MeasuringModeInfoHash[m_ConfigData.m_sMeasuringMode.m_sValue].getCode()), DSPDATA::dInt);
            m_MsgNrCmdList[m_pDSPInterFace->dspMemoryWrite(m_pParameterDSP)] = writeparameter;
        }
    }
}


void cPower1ModuleMeasProgram::newIntegrationPeriod(QVariant period)
{
    bool ok;
    m_ConfigData.m_nMeasIntervalPeriod.m_nValue = period.toInt(&ok);
    if (m_ConfigData.m_sIntegrationMode == "period")
    {
        m_pDSPInterFace->setVarData(m_pParameterDSP, QString("TIPAR:%1;TISTART:%2;MMODE:%3")
                                                        .arg(m_ConfigData.m_nMeasIntervalPeriod.m_nValue)
                                                        .arg(0)
                                                        .arg(m_MeasuringModeInfoHash[m_ConfigData.m_sMeasuringMode.m_sValue].getCode()), DSPDATA::dInt);
        m_MsgNrCmdList[m_pDSPInterFace->dspMemoryWrite(m_pParameterDSP)] = writeparameter;
    }
}


void cPower1ModuleMeasProgram::newMeasMode(QVariant mm)
{
    m_ConfigData.m_sMeasuringMode.m_sValue = mm.toString();

    if (m_ConfigData.m_sIntegrationMode == "time")
    {
        if (m_ConfigData.m_bmovingWindow)
            m_pDSPInterFace->setVarData(m_pParameterDSP, QString("TIPAR:%1;TISTART:%2;MMODE:%3")
                                                            .arg(m_ConfigData.m_fmovingwindowInterval*1000)
                                                            .arg(0)
                                                            .arg(m_MeasuringModeInfoHash[m_ConfigData.m_sMeasuringMode.m_sValue].getCode()), DSPDATA::dInt);
        else
            m_pDSPInterFace->setVarData(m_pParameterDSP, QString("TIPAR:%1;TISTART:%2;MMODE:%3")
                                                            .arg(m_ConfigData.m_fMeasIntervalTime.m_fValue*1000)
                                                            .arg(0)
                                                            .arg(m_MeasuringModeInfoHash[m_ConfigData.m_sMeasuringMode.m_sValue].getCode()), DSPDATA::dInt);
    }
    else
        m_pDSPInterFace->setVarData(m_pParameterDSP, QString("TIPAR:%1;TISTART:%2;MMODE:%3")
                                                        .arg(m_ConfigData.m_nMeasIntervalPeriod.m_nValue)
                                                        .arg(0)
                                                        .arg(m_MeasuringModeInfoHash[m_ConfigData.m_sMeasuringMode.m_sValue].getCode()), DSPDATA::dInt);

    m_MsgNrCmdList[m_pDSPInterFace->dspMemoryWrite(m_pParameterDSP)] = writeparameter;
    setActualValuesNames();
}

}





