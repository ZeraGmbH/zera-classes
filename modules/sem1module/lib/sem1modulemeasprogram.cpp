#include "sem1modulemeasprogram.h"
#include "sem1module.h"
#include "sem1moduleconfigdata.h"
#include "sem1moduleconfiguration.h"
#include <errormessages.h>
#include <scpi.h>
#include <unithelper.h>
#include <doublevalidator.h>
#include <intvalidator.h>
#include <stringvalidator.h>
#include <reply.h>
#include <proxy.h>
#include <math.h>
#include <timerfactoryqt.h>

namespace SEM1MODULE
{

cSem1ModuleMeasProgram::cSem1ModuleMeasProgram(cSem1Module* module, std::shared_ptr<BaseModuleConfiguration> pConfiguration) :
    cBaseMeasProgram(pConfiguration, module->getVeinModuleName()),
    m_pModule(module)
{
    // we have to instantiate a working resource manager and secserver interface
    m_secInterface = std::make_unique<Zera::cSECInterface>();
    m_pcbInterface = std::make_shared<Zera::cPCBInterface>();

    m_IdentifyState.addTransition(this, &cSem1ModuleMeasProgram::activationContinue, &m_testSEC1ResourceState);
    m_testSEC1ResourceState.addTransition(this, &cSem1ModuleMeasProgram::activationContinue, &m_setECResourceState); // test presence of sec1 resource
    m_setECResourceState.addTransition(this, &cSem1ModuleMeasProgram::activationContinue, &m_readResourcesState); // claim 3 ecalculator units
    m_readResourcesState.addTransition(this, &cSem1ModuleMeasProgram::activationContinue, &m_readResourceState); // init read resources
    m_readResourceState.addTransition(this, &cSem1ModuleMeasProgram::activationLoop, &m_readResourceState); // read their resources into list
    m_readResourceState.addTransition(this, &cSem1ModuleMeasProgram::activationContinue, &m_testSemInputsState); // go on if done
    m_testSemInputsState.addTransition(this, &cSem1ModuleMeasProgram::activationContinue, &m_ecalcServerConnectState); // test all configured Inputs
    //m_ecalcServerConnectState.addTransition(this, &cSem1ModuleMeasProgram::activationContinue, &m_fetchECalcUnitsState); // connect to ecalc server
    //transition from this state to m_fetch....is done in ecalcServerConnect
    m_fetchECalcUnitsState.addTransition(this, &cSem1ModuleMeasProgram::activationContinue, &m_pcbServerConnectState); // connect to pcbserver


    //m_pcbServerConnectState.addTransition(this, &cSem1ModuleMeasProgram::activationContinue, &m_pcbServerConnectState);
    //transition from this state to m_readREFInputsState....is done in pcbServerConnect
    m_readREFInputsState.addTransition(this, &cSem1ModuleMeasProgram::activationContinue, &m_readREFInputAliasState);
    m_readREFInputAliasState.addTransition(this, &cSem1ModuleMeasProgram::activationContinue, &m_readREFInputDoneState);
    m_readREFInputDoneState.addTransition(this, &cSem1ModuleMeasProgram::activationLoop, &m_readREFInputAliasState);
    m_readREFInputDoneState.addTransition(this, &cSem1ModuleMeasProgram::activationContinue, &m_setpcbREFConstantNotifierState);

    m_setpcbREFConstantNotifierState.addTransition(this, &cSem1ModuleMeasProgram::activationContinue, &m_setsecINTNotifierState);
    m_setsecINTNotifierState.addTransition(this, &cSem1ModuleMeasProgram::activationContinue, &m_activationDoneState);

    m_activationMachine.addState(&resourceManagerConnectState);
    m_activationMachine.addState(&m_IdentifyState);
    m_activationMachine.addState(&m_testSEC1ResourceState);
    m_activationMachine.addState(&m_setECResourceState);
    m_activationMachine.addState(&m_readResourcesState);
    m_activationMachine.addState(&m_readResourceState);
    m_activationMachine.addState(&m_testSemInputsState);
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

    connect(&resourceManagerConnectState, &QState::entered, this, &cSem1ModuleMeasProgram::resourceManagerConnect);
    connect(&m_IdentifyState, &QState::entered, this, &cSem1ModuleMeasProgram::sendRMIdent);
    connect(&m_testSEC1ResourceState, &QState::entered, this, &cSem1ModuleMeasProgram::testSEC1Resource);
    connect(&m_setECResourceState, &QState::entered, this, &cSem1ModuleMeasProgram::setECResource);
    connect(&m_readResourcesState, &QState::entered, this, &cSem1ModuleMeasProgram::readResources);
    connect(&m_readResourceState, &QState::entered, this, &cSem1ModuleMeasProgram::readResource);
    connect(&m_testSemInputsState, &QState::entered, this, &cSem1ModuleMeasProgram::testSemInputs);
    connect(&m_ecalcServerConnectState, &QState::entered, this, &cSem1ModuleMeasProgram::ecalcServerConnect);
    connect(&m_fetchECalcUnitsState, &QState::entered, this, &cSem1ModuleMeasProgram::fetchECalcUnits);
    connect(&m_pcbServerConnectState, &QState::entered, this, &cSem1ModuleMeasProgram::pcbServerConnect);
    connect(&m_readREFInputsState, &QState::entered, this, &cSem1ModuleMeasProgram::readREFInputs);
    connect(&m_readREFInputAliasState, &QState::entered, this, &cSem1ModuleMeasProgram::readREFInputAlias);
    connect(&m_readREFInputDoneState, &QState::entered, this, &cSem1ModuleMeasProgram::readREFInputDone);
    connect(&m_setpcbREFConstantNotifierState, &QState::entered, this, &cSem1ModuleMeasProgram::setpcbREFConstantNotifier);
    connect(&m_setsecINTNotifierState, &QState::entered, this, &cSem1ModuleMeasProgram::setsecINTNotifier);
    connect(&m_activationDoneState, &QState::entered, this, &cSem1ModuleMeasProgram::activationDone);

    // setting up statemachine to free the occupied resources
    m_stopECalculatorState.addTransition(this, &cSem1ModuleMeasProgram::deactivationContinue, &m_freeECalculatorState);
    m_freeECalculatorState.addTransition(this, &cSem1ModuleMeasProgram::deactivationContinue, &m_freeECResource);
    m_freeECResource.addTransition(this, &cSem1ModuleMeasProgram::deactivationContinue, &m_deactivationDoneState);

    m_deactivationMachine.addState(&m_stopECalculatorState);
    m_deactivationMachine.addState(&m_freeECalculatorState);
    m_deactivationMachine.addState(&m_freeECResource);
    m_deactivationMachine.addState(&m_deactivationDoneState);

    m_deactivationMachine.setInitialState(&m_stopECalculatorState);

    connect(&m_stopECalculatorState, &QState::entered, this, &cSem1ModuleMeasProgram::stopECCalculator);
    connect(&m_freeECalculatorState, &QState::entered, this, &cSem1ModuleMeasProgram::freeECalculator);
    connect(&m_freeECResource, &QState::entered, this, &cSem1ModuleMeasProgram::freeECResource);
    connect(&m_deactivationDoneState, &QState::entered, this, &cSem1ModuleMeasProgram::deactivationDone);

    // setting up statemachine used when starting a measurement

    m_setsyncState.addTransition(this, &cSem1ModuleMeasProgram::setupContinue, &m_setsync2State);
    m_setsync2State.addTransition(this, &cSem1ModuleMeasProgram::setupContinue, &m_setMeaspulsesState);
    m_setMeaspulsesState.addTransition(this, &cSem1ModuleMeasProgram::setupContinue, &m_setMasterMuxState);
    m_setMasterMuxState.addTransition(this, &cSem1ModuleMeasProgram::setupContinue, &m_setSlaveMuxState);
    m_setSlaveMuxState.addTransition(this, &cSem1ModuleMeasProgram::setupContinue, &m_setSlave2MuxState);
    m_setSlave2MuxState.addTransition(this, &cSem1ModuleMeasProgram::setupContinue, &m_setMasterMeasModeState);
    m_setMasterMeasModeState.addTransition(this, &cSem1ModuleMeasProgram::setupContinue, &m_setSlaveMeasModeState);
    m_setSlaveMeasModeState.addTransition(this, &cSem1ModuleMeasProgram::setupContinue, &m_setSlave2MeasModeState);
    m_setSlave2MeasModeState.addTransition(this, &cSem1ModuleMeasProgram::setupContinue, &m_enableInterruptState);
    m_enableInterruptState.addTransition(this, &cSem1ModuleMeasProgram::setupContinue, &m_startMeasurementState);
    m_startMeasurementState.addTransition(this, &cSem1ModuleMeasProgram::setupContinue, &m_startMeasurementDoneState);

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

    if(m_pModule->getDemo())
        m_startMeasurementMachine.setInitialState(&m_startMeasurementState);
    else
        m_startMeasurementMachine.setInitialState(&m_setsyncState);

    connect(&m_setsyncState, &QState::entered, this, &cSem1ModuleMeasProgram::setSync);
    connect(&m_setsync2State, &QState::entered, this, &cSem1ModuleMeasProgram::setSync2);
    connect(&m_setMeaspulsesState, &QState::entered, this, &cSem1ModuleMeasProgram::setMeaspulses);
    connect(&m_setMasterMuxState, &QState::entered, this, &cSem1ModuleMeasProgram::setMasterMux);
    connect(&m_setSlaveMuxState, &QState::entered, this, &cSem1ModuleMeasProgram::setSlaveMux);
    connect(&m_setSlave2MuxState, &QState::entered, this, &cSem1ModuleMeasProgram::setSlave2Mux);
    connect(&m_setMasterMeasModeState, &QState::entered, this, &cSem1ModuleMeasProgram::setMasterMeasMode);
    connect(&m_setSlaveMeasModeState, &QState::entered, this, &cSem1ModuleMeasProgram::setSlaveMeasMode);
    connect(&m_setSlave2MeasModeState, &QState::entered, this, &cSem1ModuleMeasProgram::setSlave2MeasMode);
    connect(&m_enableInterruptState, &QState::entered, this, &cSem1ModuleMeasProgram::enableInterrupt);
    connect(&m_startMeasurementState, &QState::entered, this, &cSem1ModuleMeasProgram::startMeasurement);
    connect(&m_startMeasurementDoneState, &QState::entered, this, &cSem1ModuleMeasProgram::startMeasurementDone);

    // setting up statemachine m_finalResultStateMachine
    // mode targeted:
    // * for interrupt handling (Interrupt is thrown on measurement finished)
    // mode non targeted (Start/Stop):
    // * on user stop this machine is started.
    m_readIntRegisterState.addTransition(this, &cSem1ModuleMeasProgram::interruptContinue, &m_resetIntRegisterState); // targeted: initial state
    m_resetIntRegisterState.addTransition(this, &cSem1ModuleMeasProgram::interruptContinue, &m_readFinalEnergyCounterState);
    m_readFinalEnergyCounterState.addTransition(this, &cSem1ModuleMeasProgram::interruptContinue, &m_readFinalTimeCounterState);
    m_readFinalTimeCounterState.addTransition(this, &cSem1ModuleMeasProgram::interruptContinue, &m_setEMResultState);

    m_finalResultStateMachine.addState(&m_readIntRegisterState);
    m_finalResultStateMachine.addState(&m_resetIntRegisterState);
    m_finalResultStateMachine.addState(&m_readFinalEnergyCounterState);
    m_finalResultStateMachine.addState(&m_readFinalTimeCounterState);
    m_finalResultStateMachine.addState(&m_setEMResultState);

    m_finalResultStateMachine.setInitialState(&m_readIntRegisterState);

    connect(&m_readIntRegisterState, &QState::entered, this, &cSem1ModuleMeasProgram::readIntRegister);
    connect(&m_resetIntRegisterState, &QState::entered, this, &cSem1ModuleMeasProgram::resetIntRegister);
    connect(&m_readFinalEnergyCounterState, &QState::entered, this, &cSem1ModuleMeasProgram::readVICountact);
    connect(&m_readFinalTimeCounterState, &QState::entered, this, &cSem1ModuleMeasProgram::readTCountact);
    connect(&m_setEMResultState, &QState::entered, this, &cSem1ModuleMeasProgram::onEMResultState);

    // we need a hash for our different energy input units
    mEnergyUnitFactorHash["MWh"] = 1000.0;
    mEnergyUnitFactorHash["kWh"] = 1.0;
    mEnergyUnitFactorHash["Wh"] = 0.001;
    mEnergyUnitFactorHash["MVarh"] = 1000.0;
    mEnergyUnitFactorHash["kVarh"] = 1.0;
    mEnergyUnitFactorHash["Varh"] = 0.001;
    mEnergyUnitFactorHash["MVAh"] = 1000.0;
    mEnergyUnitFactorHash["kVAh"] = 1.0;
    mEnergyUnitFactorHash["VAh"] = 0.001;

    m_ActualizeTimer = TimerFactoryQt::createPeriodic(m_nActualizeIntervallLowFreq);

    m_resourceTypeList.addTypesFromConfig(getConfData()->m_refInpList);
}

void cSem1ModuleMeasProgram::start()
{
}

void cSem1ModuleMeasProgram::stop()
{
}

void cSem1ModuleMeasProgram::generateVeinInterface()
{
    QString s;
    QString key;

    QString modNr = QString("%1").arg(m_pModule->getModuleNr(),4,10,QChar('0'));

    m_pRefInputPar = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                              key = QString("PAR_RefInput"),
                                              QString("Ref input"),
                                              QVariant(s = "Unknown"));
    m_pRefInputPar->setScpiInfo("CALCULATE", QString("%1:REFSOURCE").arg(modNr), SCPI::isQuery|SCPI::isCmdwP, m_pRefInputPar->getName());
    m_pModule->m_veinModuleParameterMap[key] = m_pRefInputPar; // for modules use

    m_pRefConstantPar = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                                 key = QString("PAR_RefConstant"),
                                                 QString("Reference meter constant"),
                                                 QVariant((double)0.0));
    m_pRefConstantPar->setScpiInfo("CALCULATE", QString("%1:REFCONSTANT").arg(modNr ), SCPI::isQuery, m_pRefConstantPar->getName());
    m_pModule->m_veinModuleParameterMap[key] = m_pRefConstantPar; // for modules use

    m_pTargetedPar = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                              key = QString("PAR_Targeted"),
                                              QString("Stop mode"),
                                              QVariant((int)0));
    m_pTargetedPar->setScpiInfo("CALCULATE", QString("%1:MODE").arg(modNr), SCPI::isQuery|SCPI::isCmdwP, m_pTargetedPar->getName());
    m_pTargetedPar->setValidator(new cIntValidator(0,1,1));
    m_pModule->m_veinModuleParameterMap[key] = m_pTargetedPar; // for modules use

    m_pMeasTimePar = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                              key = QString("PAR_MeasTime"),
                                              QString("Measurement time"),
                                              QVariant((uint)10));
    m_pMeasTimePar->setScpiInfo("CALCULATE", QString("%1:MTIME").arg(modNr), SCPI::isQuery|SCPI::isCmdwP, m_pMeasTimePar->getName());
    m_pModule->m_veinModuleParameterMap[key] = m_pMeasTimePar; // for modules use
    m_pMeasTimePar->setValidator(new cIntValidator(1, Zera::cSECInterface::maxSecCounterInitVal / 1000, 1)); // counter in ms
    m_pMeasTimePar->setUnit("s");

    m_pT0InputPar = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                             key = QString("PAR_T0Input"),
                                             QString("Start energy value"),
                                             QVariant((double)0.0));
    m_pT0InputPar->setScpiInfo("CALCULATE", QString("%1:T0INPUT").arg(modNr), SCPI::isQuery|SCPI::isCmdwP, m_pT0InputPar->getName());
    m_pT0InputPar->setValidator(new cDoubleValidator(0.0, 1.0e7, 1e-7));
    m_pModule->m_veinModuleParameterMap[key] = m_pT0InputPar; // for modules use

    m_pT1InputPar = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                             key = QString("PAR_T1input"),
                                             QString("Final energy value"),
                                             QVariant((double)0.0));
    m_pT1InputPar->setScpiInfo("CALCULATE", QString("%1:T1INPUT").arg(modNr), SCPI::isQuery|SCPI::isCmdwP, m_pT1InputPar->getName());
    m_pT1InputPar->setValidator(new cDoubleValidator(0.0, 1.0e7, 1e-7));
    m_pModule->m_veinModuleParameterMap[key] = m_pT1InputPar; // for modules use

    m_pInputUnitPar = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                               key = QString("PAR_TXUNIT"),
                                               QString("Energy unit"),
                                               QVariant("Unknown"));
    m_pInputUnitPar->setScpiInfo("CALCULATE", QString("%1:TXUNIT").arg(modNr), SCPI::isQuery|SCPI::isCmdwP, m_pInputUnitPar->getName());
    m_pModule->m_veinModuleParameterMap[key] = m_pInputUnitPar; // for modules use

    m_pStartStopPar = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                               key = QString("PAR_StartStop"),
                                               QString("Start/stop measurement (start=1, stop=0)"),
                                               QVariant((int)0));
    m_pStartStopPar->setScpiInfo("CALCULATE", QString("%1:START").arg(modNr), SCPI::isQuery|SCPI::isCmdwP, "PAR_StartStop");
    m_pStartStopPar->setValidator(new cIntValidator(0, 1, 1));
    m_pModule->m_veinModuleParameterMap[key] =  m_pStartStopPar; // for modules use

    m_pStatusAct = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                            key = QString("ACT_Status"),
                                            QString("State: 0:Idle 1:First pulse wait 2:Started 4:Ready 8:Aborted"),
                                            QVariant((quint32)0) );
    m_pStatusAct->setScpiInfo("CALCULATE", QString("%1:STATUS").arg(modNr), SCPI::isQuery, m_pStatusAct->getName());
    m_pModule->m_veinModuleParameterMap[key] =  m_pStatusAct; // and for the modules interface

    m_pTimeAct = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                          key = QString("ACT_Time"),
                                          QString("Measurement time"),
                                          QVariant((double) 0.0));
    m_pTimeAct->setScpiInfo("CALCULATE", QString("%1:TIME").arg(modNr), SCPI::isQuery, m_pTimeAct->getName());
    m_pTimeAct->setUnit("s");
    m_pModule->m_veinModuleParameterMap[key] = m_pTimeAct; // and for the modules interface

    m_pEnergyAct = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                            key = QString("ACT_Energy"),
                                            QString("Energy since last start"),
                                            QVariant((double) 0.0));
    m_pEnergyAct->setScpiInfo("CALCULATE", QString("%1:ENERGY").arg(modNr), SCPI::isQuery, m_pEnergyAct->getName());
    m_pModule->m_veinModuleParameterMap[key] = m_pEnergyAct; // and for the modules interface

    m_pPowerAct = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                           key = QString("ACT_Power"),
                                           QString("Mean power since last start"),
                                           QVariant((double) 0.0));
    m_pPowerAct->setScpiInfo("CALCULATE", QString("%1:POWER").arg(modNr), SCPI::isQuery, m_pPowerAct->getName());
    m_pModule->m_veinModuleParameterMap[key] = m_pPowerAct; // and for the modules interface

    m_pResultAct = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                            key = QString("ACT_Result"),
                                            QString("Result of last measurement"),
                                            QVariant((double) 0.0));
    m_pResultAct->setScpiInfo("CALCULATE",  QString("%1:RESULT").arg(modNr), SCPI::isQuery, m_pResultAct->getName());
    m_pResultAct->setUnit("%");
    m_pModule->m_veinModuleParameterMap[key] = m_pResultAct; // and for the modules interface

    m_pRefFreqInput = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                            key = QString("ACT_RefFreqInput"),
                                            QString("Reference frequency input to find power module"),
                                            QVariant(getConfData()->m_sRefInput.m_sPar));
    m_pRefFreqInput->setScpiInfo("CALCULATE", QString("%1:REFFREQINPUT").arg(modNr), SCPI::isQuery, m_pRefFreqInput->getName());
    m_pModule->m_veinModuleParameterMap[key] = m_pRefFreqInput; // and for the modules interface

    m_pUpperLimitPar = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                                key = QString("PAR_Uplimit"),
                                                QString("Error limit: upper"),
                                                QVariant(getConfData()->m_fUpperLimit.m_fPar));
    m_pUpperLimitPar->setScpiInfo("CALCULATE",  QString("%1:UPLIMIT").arg(modNr), SCPI::isQuery|SCPI::isCmdwP, m_pUpperLimitPar->getName());
    m_pUpperLimitPar->setValidator(new cDoubleValidator(-100.0, 100.0, 1e-6));
    m_pUpperLimitPar->setUnit("%");
    m_pModule->m_veinModuleParameterMap[key] = m_pUpperLimitPar; // for modules use

    m_pLowerLimitPar = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                                key = QString("PAR_Lolimit"),
                                                QString("Error limit: lower"),
                                                QVariant(getConfData()->m_fLowerLimit.m_fPar));
    m_pLowerLimitPar->setScpiInfo("CALCULATE",  QString("%1:LOLIMIT").arg(modNr), SCPI::isQuery|SCPI::isCmdwP, m_pLowerLimitPar->getName());
    m_pLowerLimitPar->setValidator(new cDoubleValidator(-100.0, 100.0, 1e-6));
    m_pLowerLimitPar->setUnit("%");
    m_pModule->m_veinModuleParameterMap[key] = m_pLowerLimitPar; // for modules use

    m_pRatingAct = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                               key = QString("ACT_Rating"),
                                               QString("Evaluation for the last measurement"),
                                               QVariant((int) -1));
    m_pRatingAct->setScpiInfo("CALCULATE",  QString("%1:RATING").arg(modNr), SCPI::isQuery, m_pRatingAct->getName());
    m_pModule->m_veinModuleParameterMap[key] = m_pRatingAct; // and for the modules interface

    m_pClientNotifierPar = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                           key = QString("PAR_ClientActiveNotify"),
                                           QString("By changing this component, a client asks us for max actualize performance"),
                                           QVariant(0));
    m_pClientNotifierPar->setValidator(new cIntValidator(0, std::numeric_limits<int>::max(), 1));
    m_pModule->m_veinModuleParameterMap[key] = m_pClientNotifierPar; // for modules use
    m_ClientActiveNotifier.init(m_pClientNotifierPar);
    connect(&m_ClientActiveNotifier, &ClientActiveComponent::clientActiveStateChanged, this, &cSem1ModuleMeasProgram::clientActivationChanged);

    m_pMeasStartTime = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                             key = QString("ACT_StartTime"),
                                             QString("Last measurement: Start time (dd-MM-yyyy HH:mm:ss)"),
                                             QString());
    m_pMeasStartTime->setScpiInfo("CALCULATE",  QString("%1:STRTTIME").arg(modNr), SCPI::isQuery, m_pMeasStartTime->getName());
    m_pModule->m_veinModuleParameterMap[key] = m_pMeasStartTime; // and for the modules interface

    m_pMeasEndTime = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                           key = QString("ACT_EndTime"),
                                           QString("Last measurement: End time (dd-MM-yyyy HH:mm:ss)"),
                                           QString());
    m_pMeasEndTime->setScpiInfo("CALCULATE",  QString("%1:ENDTIME").arg(modNr), SCPI::isQuery, m_pMeasEndTime->getName());
    m_pModule->m_veinModuleParameterMap[key] = m_pMeasEndTime; // and for the modules interface

    m_pMeasDurationMs = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                        key = QString("ACT_MeasTime"),
                                        QString("Last measurement: Duration [ms]"),
                                        QVariant((int)0));
    m_pMeasDurationMs->setScpiInfo("CALCULATE",  QString("%1:MMEASTIME").arg(modNr), SCPI::isQuery, m_pMeasDurationMs->getName());
    m_pModule->m_veinModuleParameterMap[key] = m_pMeasDurationMs; // and for the modules interface
}


void cSem1ModuleMeasProgram::catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer)
{
    if (msgnr == 0) // 0 was reserved for async. messages
        // we must fetch the measured impuls count, compute the error and set corresponding entity
        handleSECInterrupt();
    else {
        if (m_MsgNrCmdList.contains(msgnr)) {
            int cmd = m_MsgNrCmdList.take(msgnr);
            switch (cmd)
            {
            case sendrmident:
                if (reply == ack)
                    emit activationContinue();
                else
                    notifyError(rmidentErrMSG);
                break;

            case testsec1resource:
                if ((reply == ack) && (answer.toString().contains("SEC1")))
                    emit activationContinue();
                else
                    notifyError(resourcetypeErrMsg);
                break;

            case setecresource:
                if (reply == ack)
                    emit activationContinue();
                else
                    notifyError(setresourceErrMsg);
                break;

            case readresource:
                if (reply == ack) {
                    QStringList resourceTypeList = m_resourceTypeList.getResourceTypeList();
                    m_ResourceHash[resourceTypeList.at(m_nIt)] = answer.toString();
                    m_nIt++;
                    if (m_nIt < resourceTypeList.count())
                        emit activationLoop();
                    else
                        emit activationContinue();
                }
                else
                    notifyError(resourceErrMsg);
                break;

            case fetchecalcunits:
            {
                qInfo("SEM: Units fetched: %s", qPrintable(answer.toString()));
                QStringList sl = answer.toString().split(';');
                if ((reply == ack) && (sl.length() >= 3)) {
                    m_masterErrCalcName = sl.at(0);
                    m_slaveErrCalcName = sl.at(1);
                    m_slave2ErrCalcName = sl.at(2);
                    emit activationContinue();
                }
                else
                    notifyError(fetchsececalcunitErrMsg);
                break;
            }

            case readrefInputalias:
            {
                if (reply == ack) {
                    QString alias = answer.toString();
                    if(alias != "0.0")
                        m_refInputDictionary.setAlias(m_sIt, answer.toString());
                    else
                        qWarning("SEM: Improper alias %s received for iterator %i", qPrintable(alias), m_nIt);
                    emit activationContinue();
                }
                else
                    notifyError(readaliasErrMsg);
                break;
            }

            case freeecalcunits:
                if (reply == ack) // we only continue if sec server manager acknowledges
                    emit deactivationContinue();
                else
                    notifyError(freesececalcunitErrMsg);
                break;

            case freeecresource:
                if (reply == ack)
                    emit deactivationContinue();
                else
                    notifyError(freeresourceErrMsg);
                break;

            case actualizeenergy:
            {
                if (reply == ack) {
                    // keep last values on (pending) abort / ignore post final responses
                    if((getStatus() & ECALCSTATUS::ABORT) == 0 && !m_finalResultStateMachine.isRunning()) {
                        m_nEnergyCounterActual = answer.toUInt();
                        m_fEnergy = 1.0 * m_nEnergyCounterActual / (m_pRefConstantPar->getValue().toDouble() * mEnergyUnitFactorHash[m_pInputUnitPar->getValue().toString()]);
                        m_pEnergyAct->setValue(m_fEnergy); // in MWh, kWh, Wh depends on selected unit for user input
                    }
                }
                else
                    notifyError(readsecregisterErrMsg);
                break;
            }


            case actualizepower:
            {
                if (reply == ack) {
                    // keep last values on (pending) abort / ignore post final responses
                    if((getStatus() & ECALCSTATUS::ABORT) == 0 && !m_finalResultStateMachine.isRunning()) {
                        m_fTimeSecondsActual = double(answer.toUInt()) * 0.001;
                        m_fPower = m_fEnergy * 3600.0 / m_fTimeSecondsActual; // in MW, kW, W depends on selected unit for user input
                        m_pPowerAct->setValue(m_fPower);
                        m_pTimeAct->setValue(m_fTimeSecondsActual);
                    }
                }
                else
                    notifyError(readsecregisterErrMsg);
                break;
            }

            case actualizestatus:
            {
                if (reply == ack) {
                    // keep last values on (pending) abort / ignore post final responses
                    quint32 status = getStatus();
                    if((status & ECALCSTATUS::ABORT) == 0 && !m_finalResultStateMachine.isRunning()) {
                        // once ready we leave status ready (continous mode)
                        setStatus((status & ECALCSTATUS::READY) | (answer.toUInt() & 7));
                    }
                }
                else
                    notifyError(readsecregisterErrMsg);
                break;
            }

            case setsync:
                if (reply == ack)
                    emit setupContinue();
                else
                    notifyError(setsyncErrMsg);
                break;

            case enableinterrupt:
            case setmeaspulses:
                if (reply == ack)
                    emit setupContinue();
                else
                    notifyError(writesecregisterErrMsg);
                break;

            case setmastermux:
            case setslavemux:
                if (reply == ack)
                    emit setupContinue();
                else
                    notifyError(setmuxErrMsg);
                break;

            case setmastermeasmode:
            case setslavemeasmode:
                if (reply == ack)
                    emit setupContinue();
                else
                    notifyError(setcmdidErrMsg);
                break;

            case startmeasurement:
                if (reply == ack)
                    emit setupContinue();
                else
                    notifyError(startmeasErrMsg);
                break;

            case stopmeas:
                if (reply == ack)
                    emit deactivationContinue();
                else
                    notifyError(stopmeasErrMsg);
                break;

            case setsecintnotifier:
                if (reply == ack)
                    emit activationContinue();
                else
                    notifyError(registerpcbnotifierErrMsg);
                break;

            case readintregister:
                if (reply == ack) {
                    m_nIntReg = answer.toInt() & 7;
                    emit interruptContinue();
                }
                else
                    notifyError(readsecregisterErrMsg);
                break;
            case resetintregister:
                if (reply == ack)
                    emit interruptContinue();
                else
                    notifyError(writesecregisterErrMsg);
                break;
            case readvicount:
                if (reply == ack)
                {
                    // Although we do not have high frequency measurements,
                    // incorporate still running check as we learned from sec1
                    // see cSec1ModuleMeasProgram::catchInterfaceAnswer /
                    // case readvicount
                    if((getStatus() & ECALCSTATUS::ABORT) == 0) {
                        m_nEnergyCounterFinal = answer.toLongLong();
                    }
                    emit interruptContinue();
                }
                else
                    notifyError(readsecregisterErrMsg);
                break;
            case readtcount:
                if (reply == ack) {// we only continue if sec server acknowledges
                    m_fTimeSecondsFinal = double(answer.toLongLong()) * 0.001;
                    emit interruptContinue();
                }
                else
                    notifyError(readsecregisterErrMsg);
                break;

            }
        }
    }
}

cSem1ModuleConfigData *cSem1ModuleMeasProgram::getConfData()
{
    return qobject_cast<cSem1ModuleConfiguration*>(m_pConfiguration.get())->getConfigurationData();
}

void cSem1ModuleMeasProgram::setInterfaceComponents()
{
    m_pRefInputPar->setValue(QVariant(getRefInputDisplayString(getConfData()->m_sRefInput.m_sPar)));
    m_pTargetedPar->setValue(QVariant(getConfData()->m_bTargeted.m_nActive));
    m_pMeasTimePar->setValue(QVariant(getConfData()->m_nMeasTime.m_nPar));
    m_pUpperLimitPar->setValue(QVariant(getConfData()->m_fUpperLimit.m_fPar));
    m_pLowerLimitPar->setValue(QVariant(getConfData()->m_fLowerLimit.m_fPar));
}

void cSem1ModuleMeasProgram::setInputUnitValidator()
{
    cStringValidator *sValidator = new cStringValidator(getEnergyUnitValidator());
    m_pInputUnitPar->setValidator(sValidator);
}

void cSem1ModuleMeasProgram::setValidators()
{
    cStringValidator *sValidator = new cStringValidator(m_REFAliasList);
    m_pRefInputPar->setValidator(sValidator);
    setInputUnitValidator();
}

void cSem1ModuleMeasProgram::setUnits()
{
    m_pPowerAct->setUnit(getPowerUnit());
    QString s = getEnergyUnit();
    m_pEnergyAct->setUnit(s);
    m_pT0InputPar->setUnit(s);
    m_pT1InputPar->setUnit(s);
    m_pInputUnitPar->setValue(s);
    // In case measurement is running, values are updated properly on next
    // interrupt (tested with vf-debugger). For a measuremnt finished we have to
    // recalc results with new units
    if(m_bActive && getStatus() & ECALCSTATUS::READY)
        setEMResult();
    m_pModule->exportMetaData();
}

QStringList cSem1ModuleMeasProgram::getEnergyUnitValidator()
{
    QStringList sl = getPowerUnitValidator();
    for (int i = 0; i < sl.count(); i++)
        sl.replace(i, sl.at(i)+"h");
    return sl;
}

QString cSem1ModuleMeasProgram::getEnergyUnit()
{
    QString powerType = m_refInputDictionary.getAlias(getConfData()->m_sRefInput.m_sPar);
    QString currentPowerUnit = m_pInputUnitPar->getValue().toString();
    return cUnitHelper::getNewEnergyUnit(powerType, currentPowerUnit, 3600);
}

QStringList cSem1ModuleMeasProgram::getPowerUnitValidator()
{
    QStringList sl;
    QString powType = m_refInputDictionary.getAlias(getConfData()->m_sRefInput.m_sPar);
    if (powType.contains('P'))
        sl = getConfData()->m_ActiveUnitList;
    if (powType.contains('Q'))
        sl = getConfData()->m_ReactiveUnitList;
    if (powType.contains('S'))
        sl = getConfData()->m_ApparentUnitList;
    return sl;
}

QString cSem1ModuleMeasProgram::getPowerUnit()
{
    QString powerType = m_refInputDictionary.getAlias(getConfData()->m_sRefInput.m_sPar);
    QString currentPowerUnit = m_pInputUnitPar->getValue().toString();
    return cUnitHelper::getNewPowerUnit(powerType, currentPowerUnit);
}

QString cSem1ModuleMeasProgram::getRefInputDisplayString(QString inputName)
{
    QString displayString = m_refInputDictionary.getAlias(inputName);
    QList<TRefInput> refInputList = getConfData()->m_refInpList;
    for(const auto &entry : refInputList) {
        if(entry.inputName == inputName) {
            displayString += entry.nameAppend;
            break;
        }
    }
    return displayString;
}

void cSem1ModuleMeasProgram::actualizeRefConstant()
{
    double constant = m_refConstantObserver.getConstant(getConfData()->m_sRefInput.m_sPar);
    m_pRefConstantPar->setValue(QVariant(constant));
    newRefConstant(QVariant(constant));
}

quint32 cSem1ModuleMeasProgram::getStatus()
{
    return m_pStatusAct->getValue().toUInt();
}

void cSem1ModuleMeasProgram::setStatus(quint32 status)
{
    m_pStatusAct->setValue(QVariant::fromValue<quint32>(status));
}

void cSem1ModuleMeasProgram::onRefConstantChanged(QString refPowerName)
{
    if(getConfData()->m_sRefInput.m_sPar == refPowerName) {
        stopMeasurement(true);
        actualizeRefConstant();
    }
}

void cSem1ModuleMeasProgram::handleSECInterrupt()
{
    if (!m_finalResultStateMachine.isRunning()) {
        m_finalResultStateMachine.setInitialState(&m_readIntRegisterState);
        m_finalResultStateMachine.start();
        m_ActualizeTimer->stop();
    }
}

void cSem1ModuleMeasProgram::updateDemoMeasurementResults()
{
    setStatus(ECALCSTATUS::READY); //still need more thoughts on this

    m_nEnergyCounterFinal = rand() % 10; //random value between 0 and 9
    m_fTimeSecondsFinal = rand() % 10 +1; //random value between 1 and 10
    newRefConstant(QVariant(3600000));
    setEMResult();
}

void cSem1ModuleMeasProgram::setDateTimeNow(QDateTime &var, VfModuleParameter *veinParam)
{
    var = QDateTime::currentDateTime();
    setDateTime(var, veinParam);
}

void cSem1ModuleMeasProgram::setDateTime(QDateTime var, VfModuleParameter *veinParam)
{
    veinParam->setValue(var.toString("dd-MM-yyyy HH:mm:ss"));
}

void cSem1ModuleMeasProgram::calculateMeasTime()
{
    m_measDuration = m_measStartDateTime.msecsTo(m_measEndDateTime);
    m_pMeasDurationMs->setValue(m_measDuration);
}

void cSem1ModuleMeasProgram::resourceManagerConnect()
{
    // first we try to get a connection to resource manager over proxy
    m_rmClient = Zera::Proxy::getInstance()->getConnectionSmart(m_pModule->getNetworkConfig()->m_rmServiceConnectionInfo,
                                                                m_pModule->getNetworkConfig()->m_tcpNetworkFactory);
    // and then we set connection resource manager interface's connection
    m_rmInterface.setClientSmart(m_rmClient); //
    resourceManagerConnectState.addTransition(m_rmClient.get(), &Zera::ProxyClient::connected, &m_IdentifyState);
    connect(&m_rmInterface, &AbstractServerInterface::serverAnswer, this, &cSem1ModuleMeasProgram::catchInterfaceAnswer);
    // todo insert timer for timeout and/or connect error conditions
    Zera::Proxy::getInstance()->startConnectionSmart(m_rmClient);
}

void cSem1ModuleMeasProgram::sendRMIdent()
{
    m_MsgNrCmdList[m_rmInterface.rmIdent(QString("Sem1Module%1").arg(m_pModule->getModuleNr()))] = sendrmident;
}

void cSem1ModuleMeasProgram::testSEC1Resource()
{
    m_MsgNrCmdList[m_rmInterface.getResourceTypes()] = testsec1resource;
}

void cSem1ModuleMeasProgram::setECResource()
{
    m_MsgNrCmdList[m_rmInterface.setResource("SEC1", "ECALCULATOR", 3)] = setecresource;
}

void cSem1ModuleMeasProgram::readResources()
{
    m_nIt = 0; // we want to read all resources from resourcetypelist
    emit activationContinue();
}

void cSem1ModuleMeasProgram::readResource()
{
    QString resourcetype = m_resourceTypeList.getResourceTypeList().at(m_nIt);
    m_MsgNrCmdList[m_rmInterface.getResources(resourcetype)] = readresource;
}

void cSem1ModuleMeasProgram::testSemInputs()
{
    const auto refInpList = getConfData()->m_refInpList;
    qint32 refInCountLeftToCheck = refInpList.count();
    QStringList resourceTypeList = m_resourceTypeList.getResourceTypeList();
    for (int refInputNo = 0; refInputNo < refInpList.count(); refInputNo++) {
        QString refPowerName = refInpList[refInputNo].inputName;
        for (int resourceTypeNo = 0; resourceTypeNo < resourceTypeList.count(); resourceTypeNo++) {
            QString resourcelist = m_ResourceHash[resourceTypeList[resourceTypeNo]];
            if (resourcelist.contains(refPowerName)) {
                refInCountLeftToCheck--;
                m_refInputDictionary.addReferenceInput(refPowerName, resourceTypeList[resourceTypeNo]);
                break;
            }
        }
    }
    if (refInCountLeftToCheck == 0) // we found all our configured Inputs
        emit activationContinue(); // so lets go on
    else
        notifyError(resourceErrMsg);
}

void cSem1ModuleMeasProgram::ecalcServerConnect()
{
    Zera::ProxyClientPtr secClient = Zera::Proxy::getInstance()->getConnectionSmart(
        m_pModule->getNetworkConfig()->m_secServiceConnectionInfo,
        m_pModule->getNetworkConfig()->m_tcpNetworkFactory);
    // and then we set ecalcalculator interface's connection
    m_secInterface->setClientSmart(secClient);
    m_ecalcServerConnectState.addTransition(secClient.get(), &Zera::ProxyClient::connected, &m_fetchECalcUnitsState);
    connect(m_secInterface.get(), &AbstractServerInterface::serverAnswer, this, &cSem1ModuleMeasProgram::catchInterfaceAnswer);
    // todo insert timer for timeout and/or connect error conditions
    Zera::Proxy::getInstance()->startConnectionSmart(secClient);
}

void cSem1ModuleMeasProgram::fetchECalcUnits()
{
    m_MsgNrCmdList[m_secInterface->setECalcUnit(3)] = fetchecalcunits; // we need 3 ecalc units to cascade
}

void cSem1ModuleMeasProgram::pcbServerConnect()
{
    m_pcbClient = Zera::Proxy::getInstance()->getConnectionSmart(
        m_pModule->getNetworkConfig()->m_pcbServiceConnectionInfo,
        m_pModule->getNetworkConfig()->m_tcpNetworkFactory);
    m_pcbInterface->setClientSmart(m_pcbClient);
    m_pcbServerConnectState.addTransition(m_pcbClient.get(), &Zera::ProxyClient::connected, &m_readREFInputsState);
    connect(m_pcbInterface.get(), &AbstractServerInterface::serverAnswer, this, &cSem1ModuleMeasProgram::catchInterfaceAnswer);
    Zera::Proxy::getInstance()->startConnectionSmart(m_pcbClient);
}

void cSem1ModuleMeasProgram::readREFInputs()
{
    m_sItList = m_refInputDictionary.getInputNameList();
    emit activationContinue();
}

void cSem1ModuleMeasProgram::readREFInputAlias()
{
    m_sIt = m_sItList.takeFirst();
    // we will read the powertype of the reference frequency input and will use this as our alias ! for example P, +P ....
    m_MsgNrCmdList[m_pcbInterface->getPowTypeSource(m_sIt)] = readrefInputalias;

}

void cSem1ModuleMeasProgram::readREFInputDone()
{
    if (m_sItList.isEmpty())
        emit activationContinue();
    else
        emit activationLoop();
}

void cSem1ModuleMeasProgram::setpcbREFConstantNotifier()
{
    if (getConfData()->m_nRefInpCount > 0) {
        connect(&m_refConstantObserver, &RefPowerConstantObserver::sigRegistrationFinished, this, [this](bool ok) {
            if(ok) {
                actualizeRefConstant();
                connect(&m_refConstantObserver, &RefPowerConstantObserver::sigRefConstantChanged,
                        this, &cSem1ModuleMeasProgram::onRefConstantChanged);
                emit activationContinue();
            }
            else
                notifyError(registerpcbnotifierErrMsg);
        });
        m_refConstantObserver.registerNofifications(m_pcbInterface, m_refInputDictionary.getInputNameList());
    }
    else
        emit activationContinue(); // if no ref constant notifier (standalone error calc) we directly go on
}

void cSem1ModuleMeasProgram::setsecINTNotifier()
{
    m_MsgNrCmdList[m_secInterface->registerNotifier(QString("ECAL:%1:R%2?").arg(m_masterErrCalcName).arg(ECALCREG::INTREG))] = setsecintnotifier;
}

void cSem1ModuleMeasProgram::activationDone()
{
    cSem1ModuleConfigData *confData = getConfData();
    for (int i = 0; i < confData->m_refInpList.count(); i++) {
        QString displayString = getRefInputDisplayString(confData->m_refInpList.at(i).inputName);
        m_REFAliasList.append(displayString); // build up a fixed sorted list of alias
        m_refInputDictionary.setDisplayedString(confData->m_refInpList.at(i).inputName, displayString);
    }

    connect(m_ActualizeTimer.get(), &TimerTemplateQt::sigExpired, this, &cSem1ModuleMeasProgram::Actualize);

    connect(m_pStartStopPar, &VfModuleParameter::sigValueChanged, this, &cSem1ModuleMeasProgram::newStartStop);
    connect(m_pRefInputPar, &VfModuleParameter::sigValueChanged, this, &cSem1ModuleMeasProgram::newRefInput);

    connect(m_pTargetedPar, &VfModuleParameter::sigValueChanged, this , &cSem1ModuleMeasProgram::newTargeted);
    connect(m_pMeasTimePar, &VfModuleParameter::sigValueChanged, this , &cSem1ModuleMeasProgram::newMeasTime);
    connect(m_pT0InputPar, &VfModuleParameter::sigValueChanged, this , &cSem1ModuleMeasProgram::newT0Input);
    connect(m_pT1InputPar, &VfModuleParameter::sigValueChanged, this , &cSem1ModuleMeasProgram::newT1Input);
    connect(m_pInputUnitPar, &VfModuleParameter::sigValueChanged, this, &cSem1ModuleMeasProgram::newUnit);
    connect(m_pUpperLimitPar, &VfModuleParameter::sigValueChanged, this, &cSem1ModuleMeasProgram::newUpperLimit);
    connect(m_pLowerLimitPar, &VfModuleParameter::sigValueChanged, this, &cSem1ModuleMeasProgram::newLowerLimit);

    setInterfaceComponents(); // actualize interface components
    setValidators();
    setUnits();

    m_bActive = true;
    emit activated();
}

void cSem1ModuleMeasProgram::stopECCalculator()
{
    stopMeasurement(true);
}

void cSem1ModuleMeasProgram::freeECalculator()
{
    m_bActive = false;
    m_MsgNrCmdList[m_secInterface->freeECalcUnits()] = freeecalcunits;
}

void cSem1ModuleMeasProgram::freeECResource()
{
    m_MsgNrCmdList[m_rmInterface.freeResource("SEC1", "ECALCULATOR")] = freeecresource;
}

void cSem1ModuleMeasProgram::deactivationDone()
{
    disconnect(&m_rmInterface, 0, this, 0);
    disconnect(m_secInterface.get(), 0, this, 0);
    disconnect(m_pcbInterface.get(), 0, this, 0);
    disconnect(m_pStartStopPar, 0, this, 0);
    disconnect(m_pTargetedPar, 0, this, 0);
    disconnect(m_pRefInputPar, 0, this, 0);
    disconnect(m_pMeasTimePar, 0, this, 0);
    disconnect(m_pT0InputPar, 0, this, 0);
    disconnect(m_pT1InputPar, 0, this, 0);
    emit deactivated();
}

void cSem1ModuleMeasProgram::setSync()
{
    m_MsgNrCmdList[m_secInterface->setSync(m_slaveErrCalcName, m_masterErrCalcName)] = setsync;
}

void cSem1ModuleMeasProgram::setSync2()
{
    m_MsgNrCmdList[m_secInterface->setSync(m_slave2ErrCalcName, m_masterErrCalcName)] = setsync;
}

void cSem1ModuleMeasProgram::setMeaspulses()
{
    if (m_pTargetedPar->getValue().toInt() == 0)
        m_nTimerCountStart = Zera::cSECInterface::maxSecCounterInitVal; // we simply set max. time -> approx. 50 days
    else
        m_nTimerCountStart = m_pMeasTimePar->getValue().toLongLong() * 1000;

    m_MsgNrCmdList[m_secInterface->writeRegister(m_masterErrCalcName, ECALCREG::MTCNTin, m_nTimerCountStart)] = setmeaspulses;
}

void cSem1ModuleMeasProgram::setMasterMux()
{
    m_MsgNrCmdList[m_secInterface->setMux(m_masterErrCalcName, QString("t1ms"))] = setmastermux; // to be discussed
}

void cSem1ModuleMeasProgram::setSlaveMux()
{
    QString refPowerName = getConfData()->m_sRefInput.m_sPar;
    m_MsgNrCmdList[m_secInterface->setMux(m_slaveErrCalcName, refPowerName)] = setslavemux;
}

void cSem1ModuleMeasProgram::setSlave2Mux()
{
    m_MsgNrCmdList[m_secInterface->setMux(m_slave2ErrCalcName, QString("t1ms"))] = setslavemux;
}

void cSem1ModuleMeasProgram::setMasterMeasMode()
{
    m_MsgNrCmdList[m_secInterface->setCmdid(m_masterErrCalcName, ECALCCMDID::SINGLEERRORMASTER)] = setmastermeasmode;
}

void cSem1ModuleMeasProgram::setSlaveMeasMode()
{
    m_MsgNrCmdList[m_secInterface->setCmdid(m_slaveErrCalcName, ECALCCMDID::ERRORMEASSLAVE)] = setslavemeasmode;
}

void cSem1ModuleMeasProgram::setSlave2MeasMode()
{
    m_MsgNrCmdList[m_secInterface->setCmdid(m_slave2ErrCalcName, ECALCCMDID::ERRORMEASSLAVE)] = setslavemeasmode;
}

void cSem1ModuleMeasProgram::enableInterrupt()
{
    // in case of targeted mode we want an interrupt when ready
    // in case of not targeted mode we set the time to maximum and the user will stop the measurement by pressing stop
    // so we program enable int. in any case
    m_MsgNrCmdList[m_secInterface->writeRegister(m_masterErrCalcName, ECALCREG::INTMASK, ECALCINT::MTCount0)] = enableinterrupt;
}

void cSem1ModuleMeasProgram::startMeasurement()
{
    setDateTimeNow(m_measStartDateTime, m_pMeasStartTime);
    m_measDuration = 0;
    m_pMeasDurationMs->setValue(m_measDuration);
    m_measEndDateTime = QDateTime();
    setDateTime(m_measEndDateTime, m_pMeasEndTime);
    if(!m_pModule->getDemo())
        m_MsgNrCmdList[m_secInterface->start(m_masterErrCalcName)] = startmeasurement;
    setStatus(ECALCSTATUS::ARMED);
    m_fEnergy = 0.0;
    m_pEnergyAct->setValue(m_fEnergy);
    m_fPower = 0.0;
    m_pPowerAct->setValue(m_fPower);
    if(m_pModule->getDemo()) {
        updateDemoMeasurementResults();
        emit setupContinue();
    }
}

void cSem1ModuleMeasProgram::startMeasurementDone()
{
    Actualize(); // we actualize at once after started
    m_ActualizeTimer->start(); // and after current interval
}

void cSem1ModuleMeasProgram::readIntRegister()
{
    m_MsgNrCmdList[m_secInterface->readRegister(m_masterErrCalcName, ECALCREG::INTREG)] = readintregister;
}

void cSem1ModuleMeasProgram::resetIntRegister()
{
    m_MsgNrCmdList[m_secInterface->intAck(m_masterErrCalcName, 0xF)] = resetintregister; // we reset all here
}

void cSem1ModuleMeasProgram::readVICountact()
{
    m_MsgNrCmdList[m_secInterface->readRegister(m_slaveErrCalcName, ECALCREG::MTCNTfin)] = readvicount;
}

void cSem1ModuleMeasProgram::readTCountact()
{
    m_MsgNrCmdList[m_secInterface->readRegister(m_slave2ErrCalcName, ECALCREG::MTCNTfin)] = readtcount;
    // non targeted has been stopped already in newStartStop()
    if(getConfData()->m_bTargeted.m_nActive)
        m_MsgNrCmdList[m_secInterface->stop(m_masterErrCalcName)] = stopmeas;
    m_pStartStopPar->setValue(QVariant(0)); // restart enable
    setStatus(ECALCSTATUS::READY);
}

void cSem1ModuleMeasProgram::onEMResultState()
{
    setDateTimeNow(m_measEndDateTime, m_pMeasEndTime);
    calculateMeasTime();
    setEMResult();
}

void cSem1ModuleMeasProgram::setEMResult()
{
    double WRef =  m_nEnergyCounterFinal / m_pRefConstantPar->getValue().toDouble();
    double time = m_fTimeSecondsFinal;
    double WDut = (m_pT1InputPar->getValue().toDouble() - m_pT0InputPar->getValue().toDouble()) * mEnergyUnitFactorHash[m_pInputUnitPar->getValue().toString()];
    if (WRef == 0) {
        m_fResult = qQNaN();
        m_eRating = ECALCRESULT::RESULT_UNFINISHED;
    }
    else {
        m_fResult = (WDut - WRef) * 100.0 / WRef;
        setRating();
    }

    m_fEnergy = WRef / mEnergyUnitFactorHash[m_pInputUnitPar->getValue().toString()];
    m_fPower = m_fEnergy * 3600.0 / time;

    m_pTimeAct->setValue(QVariant(time));
    m_pResultAct->setValue(QVariant(m_fResult));
    m_pEnergyAct->setValue(QVariant(m_fEnergy));
    m_pPowerAct->setValue(QVariant(m_fPower));
}

void cSem1ModuleMeasProgram::setRating()
{
    if (getStatus() & ECALCSTATUS::READY) {
        if ( (m_fResult >= getConfData()->m_fLowerLimit.m_fPar) && (m_fResult <= getConfData()->m_fUpperLimit.m_fPar))
            m_eRating = ECALCRESULT::RESULT_PASSED;
        else
            m_eRating = ECALCRESULT::RESULT_FAILED;
    }
    else
        m_eRating = ECALCRESULT::RESULT_UNFINISHED;
    m_pRatingAct->setValue(int(m_eRating));
}

void cSem1ModuleMeasProgram::newStartStop(QVariant startstop)
{
    bool ok;
    int ss = startstop.toInt(&ok);
    if (ss > 0) { // we get started
        if (!m_startMeasurementMachine.isRunning())
            m_startMeasurementMachine.start();
        // setsync
        // mtvorwahl setzen
        // master -> Input mux setzen
        // slave1 + slave2 -> Input mux setzen
        // meas mode setzen + arm
        // m_ActualizeTimer.start();
    }
    else {
        if (getConfData()->m_bTargeted.m_nActive > 0)
            stopMeasurement(true);
        else {
            m_MsgNrCmdList[m_secInterface->stop(m_masterErrCalcName)] = stopmeas;
            m_ActualizeTimer->stop();
            // if we are not "targeted" we handle pressing stop as if the
            // measurement became ready and an interrupt occured
            if (!m_finalResultStateMachine.isRunning()) {
                m_finalResultStateMachine.start();
            }
        }
    }
}

void cSem1ModuleMeasProgram::newRefConstant(QVariant refconst)
{
    m_pRefConstantPar->setValue(refconst);
    setInterfaceComponents();
    emit m_pModule->parameterChanged();
}

void cSem1ModuleMeasProgram::newRefInput(QVariant refinput)
{
    QString refPowerName = m_refInputDictionary.getInputNameFromDisplayedName(refinput.toString());
    getConfData()->m_sRefInput.m_sPar = refPowerName;
    actualizeRefConstant();
    setInterfaceComponents();

    // if the reference input changes P <-> Q <-> S it is necessary to change energyunit and powerunit and their validators
    setValidators();
    setUnits();
    m_pRefFreqInput->setValue(refPowerName);
    emit m_pModule->parameterChanged();
}

void cSem1ModuleMeasProgram::newTargeted(QVariant targeted)
{
    getConfData()->m_bTargeted.m_nActive = targeted.toInt();
    setInterfaceComponents();
    emit m_pModule->parameterChanged();
}

void cSem1ModuleMeasProgram::newMeasTime(QVariant meastime)
{
    getConfData()->m_nMeasTime.m_nPar = meastime.toInt();
    setInterfaceComponents();
    emit m_pModule->parameterChanged();
}

void cSem1ModuleMeasProgram::newT0Input(QVariant t0input)
{
    m_pT0InputPar->setValue(t0input);
    setEMResult();
    emit m_pModule->parameterChanged();
}

void cSem1ModuleMeasProgram::newT1Input(QVariant t1input)
{
    m_pT1InputPar->setValue(t1input);
    setEMResult();

    emit m_pModule->parameterChanged();
}

void cSem1ModuleMeasProgram::newUnit(QVariant unit)
{
    m_pInputUnitPar->setValue(unit.toString());
    setInterfaceComponents();
    setUnits();
    emit m_pModule->parameterChanged();
}

void cSem1ModuleMeasProgram::newUpperLimit(QVariant limit)
{
    bool ok;
    getConfData()->m_fUpperLimit.m_fPar = limit.toDouble(&ok);
    setInterfaceComponents();
    setRating();

    emit m_pModule->parameterChanged();
}

void cSem1ModuleMeasProgram::newLowerLimit(QVariant limit)
{
    bool ok;
    getConfData()->m_fLowerLimit.m_fPar = limit.toDouble(&ok);
    setInterfaceComponents();
    setRating();
    emit m_pModule->parameterChanged();
}

void cSem1ModuleMeasProgram::Actualize()
{
    if(!m_pModule->getDemo()) {
        m_MsgNrCmdList[m_secInterface->readRegister(m_masterErrCalcName, ECALCREG::STATUS)] = actualizestatus;
        m_MsgNrCmdList[m_secInterface->readRegister(m_slaveErrCalcName, ECALCREG::MTCNTact)] = actualizeenergy;
        m_MsgNrCmdList[m_secInterface->readRegister(m_slave2ErrCalcName, ECALCREG::MTCNTact)] = actualizepower;
    }
}

void cSem1ModuleMeasProgram::clientActivationChanged(bool bActive)
{
    // Adjust our m_ActualizeTimer timeout to our client's needs
    m_ActualizeTimer = TimerFactoryQt::createPeriodic(bActive ? m_nActualizeIntervallHighFreq : m_nActualizeIntervallLowFreq);
    connect(m_ActualizeTimer.get(), &TimerTemplateQt::sigExpired, this, &cSem1ModuleMeasProgram::Actualize);
    m_ActualizeTimer->start();
}

void cSem1ModuleMeasProgram::stopMeasurement(bool bAbort)
{
    if(bAbort)
        setStatus(ECALCSTATUS::ABORT);
    m_MsgNrCmdList[m_secInterface->stop(m_masterErrCalcName)] = stopmeas;
    m_pStartStopPar->setValue(QVariant(0));
    m_ActualizeTimer->stop();
}

bool cSem1ModuleMeasProgram::found(QList<TRefInput> &list, QString searched)
{
    for (int i = 0; i < list.count(); i++) {
        if (list.at(i).inputName.contains(searched))
            return true;
    }
    return false;
}

}
