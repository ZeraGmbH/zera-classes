#include "spm1modulemeasprogram.h"
#include "spm1module.h"
#include "spm1moduleconfiguration.h"
#include "errormessages.h"
#include <doublevalidator.h>
#include <intvalidator.h>
#include <stringvalidator.h>
#include <reply.h>
#include <proxy.h>
#include <unithelper.h>
#include <math.h>

namespace SPM1MODULE
{

cSpm1ModuleMeasProgram::cSpm1ModuleMeasProgram(cSpm1Module* module, std::shared_ptr<cBaseModuleConfiguration> pConfiguration)
    :cBaseMeasProgram(pConfiguration), m_pModule(module)
{
    // we have to instantiate a working resource manager and secserver interface
    m_pSECInterface = new Zera::cSECInterface();
    m_pPCBInterface = new Zera::cPCBInterface();

    m_IdentifyState.addTransition(this, &cSpm1ModuleMeasProgram::activationContinue, &m_testSEC1ResourceState);
    m_testSEC1ResourceState.addTransition(this, &cSpm1ModuleMeasProgram::activationContinue, &m_setECResourceState); // test presence of sec1 resource
    m_setECResourceState.addTransition(this, &cSpm1ModuleMeasProgram::activationContinue, &m_readResourceTypesState); // claim 3 ecalculator units
    m_readResourceTypesState.addTransition(this, &cSpm1ModuleMeasProgram::activationContinue, &m_readResourcesState); // read all resources types
    m_readResourcesState.addTransition(this, &cSpm1ModuleMeasProgram::activationContinue, &m_readResourceState); // init read resources
    m_readResourceState.addTransition(this, &cSpm1ModuleMeasProgram::activationLoop, &m_readResourceState); // read their resources into list
    m_readResourceState.addTransition(this, &cSpm1ModuleMeasProgram::activationContinue, &m_testSpmInputsState); // go on if done
    m_testSpmInputsState.addTransition(this, &cSpm1ModuleMeasProgram::activationContinue, &m_ecalcServerConnectState); // test all configured Inputs
    //m_ecalcServerConnectState.addTransition(this, &cSpm1ModuleMeasProgram::activationContinue, &m_fetchECalcUnitsState); // connect to ecalc server
    //transition from this state to m_fetch....is done in ecalcServerConnect
    m_fetchECalcUnitsState.addTransition(this, &cSpm1ModuleMeasProgram::activationContinue, &m_pcbServerConnectState); // connect to pcbserver


    //m_pcbServerConnectState.addTransition(this, &cSpm1ModuleMeasProgram::activationContinue, &m_pcbServerConnectState);
    //transition from this state to m_readREFInputsState....is done in pcbServerConnect
    m_readREFInputsState.addTransition(this, &cSpm1ModuleMeasProgram::activationContinue, &m_readREFInputAliasState);
    m_readREFInputAliasState.addTransition(this, &cSpm1ModuleMeasProgram::activationContinue, &m_readREFInputDoneState);
    m_readREFInputDoneState.addTransition(this, &cSpm1ModuleMeasProgram::activationLoop, &m_readREFInputAliasState);
    m_readREFInputDoneState.addTransition(this, &cSpm1ModuleMeasProgram::activationContinue, &m_setpcbREFConstantNotifierState);

    m_setpcbREFConstantNotifierState.addTransition(this, &cSpm1ModuleMeasProgram::activationContinue, &m_setsecINTNotifierState);
    m_setsecINTNotifierState.addTransition(this, &cSpm1ModuleMeasProgram::activationContinue, &m_activationDoneState);

    m_activationMachine.addState(&resourceManagerConnectState);
    m_activationMachine.addState(&m_IdentifyState);
    m_activationMachine.addState(&m_testSEC1ResourceState);
    m_activationMachine.addState(&m_setECResourceState);
    m_activationMachine.addState(&m_readResourceTypesState);
    m_activationMachine.addState(&m_readResourcesState);
    m_activationMachine.addState(&m_readResourceState);
    m_activationMachine.addState(&m_testSpmInputsState);
    m_activationMachine.addState(&m_ecalcServerConnectState);
    m_activationMachine.addState(&m_fetchECalcUnitsState);
    m_activationMachine.addState(&m_pcbServerConnectState);
    m_activationMachine.addState(&m_readREFInputsState);
    m_activationMachine.addState(&m_readREFInputAliasState);
    m_activationMachine.addState(&m_readREFInputDoneState);
    m_activationMachine.addState(&m_setpcbREFConstantNotifierState);
    m_activationMachine.addState(&m_setsecINTNotifierState);
    m_activationMachine.addState(&m_activationDoneState);

    m_activationMachine.setInitialState(&resourceManagerConnectState);

    connect(&resourceManagerConnectState, &QState::entered, this, &cSpm1ModuleMeasProgram::resourceManagerConnect);
    connect(&m_IdentifyState, &QState::entered, this, &cSpm1ModuleMeasProgram::sendRMIdent);
    connect(&m_testSEC1ResourceState, &QState::entered, this, &cSpm1ModuleMeasProgram::testSEC1Resource);
    connect(&m_setECResourceState, &QState::entered, this, &cSpm1ModuleMeasProgram::setECResource);
    connect(&m_readResourceTypesState, &QState::entered, this, &cSpm1ModuleMeasProgram::readResourceTypes);
    connect(&m_readResourcesState, &QState::entered, this, &cSpm1ModuleMeasProgram::readResources);
    connect(&m_readResourceState, &QState::entered, this, &cSpm1ModuleMeasProgram::readResource);
    connect(&m_testSpmInputsState, &QState::entered, this, &cSpm1ModuleMeasProgram::testSpmInputs);
    connect(&m_ecalcServerConnectState, &QState::entered, this, &cSpm1ModuleMeasProgram::ecalcServerConnect);
    connect(&m_fetchECalcUnitsState, &QState::entered, this, &cSpm1ModuleMeasProgram::fetchECalcUnits);
    connect(&m_pcbServerConnectState, &QState::entered, this, &cSpm1ModuleMeasProgram::pcbServerConnect);
    connect(&m_readREFInputsState, &QState::entered, this, &cSpm1ModuleMeasProgram::readREFInputs);
    connect(&m_readREFInputAliasState, &QState::entered, this, &cSpm1ModuleMeasProgram::readREFInputAlias);
    connect(&m_readREFInputDoneState, &QState::entered, this, &cSpm1ModuleMeasProgram::readREFInputDone);
    connect(&m_setpcbREFConstantNotifierState, &QState::entered, this, &cSpm1ModuleMeasProgram::setpcbREFConstantNotifier);
    connect(&m_setsecINTNotifierState, &QState::entered, this, &cSpm1ModuleMeasProgram::setsecINTNotifier);
    connect(&m_activationDoneState, &QState::entered, this, &cSpm1ModuleMeasProgram::activationDone);

    // setting up statemachine to free the occupied resources
    m_stopECalculatorState.addTransition(this, &cSpm1ModuleMeasProgram::deactivationContinue, &m_freeECalculatorState);
    m_freeECalculatorState.addTransition(this, &cSpm1ModuleMeasProgram::deactivationContinue, &m_freeECResource);
    m_freeECResource.addTransition(this, &cSpm1ModuleMeasProgram::deactivationContinue, &m_deactivationDoneState);

    m_deactivationMachine.addState(&m_stopECalculatorState);
    m_deactivationMachine.addState(&m_freeECalculatorState);
    m_deactivationMachine.addState(&m_freeECResource);
    m_deactivationMachine.addState(&m_deactivationDoneState);

    m_deactivationMachine.setInitialState(&m_stopECalculatorState);

    connect(&m_stopECalculatorState, &QState::entered, this, &cSpm1ModuleMeasProgram::stopECCalculator);
    connect(&m_freeECalculatorState, &QState::entered, this, &cSpm1ModuleMeasProgram::freeECalculator);
    connect(&m_freeECResource, &QState::entered, this, &cSpm1ModuleMeasProgram::freeECResource);
    connect(&m_deactivationDoneState, &QState::entered, this, &cSpm1ModuleMeasProgram::deactivationDone);

    // setting up statemachine used when starting a measurement

    m_setsyncState.addTransition(this, &cSpm1ModuleMeasProgram::setupContinue, &m_setsync2State);
    m_setsync2State.addTransition(this, &cSpm1ModuleMeasProgram::setupContinue, &m_setMeaspulsesState);
    m_setMeaspulsesState.addTransition(this, &cSpm1ModuleMeasProgram::setupContinue, &m_setMasterMuxState);
    m_setMasterMuxState.addTransition(this, &cSpm1ModuleMeasProgram::setupContinue, &m_setSlaveMuxState);
    m_setSlaveMuxState.addTransition(this, &cSpm1ModuleMeasProgram::setupContinue, &m_setSlave2MuxState);
    m_setSlave2MuxState.addTransition(this, &cSpm1ModuleMeasProgram::setupContinue, &m_setMasterMeasModeState);
    m_setMasterMeasModeState.addTransition(this, &cSpm1ModuleMeasProgram::setupContinue, &m_setSlaveMeasModeState);
    m_setSlaveMeasModeState.addTransition(this, &cSpm1ModuleMeasProgram::setupContinue, &m_setSlave2MeasModeState);
    m_setSlave2MeasModeState.addTransition(this, &cSpm1ModuleMeasProgram::setupContinue, &m_enableInterruptState);
    m_enableInterruptState.addTransition(this, &cSpm1ModuleMeasProgram::setupContinue, &m_startMeasurementState);
    m_startMeasurementState.addTransition(this, &cSpm1ModuleMeasProgram::setupContinue, &m_startMeasurementDoneState);

    m_startMeasurementMachine.addState(&m_setsyncState);
    m_startMeasurementMachine.addState(&m_setsync2State);
    m_startMeasurementMachine.addState(&m_setMeaspulsesState);
    m_startMeasurementMachine.addState(&m_setMasterMuxState);
    m_startMeasurementMachine.addState(&m_setSlaveMuxState);
    m_startMeasurementMachine.addState(&m_setSlave2MuxState);
    m_startMeasurementMachine.addState(&m_setMasterMeasModeState);
    m_startMeasurementMachine.addState(&m_setSlaveMeasModeState);
    m_startMeasurementMachine.addState(&m_setSlave2MeasModeState);
    m_startMeasurementMachine.addState(&m_enableInterruptState);
    m_startMeasurementMachine.addState(&m_startMeasurementState);
    m_startMeasurementMachine.addState(&m_startMeasurementDoneState);

    m_startMeasurementMachine.setInitialState(&m_setsyncState);

    connect(&m_setsyncState, &QState::entered, this, &cSpm1ModuleMeasProgram::setSync);
    connect(&m_setsync2State, &QState::entered, this, &cSpm1ModuleMeasProgram::setSync2);
    connect(&m_setMeaspulsesState, &QState::entered, this, &cSpm1ModuleMeasProgram::setMeaspulses);
    connect(&m_setMasterMuxState, &QState::entered, this, &cSpm1ModuleMeasProgram::setMasterMux);
    connect(&m_setSlaveMuxState, &QState::entered, this, &cSpm1ModuleMeasProgram::setSlaveMux);
    connect(&m_setSlave2MuxState, &QState::entered, this, &cSpm1ModuleMeasProgram::setSlave2Mux);
    connect(&m_setMasterMeasModeState, &QState::entered, this, &cSpm1ModuleMeasProgram::setMasterMeasMode);
    connect(&m_setSlaveMeasModeState, &QState::entered, this, &cSpm1ModuleMeasProgram::setSlaveMeasMode);
    connect(&m_setSlave2MeasModeState, &QState::entered, this, &cSpm1ModuleMeasProgram::setSlave2MeasMode);
    connect(&m_enableInterruptState, &QState::entered, this, &cSpm1ModuleMeasProgram::enableInterrupt);
    connect(&m_startMeasurementState, &QState::entered, this, &cSpm1ModuleMeasProgram::startMeasurement);
    connect(&m_startMeasurementDoneState, &QState::entered, this, &cSpm1ModuleMeasProgram::startMeasurementDone);

    // setting up statemachine m_finalResultStateMachine
    // mode targeted:
    // * for interrupt handling (Interrupt is thrown on measurement finished)
    // mode non targeted (Start/Stop):
    // * on user stop this machine is started.
    m_readIntRegisterState.addTransition(this, &cSpm1ModuleMeasProgram::interruptContinue, &m_resetIntRegisterState);
    m_resetIntRegisterState.addTransition(this, &cSpm1ModuleMeasProgram::interruptContinue, &m_readFinalEnergyCounterState);
    m_readFinalEnergyCounterState.addTransition(this, &cSpm1ModuleMeasProgram::interruptContinue, &m_readFinalTimeCounterState);
    m_readFinalTimeCounterState.addTransition(this, &cSpm1ModuleMeasProgram::interruptContinue, &m_setEMResultState);

    m_finalResultStateMachine.addState(&m_readIntRegisterState);
    m_finalResultStateMachine.addState(&m_resetIntRegisterState);
    m_finalResultStateMachine.addState(&m_readFinalEnergyCounterState);
    m_finalResultStateMachine.addState(&m_readFinalTimeCounterState);
    m_finalResultStateMachine.addState(&m_setEMResultState);

    m_finalResultStateMachine.setInitialState(&m_readIntRegisterState);

    connect(&m_readIntRegisterState, &QState::entered, this, &cSpm1ModuleMeasProgram::readIntRegister);
    connect(&m_resetIntRegisterState, &QState::entered, this, &cSpm1ModuleMeasProgram::resetIntRegister);
    connect(&m_readFinalEnergyCounterState, &QState::entered, this, &cSpm1ModuleMeasProgram::readVICountact);
    connect(&m_readFinalTimeCounterState, &QState::entered, this, &cSpm1ModuleMeasProgram::readTCountact);
    connect(&m_setEMResultState, &QState::entered, this, &cSpm1ModuleMeasProgram::setEMResult);

    // we need a hash for our different power input units
    mPowerUnitFactorHash["MW"] = 1000.0;
    mPowerUnitFactorHash["kW"] = 1.0;
    mPowerUnitFactorHash["W"] = 0.001;
    mPowerUnitFactorHash["MVar"] = 1000.0;
    mPowerUnitFactorHash["kVar"] = 1.0;
    mPowerUnitFactorHash["Var"] = 0.001;
    mPowerUnitFactorHash["MVA"] = 1000.0;
    mPowerUnitFactorHash["kVA"] = 1.0;
    mPowerUnitFactorHash["VA"] = 0.001;

    m_ActualizeTimer.setInterval(m_nActualizeIntervallLowFreq);
}

cSpm1ModuleMeasProgram::~cSpm1ModuleMeasProgram()
{
    for (int i = 0; i < getConfData()->m_refInpList.count(); i++)
        delete mREFSpmInputInfoHash.take(getConfData()->m_refInpList.at(i));
    delete m_pSECInterface;
    Zera::Proxy::getInstance()->releaseConnection(m_pSECClient);
    delete m_pPCBInterface;
    Zera::Proxy::getInstance()->releaseConnection(m_pPCBClient);
}

void cSpm1ModuleMeasProgram::start()
{
}

void cSpm1ModuleMeasProgram::stop()
{
}

void cSpm1ModuleMeasProgram::generateInterface()
{
    QString s;
    QString key;

    QString modNr = QString("%1").arg(m_pModule->getModuleNr(),4,10,QChar('0'));

    m_pRefInputPar = new VfModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                              key = QString("PAR_RefInput"),
                                              QString("Ref input"),
                                              QVariant(s = "Unknown"));
    m_pRefInputPar->setSCPIInfo(new cSCPIInfo("CALCULATE", QString("%1:REFSOURCE").arg(modNr), "10", m_pRefInputPar->getName(), "0", ""));
    m_pModule->veinModuleParameterHash[key] = m_pRefInputPar; // for modules use

    m_pRefConstantPar = new VfModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                                 key = QString("PAR_RefConstant"),
                                                 QString("Ref constant"),
                                                 QVariant((double)0.0));
    m_pRefConstantPar->setSCPIInfo(new cSCPIInfo("CALCULATE", QString("%1:REFCONSTANT").arg(modNr ), "2", m_pRefConstantPar->getName(), "0", ""));
    m_pModule->veinModuleParameterHash[key] = m_pRefConstantPar; // for modules use
    // the reference constant is derived automatically from selected reference input but will be forwarded for queries
    // but so we don't need a validator

    m_pTargetedPar = new VfModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                              key = QString("PAR_Targeted"),
                                              QString("Stop mode"),
                                              QVariant((int)0));
    m_pTargetedPar->setSCPIInfo(new cSCPIInfo("CALCULATE", QString("%1:MODE").arg(modNr), "10", m_pTargetedPar->getName(), "0", ""));
    m_pModule->veinModuleParameterHash[key] = m_pTargetedPar; // for modules use
    cIntValidator *iValidator;
    iValidator = new cIntValidator(0,1,1);
    m_pTargetedPar->setValidator(iValidator);

    m_pMeasTimePar = new VfModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                              key = QString("PAR_MeasTime"),
                                              QString("Measurement time"),
                                              QVariant((quint32)10));
    m_pMeasTimePar->setSCPIInfo(new cSCPIInfo("CALCULATE", QString("%1:MTIME").arg(modNr), "10", m_pMeasTimePar->getName(), "0", "s"));
    m_pModule->veinModuleParameterHash[key] = m_pMeasTimePar; // for modules use
    iValidator = new cIntValidator(1, Zera::cSECInterface::maxSecCounterInitVal / 1000, 1); // counter in ms
    m_pMeasTimePar->setValidator(iValidator);
    m_pMeasTimePar->setUnit("s");

    m_pT0InputPar = new VfModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                             key = QString("PAR_T0Input"),
                                             QString("Start energy value"),
                                             QVariant((double)0.0));
    m_pT0InputPar->setSCPIInfo(new cSCPIInfo("CALCULATE",  QString("%1:T0INPUT").arg(modNr), "10", m_pT0InputPar->getName(), "0", ""));
    m_pModule->veinModuleParameterHash[key] = m_pT0InputPar; // for modules use
    cDoubleValidator *dValidator = new cDoubleValidator(0.0, 1.0e7, 1e-7);
    m_pT0InputPar->setValidator(dValidator);

    m_pT1InputPar = new VfModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                             key = QString("PAR_T1input"),
                                             QString("Final energy value"),
                                             QVariant((double)0.0));
    m_pT1InputPar->setSCPIInfo(new cSCPIInfo("CALCULATE",  QString("%1:T1INPUT").arg(modNr), "10", m_pT1InputPar->getName(), "0", ""));
    m_pModule->veinModuleParameterHash[key] = m_pT1InputPar; // for modules use
    dValidator = new cDoubleValidator(0.0, 1.0e7, 1e-7);
    m_pT1InputPar->setValidator(dValidator);

    m_pInputUnitPar = new VfModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                               key = QString("PAR_TXUNIT"),
                                               QString("Energy unit"),
                                               QVariant("Unknown"));
    m_pInputUnitPar->setSCPIInfo(new cSCPIInfo("CALCULATE",  QString("%1:TXUNIT").arg(modNr), "10", m_pInputUnitPar->getName(), "0", ""));
    m_pModule->veinModuleParameterHash[key] = m_pInputUnitPar; // for modules use

    m_pStartStopPar = new VfModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                               key = QString("PAR_StartStop"),
                                               QString("Start/stop measurement"),
                                               QVariant((int)0));
    m_pStartStopPar->setSCPIInfo(new cSCPIInfo("CALCULATE", QString("%1:START").arg(modNr), "10", "PAR_StartStop", "0", ""));
    m_pModule->veinModuleParameterHash[key] =  m_pStartStopPar; // for modules use
    iValidator = new cIntValidator(0, 1, 1);
    m_pStartStopPar->setValidator(iValidator);

    m_pStatusAct = new VfModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                            key = QString("ACT_Status"),
                                            QString("State: 0:Idle 1:First pulse wait 2:Started 4:Ready 8:Aborted"),
                                            QVariant((int)0) );
    m_pModule->veinModuleParameterHash[key] =  m_pStatusAct; // and for the modules interface
    m_pStatusAct->setSCPIInfo(new cSCPIInfo("CALCULATE",  QString("%1:STATUS").arg(modNr), "2", m_pStatusAct->getName(), "0", ""));

    m_pTimeAct = new VfModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                          key = QString("ACT_Time"),
                                          QString("Measurement time information"),
                                          QVariant((double) 0.0));
    m_pModule->veinModuleParameterHash[key] = m_pTimeAct; // and for the modules interface
    m_pTimeAct->setSCPIInfo(new cSCPIInfo("CALCULATE", QString("%1:TIME").arg(modNr), "2", m_pTimeAct->getName(), "0", "s"));
    m_pTimeAct->setUnit("s");

    m_pEnergyAct = new VfModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                            key = QString("ACT_Energy"),
                                            QString("Energy since last start"),
                                            QVariant((double) 0.0));
    m_pModule->veinModuleParameterHash[key] = m_pEnergyAct; // and for the modules interface
    m_pEnergyAct->setSCPIInfo(new cSCPIInfo("CALCULATE", QString("%1:ENERGY").arg(modNr), "2", m_pEnergyAct->getName(), "0", ""));

    m_pPowerAct = new VfModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                           key = QString("ACT_Power"),
                                           QString("Mean power since last start"),
                                           QVariant((double) 0.0));
    m_pModule->veinModuleParameterHash[key] = m_pPowerAct; // and for the modules interface
    m_pPowerAct->setSCPIInfo(new cSCPIInfo("CALCULATE", QString("%1:POWER").arg(modNr), "2", m_pPowerAct->getName(), "0", ""));

    m_pResultAct = new VfModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                            key = QString("ACT_Result"),
                                            QString("Result of last measurement"),
                                            QVariant((double) 0.0));
    m_pModule->veinModuleParameterHash[key] = m_pResultAct; // and for the modules interface
    m_pResultAct->setSCPIInfo(new cSCPIInfo("CALCULATE",  QString("%1:RESULT").arg(modNr), "2", m_pResultAct->getName(), "0", "%"));
    m_pResultAct->setUnit("%");

    m_pRefFreqInput = new VfModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                            key = QString("ACT_RefFreqInput"),
                                            QString("Reference frequency input to find power module"),
                                            QVariant(getConfData()->m_sRefInput.m_sPar));
    m_pModule->veinModuleParameterHash[key] = m_pRefFreqInput; // and for the modules interface
    m_pRefFreqInput->setSCPIInfo(new cSCPIInfo("CALCULATE",  QString("%1:REFFREQINPUT").arg(modNr), "2", m_pRefFreqInput->getName(), "0", ""));

    m_pUpperLimitPar = new VfModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                                key = QString("PAR_Uplimit"),
                                                QString("Error limit: upper"),
                                                QVariant((double)10.0));
    m_pUpperLimitPar->setSCPIInfo(new cSCPIInfo("CALCULATE",  QString("%1:UPLIMIT").arg(modNr), "10", m_pUpperLimitPar->getName(), "0", "%"));
    m_pModule->veinModuleParameterHash[key] = m_pUpperLimitPar; // for modules use
    dValidator = new cDoubleValidator(-100.0, 100.0, 1e-6);
    m_pUpperLimitPar->setValidator(dValidator);
    m_pUpperLimitPar->setUnit("%");

    m_pLowerLimitPar = new VfModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                                key = QString("PAR_Lolimit"),
                                                QString("Error limit: lower"),
                                                QVariant((double)-10.0));
    m_pLowerLimitPar->setSCPIInfo(new cSCPIInfo("CALCULATE",  QString("%1:LOLIMIT").arg(modNr), "10", m_pLowerLimitPar->getName(), "0", "%"));
    m_pModule->veinModuleParameterHash[key] = m_pLowerLimitPar; // for modules use
    dValidator = new cDoubleValidator(-100.0, 100.0, 1e-6);
    m_pLowerLimitPar->setValidator(dValidator);
    m_pLowerLimitPar->setUnit("%");

    m_pRatingAct = new VfModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                               key = QString("ACT_Rating"),
                                               QString("Evaluation last measurement"),
                                               QVariant((int) -1));
    m_pModule->veinModuleParameterHash[key] = m_pRatingAct; // and for the modules interface
    m_pRatingAct->setSCPIInfo(new cSCPIInfo("CALCULATE",  QString("%1:RATING").arg(modNr), "2", m_pRatingAct->getName(), "0", ""));

    m_pClientNotifierPar = new VfModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                           key = QString("PAR_ClientActiveNotify"),
                                           QString("By changing this component, a client asks us for max actualize performance"),
                                           QVariant((quint32)0));
    m_pModule->veinModuleParameterHash[key] = m_pClientNotifierPar; // for modules use
    iValidator = new cIntValidator(0, std::numeric_limits<quint32>::max(), 1);
    m_pClientNotifierPar->setValidator(iValidator);
    m_ClientActiveNotifier.init(m_pClientNotifierPar);
    connect(&m_ClientActiveNotifier, &ClientActiveComponent::clientActiveStateChanged, this, &cSpm1ModuleMeasProgram::clientActivationChanged);

}


void cSpm1ModuleMeasProgram::catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer)
{
    bool ok;

    if (msgnr == 0) // 0 was reserved for async. messages
    {
        QString sintnr;
        sintnr = answer.toString().section(':', 1, 1);
        int service = sintnr.toInt(&ok);
        switch (service)
        {
        case irqPCBRefConstanChangeNotifier:
            // we must fetch the ref constant of the selected reference Input
            handleChangedREFConst();
            break;
        default:
            // we must fetch the measured impuls count, compute the error and set corresponding entity
            handleSECInterrupt();
        }
    }
    else
    {
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
                    emit errMsg((tr(rmidentErrMSG)));
                    emit activationError();
                }
                break;

            case testsec1resource:
                if ((reply == ack) && (answer.toString().contains("SEC1")))
                    emit activationContinue();
                else
                {
                    emit errMsg((tr(resourcetypeErrMsg)));
                    emit activationError();
                }
                break;

            case setecresource:
                if (reply == ack)
                    emit activationContinue();
                else
                {
                    emit errMsg((tr(setresourceErrMsg)));
                    emit activationError();
                }
                break;

            case readresourcetypes:
                if (reply == ack)
                {
                    m_ResourceTypeList = answer.toString().split(';');
                    emit activationContinue();
                }
                else
                {
                    emit errMsg((tr(resourcetypeErrMsg)));
                    emit activationError();
                }
                break;

            case readresource:
            {
                if (reply == ack)
                {
                    m_ResourceHash[m_ResourceTypeList.at(m_nIt)] = answer.toString();
                    m_nIt++;
                    if (m_nIt < m_ResourceTypeList.count())
                        emit activationLoop();
                    else
                        emit activationContinue();
                }
                else
                {
                    emit errMsg((tr(resourceErrMsg)));
                    emit activationError();
                }
                break;
            }

            case fetchecalcunits:
            {
                QStringList sl;
                sl = answer.toString().split(';');
                if ((reply == ack) && (sl.length() >= 3))
                {
                    m_masterErrCalcName = sl.at(0);
                    m_slaveErrCalcName = sl.at(1);
                    m_slave2ErrCalcName = sl.at(2);
                    emit activationContinue();
                }
                else
                {
                    emit errMsg((tr(fetchsececalcunitErrMsg)));
                    emit activationError();
                }
                break;
            }

            case readrefInputalias:
            {
                if (reply == ack)
                {
                    m_refInputInfo->alias = answer.toString();
                    emit activationContinue();
                }
                else
                {
                    emit errMsg((tr(readaliasErrMsg)));
                    emit activationError();
                }
                break;
            }

            case freeecalcunits:
                if (reply == ack) // we only continue if sec server manager acknowledges
                    emit deactivationContinue();
                else
                {
                    emit errMsg((tr(freesececalcunitErrMsg)));
                    emit deactivationError();
                }
                break;

            case freeecresource:
                if (reply == ack) // we only continue if resource manager acknowledges
                    emit deactivationContinue();
                else
                {
                    emit errMsg((tr(freeresourceErrMsg)));
                    emit deactivationError();
                }
                break;

            case actualizeenergy:
            {
                if (reply == ack) {
                    // keep last values on (pending) abort / ignore post final responses
                    if((m_nStatus & ECALCSTATUS::ABORT) == 0 && !m_finalResultStateMachine.isRunning()) {
                        m_nEnergyCounterActual = answer.toUInt(&ok);
                        m_fEnergy = 1.0 * m_nEnergyCounterActual / (m_pRefConstantPar->getValue().toDouble() * mPowerUnitFactorHash[m_pInputUnitPar->getValue().toString()]);
                        m_pEnergyAct->setValue(m_fEnergy); // in kWh
                    }
                }
                else {
                    emit errMsg((tr(readsecregisterErrMsg)));
                    emit executionError();
                }
                break;
            }


            case actualizepower:
            {
                if (reply == ack) {
                    // keep last values on (pending) abort / ignore post final responses
                    if((m_nStatus & ECALCSTATUS::ABORT) == 0 && !m_finalResultStateMachine.isRunning()) {
                        m_fTimeSecondsActual = double(answer.toUInt(&ok)) * 0.001;
                        m_fPower = m_fEnergy * 3600.0 / (m_fTimeSecondsActual); // in kW
                        m_pPowerAct->setValue(m_fPower);
                        m_pTimeAct->setValue(m_fTimeSecondsActual);
                    }
                }
                else {
                    emit errMsg((tr(readsecregisterErrMsg)));
                    emit executionError();
                }
                break;
            }

            case actualizestatus:
            {
                if (reply == ack) {
                    // keep last values on (pending) abort / ignore post final responses
                    if((m_nStatus & ECALCSTATUS::ABORT) == 0 && !m_finalResultStateMachine.isRunning()) {
                        // once ready we leave status ready (continous mode)
                        m_nStatus = (m_nStatus & ECALCSTATUS::READY) | (answer.toUInt(&ok) & 7);
                        m_pStatusAct->setValue(QVariant(m_nStatus));
                    }
                }
                else {
                    emit errMsg((tr(readsecregisterErrMsg)));
                    emit executionError();
                }
                break;
            }

            case setsync:
                if (reply == ack)
                {
                    emit setupContinue();
                }
                else
                {
                    {
                        emit errMsg((tr(setsyncErrMsg)));
                        emit executionError();
                    }
                }
                break;

            case enableinterrupt:
            case setmeaspulses:
                if (reply == ack)
                {
                    emit setupContinue();
                }
                else
                {
                    {
                        emit errMsg((tr(writesecregisterErrMsg)));
                        emit executionError();
                    }
                }
                break;

            case setmastermux:
            case setslavemux:
                if (reply == ack)
                {
                    emit setupContinue();
                }
                else
                {
                    {
                        emit errMsg((tr(setmuxErrMsg)));
                        emit executionError();
                    }
                }
                break;

            case setmastermeasmode:
            case setslavemeasmode:
                if (reply == ack)
                {
                    emit setupContinue();
                }
                else
                {
                    {
                        emit errMsg((tr(setcmdidErrMsg)));
                        emit executionError();
                    }
                }
                break;

            case startmeasurement:
                if (reply == ack)
                {
                    emit setupContinue();
                }
                else
                {
                    {
                        emit errMsg((tr(startmeasErrMsg)));
                        emit executionError();
                    }
                }
                break;

            case stopmeas:
                if (reply == ack) {
                    emit deactivationContinue();
                }
                else
                {
                    {
                        emit errMsg((tr(stopmeasErrMsg)));
                        emit executionError();
                    }
                }
                break;

            case setpcbrefconstantnotifier:
                if (reply == ack) // we only continue if pcb server acknowledges
                    emit activationContinue();
                else
                {
                    emit errMsg((tr(registerpcbnotifierErrMsg)));
                    emit activationError();
                }
                break;

            case setsecintnotifier:
                if (reply == ack) // we only continue if sec server acknowledges
                    emit activationContinue();
                else
                {
                    emit errMsg((tr(registerpcbnotifierErrMsg)));
                    emit activationError();
                }
                break;


            case fetchrefconstant:
            {
                if (reply == ack) // we only continue if sec server acknowledges
                {
                    double constant;
                    constant = answer.toDouble(&ok);
                    m_pRefConstantPar->setValue(QVariant(constant));
                    newRefConstant(QVariant(constant));
                }
                else
                {
                    emit errMsg((tr(readrefconstantErrMsg)));
                    emit executionError();
                }
                break;
            }

            case readintregister:
                if (reply == ack) // we only continue if sec server acknowledges
                {
                    m_nIntReg = answer.toInt(&ok) & 7;
                    emit interruptContinue();
                }
                else
                {
                    emit errMsg((tr(readsecregisterErrMsg)));
                    emit executionError();
                }
                break;
            case resetintregister:
                if (reply == ack) // we only continue if sec server acknowledges
                {
                    emit interruptContinue();
                }
                else
                {
                    emit errMsg((tr(writesecregisterErrMsg)));
                    emit executionError();
                }
                break;
            case readvicount:
                if (reply == ack) // we only continue if sec server acknowledges
                {
                    // Although we do not have high frequency measurements,
                    // incorporate still running check as we learned from sec1
                    // see cSec1ModuleMeasProgram::catchInterfaceAnswer /
                    // case readvicount
                    if((m_nStatus & ECALCSTATUS::ABORT) == 0) {
                        m_nEnergyCounterFinal = answer.toLongLong(&ok);
                    }
                    emit interruptContinue();
                }
                else
                {
                    emit errMsg((tr(readsecregisterErrMsg)));
                    emit executionError();
                }
                break;
            case readtcount:
                if (reply == ack) // we only continue if sec server acknowledges
                {
                    m_fTimeSecondsFinal = double(answer.toLongLong(&ok)) * 0.001;
                    emit interruptContinue();
                }
                else
                {
                    emit errMsg((tr(readsecregisterErrMsg)));
                    emit executionError();
                }
                break;

            }
        }
    }
}

cSpm1ModuleConfigData *cSpm1ModuleMeasProgram::getConfData()
{
    return qobject_cast<cSpm1ModuleConfiguration*>(m_pConfiguration.get())->getConfigurationData();
}


void cSpm1ModuleMeasProgram::setInterfaceComponents()
{
    m_pRefInputPar->setValue(QVariant(mREFSpmInputInfoHash[getConfData()->m_sRefInput.m_sPar]->alias));
    m_pTargetedPar->setValue(QVariant(getConfData()->m_bTargeted.m_nActive));
    m_pMeasTimePar->setValue(QVariant(getConfData()->m_nMeasTime.m_nPar));
    m_pUpperLimitPar->setValue(QVariant(getConfData()->m_fUpperLimit.m_fPar));
    m_pLowerLimitPar->setValue(QVariant(getConfData()->m_fLowerLimit.m_fPar));
}


void cSpm1ModuleMeasProgram::setValidators()
{
    cStringValidator *sValidator;
    QString s;

    sValidator = new cStringValidator(m_REFAliasList);
    m_pRefInputPar->setValidator(sValidator);

    sValidator = new cStringValidator(getPowerUnitValidator());
    m_pInputUnitPar->setValidator(sValidator);
}


void cSpm1ModuleMeasProgram::setUnits()
{
    QString s;

    s = getPowerUnit();
    m_pPowerAct->setUnit(s);
    m_pT0InputPar->setUnit(s);
    m_pT1InputPar->setUnit(s);
    m_pInputUnitPar->setValue(s);

    s = getEnergyUnit();
    m_pEnergyAct->setUnit(s);
    // In case measurement is running, values are updated properly on next
    // interrupt (tested with vf-debugger). For a measuremnt finished we have to
    // recalc results with new units
    if(m_bActive && m_nStatus & ECALCSTATUS::READY) {
        setEMResult();
    }
    m_pModule->exportMetaData();
}


QStringList cSpm1ModuleMeasProgram::getEnergyUnitValidator()
{
    QStringList sl = getPowerUnitValidator();
    for (int i = 0; i < sl.count(); i++)
        sl.replace(i, sl.at(i)+"h");
    return sl;
}


QString cSpm1ModuleMeasProgram::getEnergyUnit()
{
    QString s = getPowerUnit();
    s.append("h");
    return s;
}


QStringList cSpm1ModuleMeasProgram::getPowerUnitValidator()
{
    QStringList sl;
    QString powType = mREFSpmInputInfoHash[getConfData()->m_sRefInput.m_sPar]->alias;

    if (powType.contains('P'))
        sl = getConfData()->m_ActiveUnitList;
    if (powType.contains('Q'))
        sl = getConfData()->m_ReactiveUnitList;
    if (powType.contains('S'))
        sl = getConfData()->m_ApparentUnitList;

    return sl;
}


QString cSpm1ModuleMeasProgram::getPowerUnit()
{
    QString powerType = mREFSpmInputInfoHash[getConfData()->m_sRefInput.m_sPar]->alias;
    QString currentPowerUnit = m_pInputUnitPar->getValue().toString();

    return cUnitHelper::getNewPowerUnit(powerType, currentPowerUnit);
}


void cSpm1ModuleMeasProgram::handleChangedREFConst()
{
    // we ask for the reference constant of the selected Input
    m_MsgNrCmdList[m_pPCBInterface->getConstantSource(getConfData()->m_sRefInput.m_sPar)] = fetchrefconstant;
    stopMeasuerment(true);
}


void cSpm1ModuleMeasProgram::handleSECInterrupt()
{
    if (!m_finalResultStateMachine.isRunning()) {
        m_finalResultStateMachine.setInitialState(&m_readIntRegisterState);
        m_finalResultStateMachine.start();
        m_ActualizeTimer.stop();
    }
}


void cSpm1ModuleMeasProgram::resourceManagerConnect()
{
    // first we try to get a connection to resource manager over proxy
    m_rmClient = Zera::Proxy::getInstance()->getConnectionSmart(getConfData()->m_RMSocket.m_sIP, getConfData()->m_RMSocket.m_nPort);
    // and then we set connection resource manager interface's connection
    m_rmInterface.setClientSmart(m_rmClient);
    resourceManagerConnectState.addTransition(m_rmClient.get(), &Zera::ProxyClient::connected, &m_IdentifyState);
    connect(&m_rmInterface, &Zera::cRMInterface::serverAnswer, this, &cSpm1ModuleMeasProgram::catchInterfaceAnswer);
    // todo insert timer for timeout and/or connect error conditions
    Zera::Proxy::getInstance()->startConnectionSmart(m_rmClient);
}


void cSpm1ModuleMeasProgram::sendRMIdent()
{
    m_MsgNrCmdList[m_rmInterface.rmIdent(QString("Spm1Module%1").arg(m_pModule->getModuleNr()))] = sendrmident;
}


void cSpm1ModuleMeasProgram::testSEC1Resource()
{
    m_MsgNrCmdList[m_rmInterface.getResourceTypes()] = testsec1resource;
}


void cSpm1ModuleMeasProgram::setECResource()
{
    m_MsgNrCmdList[m_rmInterface.setResource("SEC1", "ECALCULATOR", 3)] = setecresource;
}



void cSpm1ModuleMeasProgram::readResourceTypes()
{
    //m_MsgNrCmdList[m_rmInterface.getResourceTypes()] = readresourcetypes;
    // instead of taking all resourcetypes we take predefined types if we found them in our config

    if (found(getConfData()->m_refInpList, "fi"))
        m_ResourceTypeList.append("FRQINPUT");
    if (found(getConfData()->m_refInpList, "fo"))
        m_ResourceTypeList.append("SOURCE");
    if (found(getConfData()->m_refInpList, "sh"))
        m_ResourceTypeList.append("SCHEAD");
    if (found(getConfData()->m_refInpList, "hk"))
        m_ResourceTypeList.append("HKEY");

    emit activationContinue();

}


void cSpm1ModuleMeasProgram::readResources()
{
    m_nIt = 0; // we want to read all resources from resourcetypelist
    emit activationContinue();
}


void cSpm1ModuleMeasProgram::readResource()
{
    QString resourcetype = m_ResourceTypeList.at(m_nIt);
    m_MsgNrCmdList[m_rmInterface.getResources(resourcetype)] = readresource;
}


void cSpm1ModuleMeasProgram::testSpmInputs()
{
    qint32 referenceInputCount = getConfData()->m_refInpList.count();
    auto refInputNames = mREFSpmInputInfoHash.keys();
    for(const auto &refInputName : refInputNames) {
        for (int i = 0; i < m_ResourceTypeList.count(); i++) {
            QString resourcelist = m_ResourceHash[m_ResourceTypeList.at(i)];
            if (resourcelist.contains(refInputName)) {
                referenceInputCount--;
                mREFSpmInputInfoHash[refInputName] = new SecInputInfo();
                mREFSpmInputInfoHash[refInputName]->name = refInputName;
                mREFSpmInputInfoHash[refInputName]->resource  = m_ResourceTypeList.at(i);
                break;
            }
        }
    }
    if (referenceInputCount == 0) // we found all our configured Inputs
        emit activationContinue(); // so lets go on
    else {
        emit errMsg((tr(resourceErrMsg)));
        emit activationError();
    }
}


void cSpm1ModuleMeasProgram::ecalcServerConnect()
{
    // we try to get a connection to ecalc server over proxy
    m_pSECClient = Zera::Proxy::getInstance()->getConnection(getConfData()->m_SECServerSocket.m_sIP, getConfData()->m_SECServerSocket.m_nPort);
    // and then we set ecalcalculator interface's connection
    m_pSECInterface->setClient(m_pSECClient); //
    m_ecalcServerConnectState.addTransition(m_pSECClient, &Zera::ProxyClient::connected, &m_fetchECalcUnitsState);
    connect(m_pSECInterface, &Zera::cSECInterface::serverAnswer, this, &cSpm1ModuleMeasProgram::catchInterfaceAnswer);
    // todo insert timer for timeout and/or connect error conditions
    Zera::Proxy::getInstance()->startConnection(m_pSECClient);
}


void cSpm1ModuleMeasProgram::fetchECalcUnits()
{
    m_MsgNrCmdList[m_pSECInterface->setECalcUnit(3)] = fetchecalcunits; // we need 3 ecalc units to cascade
}


void cSpm1ModuleMeasProgram::pcbServerConnect()
{
    // we try to get a connection to ecalc server over proxy
    m_pPCBClient = Zera::Proxy::getInstance()->getConnection(getConfData()->m_PCBServerSocket.m_sIP, getConfData()->m_PCBServerSocket.m_nPort);
    // and then we set ecalcalculator interface's connection
    m_pPCBInterface->setClient(m_pPCBClient); //
    m_pcbServerConnectState.addTransition(m_pPCBClient, &Zera::ProxyClient::connected, &m_readREFInputsState);
    connect(m_pPCBInterface, &Zera::cPCBInterface::serverAnswer, this, &cSpm1ModuleMeasProgram::catchInterfaceAnswer);
    // todo insert timer for timeout and/or connect error conditions
    Zera::Proxy::getInstance()->startConnection(m_pPCBClient);
}


void cSpm1ModuleMeasProgram::readREFInputs()
{
    m_sItList = mREFSpmInputInfoHash.keys();
    emit activationContinue();
}


void cSpm1ModuleMeasProgram::readREFInputAlias()
{
    m_sIt = m_sItList.takeFirst();
    m_refInputInfo = mREFSpmInputInfoHash.take(m_sIt); // if set some info that could be useful later
    //m_MsgNrCmdList[m_refInputInfo->pcbIFace->resourceAliasQuery(m_refInputInfo->resource, m_sIt)] = readrefInputalias;

    // we will read the powertype of the reference frequency input and will use this as our alias ! for example P, +P ....
    m_MsgNrCmdList[m_pPCBInterface->getPowTypeSource(m_sIt)] = readrefInputalias;

}


void cSpm1ModuleMeasProgram::readREFInputDone()
{
    mREFSpmInputInfoHash[m_refInputInfo->name] = m_refInputInfo;
    if (m_sItList.isEmpty())
        emit activationContinue();
    else
        emit activationLoop();
}


void cSpm1ModuleMeasProgram::setpcbREFConstantNotifier()
{
    if ( (getConfData()->m_nRefInpCount > 0) && getConfData()->m_bEmbedded ) // if we have some ref. Input and are embedded in meter we register for notification
    {
        m_MsgNrCmdList[m_pPCBInterface->registerNotifier(QString("SOURCE:%1:CONSTANT?").arg(getConfData()->m_sRefInput.m_sPar), irqPCBRefConstanChangeNotifier)] = setpcbrefconstantnotifier;
        // todo also configure the query for setting this notifier .....very flexible
    }
    else
        emit activationContinue(); // if no ref constant notifier (standalone error calc) we directly go on
}


void cSpm1ModuleMeasProgram::setsecINTNotifier()
{
    m_MsgNrCmdList[m_pSECInterface->registerNotifier(QString("ECAL:%1:R%2?").arg(m_masterErrCalcName).arg(ECALCREG::INTREG))] = setsecintnotifier;
}


void cSpm1ModuleMeasProgram::activationDone()
{
    int nref;

    nref = getConfData()->m_refInpList.count();
    if (nref > 0)
    for (int i = 0; i < nref; i++)
    {
        m_REFAliasList.append(mREFSpmInputInfoHash[getConfData()->m_refInpList.at(i)]->alias); // build up a fixed sorted list of alias
        m_refInputInfo = mREFSpmInputInfoHash[getConfData()->m_refInpList.at(i)]; // change the hash for access via alias
        mREFSpmInputSelectionHash[m_refInputInfo->alias] = m_refInputInfo;
    }

    connect(&m_ActualizeTimer, &QTimer::timeout, this, &cSpm1ModuleMeasProgram::Actualize);

    connect(m_pStartStopPar, &VfModuleParameter::sigValueChanged, this, &cSpm1ModuleMeasProgram::newStartStop);
    connect(m_pRefInputPar, &VfModuleParameter::sigValueChanged, this, &cSpm1ModuleMeasProgram::newRefInput);

    connect(m_pTargetedPar, &VfModuleParameter::sigValueChanged, this , &cSpm1ModuleMeasProgram::newTargeted);
    connect(m_pMeasTimePar, &VfModuleParameter::sigValueChanged, this , &cSpm1ModuleMeasProgram::newMeasTime);
    connect(m_pT0InputPar, &VfModuleParameter::sigValueChanged, this , &cSpm1ModuleMeasProgram::newT0Input);
    connect(m_pT1InputPar, &VfModuleParameter::sigValueChanged, this , &cSpm1ModuleMeasProgram::newT1Input);
    connect(m_pInputUnitPar, &VfModuleParameter::sigValueChanged, this, &cSpm1ModuleMeasProgram::newUnit);
    connect(m_pUpperLimitPar, &VfModuleParameter::sigValueChanged, this, &cSpm1ModuleMeasProgram::newUpperLimit);
    connect(m_pLowerLimitPar, &VfModuleParameter::sigValueChanged, this, &cSpm1ModuleMeasProgram::newLowerLimit);

    setInterfaceComponents(); // actualize interface components
    setValidators();
    setUnits();

    // we ask for the reference constant of the selected Input
    m_MsgNrCmdList[m_pPCBInterface->getConstantSource(getConfData()->m_sRefInput.m_sPar)] = fetchrefconstant;

    m_bActive = true;
    emit activated();
}

void cSpm1ModuleMeasProgram::stopECCalculator()
{
    stopMeasuerment(true);
}


void cSpm1ModuleMeasProgram::freeECalculator()
{
    m_bActive = false;
    m_MsgNrCmdList[m_pSECInterface->freeECalcUnits()] = freeecalcunits;
}


void cSpm1ModuleMeasProgram::freeECResource()
{
    m_MsgNrCmdList[m_rmInterface.freeResource("SEC1", "ECALCULATOR")] = freeecresource;
}


void cSpm1ModuleMeasProgram::deactivationDone()
{
    disconnect(&m_rmInterface, 0, this, 0);
    disconnect(m_pSECInterface, 0, this, 0);
    disconnect(m_pPCBInterface, 0, this, 0);

    disconnect(m_pStartStopPar, 0, this, 0);
    disconnect(m_pTargetedPar, 0, this, 0);
    disconnect(m_pRefInputPar, 0, this, 0);
    disconnect(m_pMeasTimePar, 0, this, 0);
    disconnect(m_pT0InputPar, 0, this, 0);
    disconnect(m_pT1InputPar, 0, this, 0);

    emit deactivated();
}


void cSpm1ModuleMeasProgram::setSync()
{
    m_MsgNrCmdList[m_pSECInterface->setSync(m_slaveErrCalcName, m_masterErrCalcName)] = setsync;
}


void cSpm1ModuleMeasProgram::setSync2()
{
    m_MsgNrCmdList[m_pSECInterface->setSync(m_slave2ErrCalcName, m_masterErrCalcName)] = setsync;
}


void cSpm1ModuleMeasProgram::setMeaspulses()
{
    if (m_pTargetedPar->getValue().toInt() == 0)
        m_nTimerCountStart = Zera::cSECInterface::maxSecCounterInitVal; // we simply set max. time -> approx. 50 days
    else
        m_nTimerCountStart = m_pMeasTimePar->getValue().toLongLong() * 1000;

    m_MsgNrCmdList[m_pSECInterface->writeRegister(m_masterErrCalcName, ECALCREG::MTCNTin, m_nTimerCountStart)] = setmeaspulses;
}


void cSpm1ModuleMeasProgram::setMasterMux()
{
    m_MsgNrCmdList[m_pSECInterface->setMux(m_masterErrCalcName, QString("t1ms"))] = setmastermux; // to be discussed
}


void cSpm1ModuleMeasProgram::setSlaveMux()
{
    m_MsgNrCmdList[m_pSECInterface->setMux(m_slaveErrCalcName, mREFSpmInputSelectionHash[m_pRefInputPar->getValue().toString()]->name)] = setslavemux;
}


void cSpm1ModuleMeasProgram::setSlave2Mux()
{
    m_MsgNrCmdList[m_pSECInterface->setMux(m_slave2ErrCalcName, QString("t1ms"))] = setslavemux;
}


void cSpm1ModuleMeasProgram::setMasterMeasMode()
{
    m_MsgNrCmdList[m_pSECInterface->setCmdid(m_masterErrCalcName, ECALCCMDID::SINGLEERRORMASTER)] = setmastermeasmode;
}


void cSpm1ModuleMeasProgram::setSlaveMeasMode()
{
    m_MsgNrCmdList[m_pSECInterface->setCmdid(m_slaveErrCalcName, ECALCCMDID::ERRORMEASSLAVE)] = setslavemeasmode;
}


void cSpm1ModuleMeasProgram::setSlave2MeasMode()
{
    m_MsgNrCmdList[m_pSECInterface->setCmdid(m_slave2ErrCalcName, ECALCCMDID::ERRORMEASSLAVE)] = setslavemeasmode;
}


void cSpm1ModuleMeasProgram::enableInterrupt()
{
    // in case of targeted mode we want an interrupt when ready
    // in case of not targeted mode we set the time to maximum and the user will stop the measurement by pressing stop
    // so we program enable int. in any case
    m_MsgNrCmdList[m_pSECInterface->writeRegister(m_masterErrCalcName, ECALCREG::INTMASK, ECALCINT::MTCount0)] = enableinterrupt;
}


void cSpm1ModuleMeasProgram::startMeasurement()
{
    m_MsgNrCmdList[m_pSECInterface->start(m_masterErrCalcName)] = startmeasurement;
    m_nStatus = ECALCSTATUS::ARMED;
}


void cSpm1ModuleMeasProgram::startMeasurementDone()
{
    Actualize(); // we actualize at once after started
    m_ActualizeTimer.start(); // and after current interval
    m_fEnergy = 0.0;
    m_pEnergyAct->setValue(m_fEnergy);
    m_fPower = 0.0;
    m_pPowerAct->setValue(m_fPower);
}


void cSpm1ModuleMeasProgram::readIntRegister()
{
    m_MsgNrCmdList[m_pSECInterface->readRegister(m_masterErrCalcName, ECALCREG::INTREG)] = readintregister;
}


void cSpm1ModuleMeasProgram::resetIntRegister()
{
    m_MsgNrCmdList[m_pSECInterface->intAck(m_masterErrCalcName, 0xF)] = resetintregister; // we reset all here
}


void cSpm1ModuleMeasProgram::readVICountact()
{
    m_MsgNrCmdList[m_pSECInterface->readRegister(m_slaveErrCalcName, ECALCREG::MTCNTfin)] = readvicount;
}


void cSpm1ModuleMeasProgram::readTCountact()
{
    m_MsgNrCmdList[m_pSECInterface->readRegister(m_slave2ErrCalcName, ECALCREG::MTCNTfin)] = readtcount;
    // non targeted has been stopped already in newStartStop()
    if(getConfData()->m_bTargeted.m_nActive) {
        m_MsgNrCmdList[m_pSECInterface->stop(m_masterErrCalcName)] = stopmeas;
    }
    m_pStartStopPar->setValue(QVariant(0)); // restart enable
    m_nStatus = ECALCSTATUS::READY;
    m_pStatusAct->setValue(QVariant(m_nStatus));
}


void cSpm1ModuleMeasProgram::setEMResult()
{
    double PRef;
    double PDut;
    double time;

    m_fEnergy = 1.0 * m_nEnergyCounterFinal / m_pRefConstantPar->getValue().toDouble();
    time = m_fTimeSecondsFinal;

    PRef = m_fEnergy * 3600.0 / time;
    PDut = (m_pT1InputPar->getValue().toDouble() - m_pT0InputPar->getValue().toDouble()) * mPowerUnitFactorHash[m_pInputUnitPar->getValue().toString()];
    if (PRef == 0)
    {
        m_fResult = qQNaN();
        m_eRating = ECALCRESULT::RESULT_UNFINISHED;
    }
    else
    {
        m_fResult = (PDut - PRef) * 100.0 / PRef;
        setRating();
    }

    m_fEnergy /=  mPowerUnitFactorHash[m_pInputUnitPar->getValue().toString()];
    m_fPower = m_fEnergy * 3600.0 / time;

    m_pTimeAct->setValue(QVariant(time));
    m_pResultAct->setValue(QVariant(m_fResult));
    m_pEnergyAct->setValue(QVariant(m_fEnergy));
    m_pPowerAct->setValue(QVariant(m_fPower));
}


void cSpm1ModuleMeasProgram::setRating()
{
    if (m_nStatus & ECALCSTATUS::READY)
    {
        if ( (m_fResult >= getConfData()->m_fLowerLimit.m_fPar) && (m_fResult <= getConfData()->m_fUpperLimit.m_fPar))
            m_eRating = ECALCRESULT::RESULT_PASSED;
        else
            m_eRating = ECALCRESULT::RESULT_FAILED;
    }
    else
        m_eRating = ECALCRESULT::RESULT_UNFINISHED;

    m_pRatingAct->setValue(int(m_eRating));
}


void cSpm1ModuleMeasProgram::newStartStop(QVariant startstop)
{
    bool ok;
    int ss = startstop.toInt(&ok);
    if (ss > 0) // we get started
    {
        if (!m_startMeasurementMachine.isRunning())
            m_startMeasurementMachine.start();
        // setsync
        // mtvorwahl setzen
        // master -> Input mux setzen
        // slave1 + slave2 -> Input mux setzen
        // meas mode setzen + arm
        // m_ActualizeTimer.start();
    }
    else
    {
        if (getConfData()->m_bTargeted.m_nActive > 0) {
            stopMeasuerment(true);
        }
        else {
            m_MsgNrCmdList[m_pSECInterface->stop(m_masterErrCalcName)] = stopmeas;
            m_ActualizeTimer.stop();
            // if we are not "targeted" we handle pressing stop as if the
            // measurement became ready and an interrupt occured
            if (!m_finalResultStateMachine.isRunning()) {
                m_finalResultStateMachine.start();
            }
        }
    }
}


void cSpm1ModuleMeasProgram::newRefConstant(QVariant refconst)
{
    m_pRefConstantPar->setValue(refconst);
    setInterfaceComponents();

    emit m_pModule->parameterChanged();
}


void cSpm1ModuleMeasProgram::newRefInput(QVariant refinput)
{
    QString refInputName = mREFSpmInputSelectionHash[refinput.toString()]->name;
    getConfData()->m_sRefInput.m_sPar = refInputName;
    setInterfaceComponents();

    // if the reference input changes P <-> Q <-> S it is necessary to change energyunit and powerunit and their validators
    setValidators();
    setUnits();
    m_pRefFreqInput->setValue(refInputName);

    emit m_pModule->parameterChanged();
}


void cSpm1ModuleMeasProgram::newTargeted(QVariant targeted)
{
    getConfData()->m_bTargeted.m_nActive = targeted.toInt();
    setInterfaceComponents();

    emit m_pModule->parameterChanged();
}


void cSpm1ModuleMeasProgram::newMeasTime(QVariant meastime)
{
    getConfData()->m_nMeasTime.m_nPar = meastime.toInt();
    setInterfaceComponents();

    emit m_pModule->parameterChanged();
}


void cSpm1ModuleMeasProgram::newT0Input(QVariant t0input)
{
    m_pT0InputPar->setValue(t0input);
    setEMResult();

    emit m_pModule->parameterChanged();
}


void cSpm1ModuleMeasProgram::newT1Input(QVariant t1input)
{
    m_pT1InputPar->setValue(t1input);
    setEMResult();

    emit m_pModule->parameterChanged();
}


void cSpm1ModuleMeasProgram::newUnit(QVariant unit)
{
    m_pInputUnitPar->setValue(unit.toString());
    setInterfaceComponents();
    setUnits();

    emit m_pModule->parameterChanged();
}


void cSpm1ModuleMeasProgram::newUpperLimit(QVariant limit)
{
    bool ok;
    getConfData()->m_fUpperLimit.m_fPar = limit.toDouble(&ok);
    setRating();
    setInterfaceComponents();

    emit m_pModule->parameterChanged();
}


void cSpm1ModuleMeasProgram::newLowerLimit(QVariant limit)
{
    bool ok;
    getConfData()->m_fLowerLimit.m_fPar = limit.toDouble(&ok);
    setRating();
    setInterfaceComponents();

    emit m_pModule->parameterChanged();
}


void cSpm1ModuleMeasProgram::Actualize()
{
    m_MsgNrCmdList[m_pSECInterface->readRegister(m_masterErrCalcName, ECALCREG::STATUS)] = actualizestatus;
    m_MsgNrCmdList[m_pSECInterface->readRegister(m_slaveErrCalcName, ECALCREG::MTCNTact)] = actualizeenergy;
    m_MsgNrCmdList[m_pSECInterface->readRegister(m_slave2ErrCalcName, ECALCREG::MTCNTact)] = actualizepower;
}

void cSpm1ModuleMeasProgram::clientActivationChanged(bool bActive)
{
    // Adjust our m_ActualizeTimer timeout to our client's needs
    m_ActualizeTimer.setInterval(bActive ? m_nActualizeIntervallHighFreq : m_nActualizeIntervallLowFreq);
}


bool cSpm1ModuleMeasProgram::found(QList<QString> &list, QString searched)
{
    for (int i = 0; i < list.count(); i++)
        if (list.at(i).contains(searched))
            return true;
    return false;
}

void cSpm1ModuleMeasProgram::stopMeasuerment(bool bAbort)
{
    if(bAbort) {
        m_nStatus = ECALCSTATUS::ABORT;
        m_pStatusAct->setValue(QVariant(m_nStatus));
    }
    m_MsgNrCmdList[m_pSECInterface->stop(m_masterErrCalcName)] = stopmeas;
    m_pStartStopPar->setValue(QVariant(0));
    m_ActualizeTimer.stop();
}

}















