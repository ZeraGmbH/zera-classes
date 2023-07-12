#include "sec1modulemeasprogram.h"
#include "sec1module.h"
#include "sec1moduleconfigdata.h"
#include "sec1moduleconfiguration.h"
#include "unithelper.h"
#include "errormessages.h"
#include <proxy.h>
#include <proxyclient.h>
#include <scpiinfo.h>
#include <doublevalidator.h>
#include <regexvalidator.h>
#include <intvalidator.h>
#include <stringvalidator.h>
#include <reply.h>
#include <QJsonDocument>
#include <math.h>

namespace SEC1MODULE
{

cSec1ModuleMeasProgram::cSec1ModuleMeasProgram(cSec1Module* module, std::shared_ptr<cBaseModuleConfiguration> pConfiguration) :
    cBaseMeasProgram(pConfiguration),
    m_pModule(module)
{
    // we have to instantiate a working resource manager and secserver interface
    m_pSECInterface = new Zera::cSECInterface();
    m_pPCBInterface = new Zera::cPCBInterface();

    m_IdentifyState.addTransition(this, &cSec1ModuleMeasProgram::activationContinue, &m_testSEC1ResourceState);
    m_testSEC1ResourceState.addTransition(this, &cSec1ModuleMeasProgram::activationContinue, &m_setECResourceState); // test presence of sec1 resource
    m_setECResourceState.addTransition(this, &cSec1ModuleMeasProgram::activationContinue, &m_readResourceTypesState); // claim 2 ecalculator units
    m_readResourceTypesState.addTransition(this, &cSec1ModuleMeasProgram::activationContinue, &m_readResourcesState); // read all resources types
    m_readResourcesState.addTransition(this, &cSec1ModuleMeasProgram::activationContinue, &m_readResourceState); // init read resources
    m_readResourceState.addTransition(this, &cSec1ModuleMeasProgram::activationLoop, &m_readResourceState); // read their resources into list
    m_readResourceState.addTransition(this, &cSec1ModuleMeasProgram::activationContinue, &m_testSecInputsState); // go on if done
    m_testSecInputsState.addTransition(this, &cSec1ModuleMeasProgram::activationContinue, &m_ecalcServerConnectState); // test all configured Inputs
    //m_ecalcServerConnectState.addTransition(this, &cSec1ModuleMeasProgram::activationContinue, &m_fetchECalcUnitsState); // connect to ecalc server
    //transition from this state to m_fetch....is done in ecalcServerConnect
    m_fetchECalcUnitsState.addTransition(this, &cSec1ModuleMeasProgram::activationContinue, &m_pcbServerConnectState); // connect to pcbserver


    //m_pcbServerConnectState.addTransition(this, &cSec1ModuleMeasProgram::activationContinue, &m_pcbServerConnectState);
    //transition from this state to m_readREFInputsState....is done in pcbServerConnect
    m_readREFInputsState.addTransition(this, &cSec1ModuleMeasProgram::activationContinue, &m_readREFInputAliasState);
    m_readREFInputAliasState.addTransition(this, &cSec1ModuleMeasProgram::activationContinue, &m_readREFInputDoneState);
    m_readREFInputDoneState.addTransition(this, &cSec1ModuleMeasProgram::activationLoop, &m_readREFInputAliasState);
    m_readREFInputDoneState.addTransition(this, &cSec1ModuleMeasProgram::activationContinue, &m_readDUTInputsState);
    m_readDUTInputsState.addTransition(this, &cSec1ModuleMeasProgram::activationContinue, &m_readDUTInputAliasState);
    m_readDUTInputAliasState.addTransition(this, &cSec1ModuleMeasProgram::activationContinue, &m_readDUTInputDoneState);
    m_readDUTInputDoneState.addTransition(this, &cSec1ModuleMeasProgram::activationLoop, &m_readDUTInputAliasState);
    m_readDUTInputDoneState.addTransition(this, &cSec1ModuleMeasProgram::activationContinue, &m_setpcbREFConstantNotifierState);

    m_setpcbREFConstantNotifierState.addTransition(this, &cSec1ModuleMeasProgram::activationContinue, &m_setsecINTNotifierState);
    m_setsecINTNotifierState.addTransition(this, &cSec1ModuleMeasProgram::activationContinue, &m_activationDoneState);

    m_activationMachine.addState(&resourceManagerConnectState);
    m_activationMachine.addState(&m_IdentifyState);
    m_activationMachine.addState(&m_testSEC1ResourceState);
    m_activationMachine.addState(&m_setECResourceState);
    m_activationMachine.addState(&m_readResourceTypesState);
    m_activationMachine.addState(&m_readResourcesState);
    m_activationMachine.addState(&m_readResourceState);
    m_activationMachine.addState(&m_testSecInputsState);
    m_activationMachine.addState(&m_ecalcServerConnectState);
    m_activationMachine.addState(&m_fetchECalcUnitsState);
    m_activationMachine.addState(&m_pcbServerConnectState);
    m_activationMachine.addState(&m_readREFInputsState);
    m_activationMachine.addState(&m_readREFInputAliasState);
    m_activationMachine.addState(&m_readREFInputDoneState);
    m_activationMachine.addState(&m_readDUTInputsState);
    m_activationMachine.addState(&m_readDUTInputAliasState);
    m_activationMachine.addState(&m_readDUTInputDoneState);
    m_activationMachine.addState(&m_setpcbREFConstantNotifierState);
    m_activationMachine.addState(&m_setsecINTNotifierState);
    m_activationMachine.addState(&m_activationDoneState);

    m_activationMachine.setInitialState(&resourceManagerConnectState);

    connect(&resourceManagerConnectState, &QState::entered, this, &cSec1ModuleMeasProgram::resourceManagerConnect);
    connect(&m_IdentifyState, &QState::entered, this, &cSec1ModuleMeasProgram::sendRMIdent);
    connect(&m_testSEC1ResourceState, &QState::entered, this, &cSec1ModuleMeasProgram::testSEC1Resource);
    connect(&m_setECResourceState, &QState::entered, this, &cSec1ModuleMeasProgram::setECResource);
    connect(&m_readResourceTypesState, &QState::entered, this, &cSec1ModuleMeasProgram::readResourceTypes);
    connect(&m_readResourcesState, &QState::entered, this, &cSec1ModuleMeasProgram::readResources);
    connect(&m_readResourceState, &QState::entered, this, &cSec1ModuleMeasProgram::readResource);
    connect(&m_testSecInputsState, &QState::entered, this, &cSec1ModuleMeasProgram::testSecInputs);
    connect(&m_ecalcServerConnectState, &QState::entered, this, &cSec1ModuleMeasProgram::ecalcServerConnect);
    connect(&m_fetchECalcUnitsState, &QState::entered, this, &cSec1ModuleMeasProgram::fetchECalcUnits);
    connect(&m_pcbServerConnectState, &QState::entered, this, &cSec1ModuleMeasProgram::pcbServerConnect);
    connect(&m_readREFInputsState, &QState::entered, this, &cSec1ModuleMeasProgram::readREFInputs);
    connect(&m_readREFInputAliasState, &QState::entered, this, &cSec1ModuleMeasProgram::readREFInputAlias);
    connect(&m_readREFInputDoneState, &QState::entered, this, &cSec1ModuleMeasProgram::readREFInputDone);
    connect(&m_readDUTInputsState, &QState::entered, this, &cSec1ModuleMeasProgram::readDUTInputs);
    connect(&m_readDUTInputAliasState, &QState::entered, this, &cSec1ModuleMeasProgram::readDUTInputAlias);
    connect(&m_readDUTInputDoneState, &QState::entered, this, &cSec1ModuleMeasProgram::readDUTInputDone);
    connect(&m_setpcbREFConstantNotifierState, &QState::entered, this, &cSec1ModuleMeasProgram::setpcbREFConstantNotifier);
    connect(&m_setsecINTNotifierState, &QState::entered, this, &cSec1ModuleMeasProgram::setsecINTNotifier);
    connect(&m_activationDoneState, &QState::entered, this, &cSec1ModuleMeasProgram::activationDone);

    // setting up statemachine to free the occupied resources
    m_stopECalculatorState.addTransition(this, &cSec1ModuleMeasProgram::deactivationContinue, &m_freeECalculatorState);
    m_freeECalculatorState.addTransition(this, &cSec1ModuleMeasProgram::deactivationContinue, &m_freeECResource);
    m_freeECResource.addTransition(this, &cSec1ModuleMeasProgram::deactivationContinue, &m_deactivationDoneState);

    m_deactivationMachine.addState(&m_stopECalculatorState);
    m_deactivationMachine.addState(&m_freeECalculatorState);
    m_deactivationMachine.addState(&m_freeECResource);
    m_deactivationMachine.addState(&m_deactivationDoneState);

    m_deactivationMachine.setInitialState(&m_stopECalculatorState);

    connect(&m_stopECalculatorState, &QState::entered, this, &cSec1ModuleMeasProgram::stopECCalculator);
    connect(&m_freeECalculatorState, &QState::entered, this, &cSec1ModuleMeasProgram::freeECalculator);
    connect(&m_freeECResource, &QState::entered, this, &cSec1ModuleMeasProgram::freeECResource);
    connect(&m_deactivationDoneState, &QState::entered, this, &cSec1ModuleMeasProgram::deactivationDone);

    // setting up statemachine used when starting a measurement

    m_setsyncState.addTransition(this, &cSec1ModuleMeasProgram::setupContinue, &m_setMeaspulsesState);
    m_setMeaspulsesState.addTransition(this, &cSec1ModuleMeasProgram::setupContinue, &m_setMasterMuxState);
    m_setMasterMuxState.addTransition(this, &cSec1ModuleMeasProgram::setupContinue, &m_setSlaveMuxState);
    m_setSlaveMuxState.addTransition(this, &cSec1ModuleMeasProgram::setupContinue, &m_setMasterMeasModeState);
    m_setMasterMeasModeState.addTransition(this, &cSec1ModuleMeasProgram::setupContinue, &m_setSlaveMeasModeState);
    m_setSlaveMeasModeState.addTransition(this, &cSec1ModuleMeasProgram::setupContinue, &m_enableInterruptState);
    m_enableInterruptState.addTransition(this, &cSec1ModuleMeasProgram::setupContinue, &m_startMeasurementState);
    m_startMeasurementState.addTransition(this, &cSec1ModuleMeasProgram::setupContinue, &m_startMeasurementDoneState);

    m_startMeasurementMachine.addState(&m_setsyncState);
    m_startMeasurementMachine.addState(&m_setMeaspulsesState);
    m_startMeasurementMachine.addState(&m_setMasterMuxState);
    m_startMeasurementMachine.addState(&m_setSlaveMuxState);
    m_startMeasurementMachine.addState(&m_setMasterMeasModeState);
    m_startMeasurementMachine.addState(&m_setSlaveMeasModeState);
    m_startMeasurementMachine.addState(&m_enableInterruptState);
    m_startMeasurementMachine.addState(&m_startMeasurementState);
    m_startMeasurementMachine.addState(&m_startMeasurementDoneState);

    m_startMeasurementMachine.setInitialState(&m_setsyncState);

    connect(&m_setsyncState, &QState::entered, this, &cSec1ModuleMeasProgram::setSync);
    connect(&m_setMeaspulsesState, &QState::entered, this, &cSec1ModuleMeasProgram::setMeaspulses);
    connect(&m_setMasterMuxState, &QState::entered, this, &cSec1ModuleMeasProgram::setMasterMux);
    connect(&m_setSlaveMuxState, &QState::entered, this, &cSec1ModuleMeasProgram::setSlaveMux);
    connect(&m_setMasterMeasModeState, &QState::entered, this, &cSec1ModuleMeasProgram::setMasterMeasMode);
    connect(&m_setSlaveMeasModeState, &QState::entered, this, &cSec1ModuleMeasProgram::setSlaveMeasMode);
    connect(&m_enableInterruptState, &QState::entered, this, &cSec1ModuleMeasProgram::enableInterrupt);
    connect(&m_startMeasurementState, &QState::entered, this, &cSec1ModuleMeasProgram::startMeasurement);
    connect(&m_startMeasurementDoneState, &QState::entered, this, &cSec1ModuleMeasProgram::startMeasurementDone);

    // setting up statemachine for interrupt handling (Interrupt is thrown on measuremnt finished)
    m_readIntRegisterState.addTransition(this, &cSec1ModuleMeasProgram::interruptContinue, &m_readMTCountactState);
    m_readMTCountactState.addTransition(this, &cSec1ModuleMeasProgram::interruptContinue, &m_calcResultAndResetIntState);
    m_calcResultAndResetIntState.addTransition(this, &cSec1ModuleMeasProgram::interruptContinue, &m_FinalState);

    m_InterrupthandlingStateMachine.addState(&m_readIntRegisterState);
    m_InterrupthandlingStateMachine.addState(&m_readMTCountactState);
    m_InterrupthandlingStateMachine.addState(&m_calcResultAndResetIntState);
    m_InterrupthandlingStateMachine.addState(&m_FinalState);

    m_InterrupthandlingStateMachine.setInitialState(&m_readIntRegisterState);

    connect(&m_readIntRegisterState, &QState::entered, this, &cSec1ModuleMeasProgram::readIntRegister);
    connect(&m_readMTCountactState, &QState::entered, this, &cSec1ModuleMeasProgram::readMTCountact);
    connect(&m_calcResultAndResetIntState, &QState::entered, this, &cSec1ModuleMeasProgram::setECResultAndResetInt);
    connect(&m_FinalState, &QState::entered, this, &cSec1ModuleMeasProgram::checkForRestart);
}


cSec1ModuleMeasProgram::~cSec1ModuleMeasProgram()
{
    int n = getConfData()->m_refInpList.count();
    for (int i = 0; i < n; i++) {
        cSec1ModuleConfigData::TRefInput refInput = getConfData()->m_refInpList.at(i);
        siInfo = mREFSecInputInfoHash.take(refInput.inputName); // change the hash for access via alias
        delete siInfo;
    }
    n = getConfData()->m_dutInpList.count();
    for (int i = 0; i < n; i++) {
        siInfo = mDUTSecInputInfoHash.take(getConfData()->m_dutInpList.at(i)); // change the hash for access via alias
        delete siInfo;
    }
    delete m_pSECInterface;
    Zera::Proxy::getInstance()->releaseConnection(m_pSECClient);
    delete m_pPCBInterface;
    Zera::Proxy::getInstance()->releaseConnection(m_pPCBClient);
}


void cSec1ModuleMeasProgram::start()
{
}


void cSec1ModuleMeasProgram::stop()
{
}


void cSec1ModuleMeasProgram::generateInterface()
{
    QString s;
    QString key;

    QString modNr = QString("%1").arg(m_pModule->getModuleNr(),4,10,QChar('0'));

    m_pDutInputPar = new VfModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                              key = QString("PAR_DutInput"),
                                              QString("DUT input"),
                                              QVariant(s = "Unknown"));
    m_pDutInputPar->setSCPIInfo(new cSCPIInfo("CALCULATE", QString("%1:DUTSOURCE").arg(modNr), "10", m_pDutInputPar->getName(), "0", ""));
    m_pModule->veinModuleParameterHash[key] = m_pDutInputPar; // for modules use

    m_pRefInputPar = new VfModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                              key = QString("PAR_RefInput"),
                                              QString("REF input"),
                                              QVariant(s = "Unknown"));
    m_pRefInputPar->setSCPIInfo(new cSCPIInfo("CALCULATE", QString("%1:REFSOURCE").arg(modNr), "10", m_pRefInputPar->getName(), "0", ""));
    m_pModule->veinModuleParameterHash[key] = m_pRefInputPar; // for modules use

    m_pRefConstantPar = new VfModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                                 key = QString("PAR_RefConstant"),
                                                 QString("REF constant"),
                                                 QVariant((double)0.0));
    m_pRefConstantPar->setSCPIInfo(new cSCPIInfo("CALCULATE", QString("%1:REFCONSTANT").arg(modNr ), "10", m_pRefConstantPar->getName(), "0", ""));
    m_pModule->veinModuleParameterHash[key] = m_pRefConstantPar; // for modules use
    cDoubleValidator *dValidator;
    dValidator = new cDoubleValidator(1.0, 1.0e20, 1e-4);
    m_pRefConstantPar->setValidator(dValidator);

    m_pDutConstantPar = new VfModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                                 key = QString("PAR_DutConstant"),
                                                 QString("DUT constant"),
                                                 QVariant((double)0.0));
    m_pDutConstantPar->setSCPIInfo(new cSCPIInfo("CALCULATE", QString("%1:DUTCONSTANT").arg(modNr), "10", m_pDutConstantPar->getName(), "0", ""));
    m_pModule->veinModuleParameterHash[key] = m_pDutConstantPar; // for modules use
    dValidator = new cDoubleValidator(1e-6, 1.0e20, 1e-5);
    m_pDutConstantPar->setValidator(dValidator);

    m_pDutConstantUScaleNum = new VfModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                                 key = QString("PAR_DutConstantUScaleNum"),
                                                 QString("Scale the dut constant in case tranformers are used (U fraction numerator)"),
                                                 QVariant("1"));
    m_pDutConstantUScaleNum->setSCPIInfo(new cSCPIInfo("CONFIGURATION", QString("%1:DUTCONSTANTUSCALENUM").arg(modNr), "10", key, "0", ""));
    m_pModule->veinModuleParameterHash[key] = m_pDutConstantUScaleNum; // for modules use
    //Match x or x/sqrt(3)
    m_pDutConstantUScaleNum->setValidator(new cRegExValidator("^[1-9][0-9]*(\\/sqrt\\(3\\))?$"));


    m_pDutConstantUScaleDenom = new VfModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                                 key = QString("PAR_DutConstantUScaleDenom"),
                                                 QString("Scale the dut constant in case tranformers are used (U fraction denominator)"),
                                                 QVariant("1"));
    m_pDutConstantUScaleDenom->setSCPIInfo(new cSCPIInfo("CONFIGURATION", QString("%1:DUTCONSTANTUSCALEDENOM").arg(modNr), "10", key, "0", ""));
    m_pModule->veinModuleParameterHash[key] = m_pDutConstantUScaleDenom; // for modules use
    //Match x or x/sqrt(3)
    m_pDutConstantUScaleDenom->setValidator(new cRegExValidator("^[1-9][0-9]*(\\/sqrt\\(3\\))?$"));

    m_pDutConstantIScaleNum = new VfModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                                 key = QString("PAR_DutConstantIScaleNum"),
                                                 QString("Scale the dut constant in case tranformers are used (I fraction numerator)"),
                                                 QVariant("1"));
    m_pDutConstantIScaleNum->setSCPIInfo(new cSCPIInfo("CONFIGURATION", QString("%1:DUTCONSTANTISCALENUM").arg(modNr), "10", key, "0", ""));
    m_pModule->veinModuleParameterHash[key] = m_pDutConstantIScaleNum; // for modules use
    //Match x or x/sqrt(3)
    m_pDutConstantIScaleNum->setValidator(new cRegExValidator("^[1-9][0-9]*$"));


    m_pDutConstantIScaleDenom = new VfModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                                 key = QString("PAR_DutConstantIScaleDenom"),
                                                 QString("Scale the dut constant in case tranformers are used (I fraction denominator)"),
                                                 QVariant("1"));
    m_pDutConstantIScaleDenom->setSCPIInfo(new cSCPIInfo("CONFIGURATION", QString("%1:DUTCONSTANTISCALEDENOM").arg(modNr), "10", key, "0", ""));
    m_pModule->veinModuleParameterHash[key] = m_pDutConstantIScaleDenom; // for modules use
    //Match x or x/sqrt(3)
    m_pDutConstantIScaleDenom->setValidator(new cRegExValidator("^[1-9][0-9]*?$"));


    m_pDutTypeMeasurePoint = new VfModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                                 key = QString("PAR_DutTypeMeasurePoint"),
                                                 QString("Scale the dut constant in case tranformers are used (Measurement point selection)"),
                                                 QVariant("CsIsUs"));
    m_pDutTypeMeasurePoint->setSCPIInfo(new cSCPIInfo("CONFIGURATION", QString("%1:DUTCONSTANTMEASUREPOINT").arg(modNr), "10", key, "0", ""));
    m_pModule->veinModuleParameterHash[key] = m_pDutTypeMeasurePoint; // for modules use
    //Match x or x/sqrt(3)
    m_pDutTypeMeasurePoint->setValidator(new cStringValidator({"CpIpUp","CpIsUs","CsIpUp","CsIsUs"}));



    m_pDutConstantUnitPar = new VfModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                                     key = QString("PAR_DUTConstUnit"),
                                                     QString("REF constant unit"),
                                                     QVariant(s = "Unknown"));
    m_pDutConstantUnitPar->setSCPIInfo(new cSCPIInfo("CALCULATE", QString("%1:DCUNIT").arg(modNr), "10", m_pDutConstantUnitPar->getName(), "0", ""));
    m_pModule->veinModuleParameterHash[key] = m_pDutConstantUnitPar; // for modules use

    m_pMRatePar = new VfModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                           key = QString("PAR_MRate"),
                                           QString("Pulses to measure"),
                                           QVariant((double)0.0));
    m_pMRatePar->setSCPIInfo(new cSCPIInfo("CALCULATE", QString("%1:MRATE").arg(modNr), "10", m_pMRatePar->getName(), "0", ""));
    m_pModule->veinModuleParameterHash[key] = m_pMRatePar; // for modules use
    cIntValidator *iValidator;
    iValidator = new cIntValidator(1, 4294967295, 1);
    m_pMRatePar->setValidator(iValidator);

    m_pTargetPar = new VfModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                            key = QString("PAR_Target"),
                                            QString("REF pulses for 0% error - set automatically"),
                                            QVariant((double)0.0));
    m_pTargetPar->setSCPIInfo(new cSCPIInfo("CALCULATE",QString("%1:TARGET").arg(modNr), "10", m_pTargetPar->getName(), "0", ""));
    m_pModule->veinModuleParameterHash[key] = m_pTargetPar; // for modules use
    iValidator = new cIntValidator(1, 4294967295, 1);
    m_pTargetPar->setValidator(iValidator);

    m_pEnergyPar = new VfModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                            key = QString("PAR_Energy"),
                                            QString("Energy value"),
                                            QVariant((double)0.0));
    m_pEnergyPar->setSCPIInfo(new cSCPIInfo("CALCULATE",  QString("%1:PARENERGY").arg(modNr), "10", m_pEnergyPar->getName(), "0", ""));
    m_pModule->veinModuleParameterHash[key] = m_pEnergyPar; // for modules use
    dValidator = new cDoubleValidator(0.0, 1.0e7, 1e-5);
    m_pEnergyPar->setValidator(dValidator);

    m_pStartStopPar = new VfModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                               key = QString("PAR_StartStop"),
                                               QString("Start/stop measurement"),
                                               QVariant((int)0));
    m_pStartStopPar->setSCPIInfo(new cSCPIInfo("CALCULATE", QString("%1:START").arg(modNr), "10", m_pStartStopPar->getName(), "0", ""));
    m_pModule->veinModuleParameterHash[key] =  m_pStartStopPar; // for modules use
    iValidator = new cIntValidator(0, 1, 1);
    m_pStartStopPar->setValidator(iValidator);

    m_pContinuousPar = new VfModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                                key = QString("PAR_Continuous"),
                                                QString("Enables/disable continuous measurement"),
                                                QVariant(int(0)));
    m_pContinuousPar->setSCPIInfo(new cSCPIInfo("CALCULATE", QString("%1:CONTINUOUS").arg(modNr), "10", m_pContinuousPar->getName(), "0", ""));
    m_pModule->veinModuleParameterHash[key] =  m_pContinuousPar; // for modules use
    iValidator = new cIntValidator(0, 1, 1);
    m_pContinuousPar->setValidator(iValidator);
    m_pMeasCountPar = new VfModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                           key = QString("PAR_MeasCount"),
                                           QString("Multiple measurements: Number of measurements"),
                                           QVariant((int)1));
    m_pMeasCountPar->setSCPIInfo(new cSCPIInfo("CALCULATE", QString("%1:MCOUNT").arg(modNr), "10", m_pMeasCountPar->getName(), "0", ""));
    m_pModule->veinModuleParameterHash[key] = m_pMeasCountPar; // for modules use
    iValidator = new cIntValidator(1, m_nMulMeasStoredMax, 1);
    m_pMeasCountPar->setValidator(iValidator);

    m_pMeasWait = new VfModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                           key = QString("PAR_MeasWait"),
                                           QString("Multiple measurements: Seconds to wait between measurements"),
                                           QVariant((int)0));
    m_pMeasWait->setSCPIInfo(new cSCPIInfo("CALCULATE", QString("%1:MWAIT").arg(modNr), "10", m_pMeasWait->getName(), "0", ""));
    m_pModule->veinModuleParameterHash[key] = m_pMeasWait; // for modules use
    iValidator = new cIntValidator(0, std::numeric_limits<int>::max()/1000 /* ms */, 1);
    m_pMeasWait->setValidator(iValidator);
    m_pMeasWait->setUnit("s");

    m_pClientNotifierPar = new VfModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                           key = QString("PAR_ClientActiveNotify"),
                                           QString("By changing this component, a client asks us for max actualize performance"),
                                           QVariant((quint32)0));
    m_pModule->veinModuleParameterHash[key] = m_pClientNotifierPar; // for modules use
    iValidator = new cIntValidator(0, std::numeric_limits<quint32>::max(), 1);
    m_pClientNotifierPar->setValidator(iValidator);
    m_ClientActiveNotifier.init(m_pClientNotifierPar);
    connect(&m_ClientActiveNotifier, &ClientActiveComponent::clientActiveStateChanged, this, &cSec1ModuleMeasProgram::clientActivationChanged);

    // after configuration we still have to set the string validators

    m_pStatusAct = new VfModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                            key = QString("ACT_Status"),
                                            QString("Status: 0:Idle 1:Waiting for first pulse 2:Started 4:Ready 8:Aborted"),
                                            QVariant((int)0) );
    m_pModule->veinModuleParameterHash[key] =  m_pStatusAct; // for modules use
    m_pStatusAct->setSCPIInfo(new cSCPIInfo("CALCULATE",  QString("%1:STATUS").arg(modNr), "2", m_pStatusAct->getName(), "0", ""));

    m_pProgressAct = new VfModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                              key = QString("ACT_Progress"),
                                              QString("Progress information"),
                                              QVariant((double) 0.0));
    m_pModule->veinModuleParameterHash[key] = m_pProgressAct; // and for the modules interface
    m_pProgressAct->setSCPIInfo(new cSCPIInfo("CALCULATE", QString("%1:PROGRESS").arg(modNr), "2", m_pProgressAct->getName(), "0", ""));

    m_pEnergyAct = new VfModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                            key = QString("ACT_Energy"),
                                            QString("Energy since last start"),
                                            QVariant((double) 0.0));
    m_pModule->veinModuleParameterHash[key] = m_pEnergyAct; // and for the modules interface
    m_pEnergyAct->setSCPIInfo(new cSCPIInfo("CALCULATE", QString("%1:ENERGY").arg(modNr), "2", m_pEnergyAct->getName(), "0", ""));


    m_pEnergyFinalAct = new VfModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                                 key = QString("ACT_EnergyFinal"),
                                                 QString("Energy of last measurement"),
                                                 QVariant((double) 0.0));
    m_pModule->veinModuleParameterHash[key] = m_pEnergyFinalAct; // and for the modules interface
    m_pEnergyFinalAct->setSCPIInfo(new cSCPIInfo("CALCULATE", QString("%1:ENFINAL").arg(modNr), "2", m_pEnergyFinalAct->getName(), "0", ""));

    m_pResultAct = new VfModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                            key = QString("ACT_Result"),
                                            QString("Result of last measurement"),
                                            QVariant((double) 0.0));
    m_pModule->veinModuleParameterHash[key] = m_pResultAct; // and for the modules interface
    m_pResultAct->setSCPIInfo(new cSCPIInfo("CALCULATE",  QString("%1:RESULT").arg(modNr), "2", m_pResultAct->getName(), "0", ""));

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

    m_pResultUnit = new VfModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                              key = QString("PAR_ResultUnit"),
                                              QString("Result unit"),
                                              QVariant(s = "%"));
    m_pResultUnit->setSCPIInfo(new cSCPIInfo("CALCULATE", QString("%1:RESULTUNIT").arg(modNr), "10", m_pResultUnit->getName(), "0", ""));
    m_pModule->veinModuleParameterHash[key] = m_pResultUnit; // for modules use
    m_pResultUnit->setValidator(new cStringValidator((QStringList() << "%" << "ppm" )));

    m_pRatingAct = new VfModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                               key = QString("ACT_Rating"),
                                               QString("Evaluation last measurement"),
                                               QVariant((int) -1));
    m_pModule->veinModuleParameterHash[key] = m_pRatingAct; // and for the modules interface
    m_pRatingAct->setSCPIInfo(new cSCPIInfo("CALCULATE",  QString("%1:RATING").arg(modNr), "2", m_pRatingAct->getName(), "0", ""));

    m_pMeasNumAct = new VfModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                               key = QString("ACT_MeasNum"),
                                               QString("Multiple measurements: Count"),
                                               QVariant((int) 0));
    m_pModule->veinModuleParameterHash[key] = m_pMeasNumAct; // and for the modules interface
    m_pMeasNumAct->setSCPIInfo(new cSCPIInfo("CALCULATE",  QString("%1:MEASNUM").arg(modNr), "2", m_pMeasNumAct->getName(), "0", ""));

    m_pMulCountAct = new VfModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                               key = QString("ACT_MulCount"),
                                               QString("Multiple measurements: Count stored"),
                                               QVariant((int) 0));
    m_pModule->veinModuleParameterHash[key] = m_pMulCountAct; // and for the modules interface
    m_pMulCountAct->setSCPIInfo(new cSCPIInfo("CALCULATE",  QString("%1:MULCOUNT").arg(modNr), "2", m_pMulCountAct->getName(), "0", ""));

    m_pMulResultArray = new VfModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                               key = QString("ACT_MulResult"),
                                               QString("Multiple measurements: JSON result array / statistics"),
                                               QVariant(multiResultToJson()));
    m_pModule->veinModuleParameterHash[key] = m_pMulResultArray; // and for the modules interface
    m_pMulResultArray->setSCPIInfo(new cSCPIInfo("CALCULATE",  QString("%1:STJARRAY").arg(modNr), "2", m_pMulResultArray->getName(), "0", ""));
}


void cSec1ModuleMeasProgram::catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer)
{
    bool ok;

    if (msgnr == 0) // 0 was reserved for async. messages
    {
        QString sintnr;
        sintnr = answer.toString().section(':', 1, 1);
        int service = sintnr.toInt(&ok);
        switch (service)
        {
        case irqPCBNotifier:
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
                    notifyActivationError(tr(rmidentErrMSG));
                break;

            case testsec1resource:
                if ((reply == ack) && (answer.toString().contains("SEC1")))
                    emit activationContinue();
                else
                    notifyActivationError(tr(resourcetypeErrMsg));
                break;

            case setecresource:
                if (reply == ack)
                    emit activationContinue();
                else
                    notifyActivationError(tr(setresourceErrMsg));
                break;

            case readresourcetypes:
                if (reply == ack) {
                    m_ResourceTypeList = answer.toString().split(';');
                    emit activationContinue();
                }
                else
                    notifyActivationError(tr(resourcetypeErrMsg));
                break;

            case readresource:
            {
                if (reply == ack) {
                    m_ResourceHash[m_ResourceTypeList.at(m_nIt)] = answer.toString();
                    m_nIt++;
                    if (m_nIt < m_ResourceTypeList.count())
                        emit activationLoop();
                    else
                        emit activationContinue();
                }
                else
                    notifyActivationError(tr(resourceErrMsg));
                break;
            }

            case fetchecalcunits:
            {
                QStringList sl;
                sl = answer.toString().split(';');
                if ((reply == ack) && (sl.length() >= 2)) {
                    m_MasterEcalculator.name = sl.at(0);
                    m_MasterEcalculator.secIFace = m_pSECInterface;
                    m_MasterEcalculator.secServersocket = getConfData()->m_SECServerSocket;
                    m_SlaveEcalculator.name = sl.at(1);
                    m_SlaveEcalculator.secIFace = m_pSECInterface;
                    m_SlaveEcalculator.secServersocket = getConfData()->m_SECServerSocket;
                    emit activationContinue();
                }
                else
                    notifyActivationError(tr(fetchsececalcunitErrMsg));
                break;
            }

            case readrefInputalias:
            {
                if (reply == ack) {
                    siInfo->alias = answer.toString();
                    emit activationContinue();
                }
                else
                    notifyActivationError(tr(readaliasErrMsg));
                break;
            }

            case readdutInputalias:
            {
                if (reply == ack) {
                    siInfo->alias = answer.toString();
                    emit activationContinue();
                }
                else
                    notifyActivationError(tr(readaliasErrMsg));
                break;
            }

            case freeecalcunits:
                if (reply == ack) // we only continue if sec server manager acknowledges
                    emit deactivationContinue();
                else
                    notifyDeactivationError(tr(freesececalcunitErrMsg));
                break;

            case freeecresource:
                if (reply == ack) // we only continue if resource manager acknowledges
                    emit deactivationContinue();
                else
                    notifyDeactivationError(tr(freeresourceErrMsg));
                break;

            case actualizeprogress:
            {
                if (reply == ack) {
                    // Still running and not waiting for next?
                    if(m_bMeasurementRunning && (m_nStatus & ECALCSTATUS::WAIT) == 0) {
                        m_nDUTPulseCounterActual = answer.toUInt(&ok);
                        m_fProgress = ((1.0 * m_nDUTPulseCounterStart - 1.0 * m_nDUTPulseCounterActual)/ m_nDUTPulseCounterStart)*100.0;
                        if (m_fProgress > 100.0) {
                            m_fProgress = 100.0;
                        }
                        if(m_fProgress < 0.0) {
                            m_fProgress = 0.0;
                        }
                        m_pProgressAct->setValue(QVariant(m_fProgress));
                    }
                }
                else
                    notifyExecutionError(readsecregisterErrMsg);
                break;
            }

            case actualizeenergy:
            {
                if (reply == ack) {
                    // Still running and not waiting for next?
                    if(m_bMeasurementRunning && (m_nStatus & ECALCSTATUS::WAIT) == 0) {
                        m_nEnergyCounterActual = answer.toUInt(&ok);
                        m_fEnergy = m_nEnergyCounterActual / getConfData()->m_fRefConstant.m_fPar;
                        m_pEnergyAct->setValue(m_fEnergy);
                        if (m_bFirstMeas) {
                            // keep in final until a result is calculated in
                            m_pEnergyFinalAct->setValue(m_fEnergy);
                        }
                    }
                }
                else
                    notifyExecutionError(readsecregisterErrMsg);
                break;
            }


            case actualizestatus:
            {
                if (reply == ack)
                {
                    // Still running and not waiting for next?
                    if(m_bMeasurementRunning && (m_nStatus & ECALCSTATUS::WAIT) == 0) {
                        // once ready we leave status ready (continous mode)
                        m_nStatus = (m_nStatus & ECALCSTATUS::READY) | (answer.toUInt(&ok) & 7);
                        m_pStatusAct->setValue(QVariant(m_nStatus));
                    }
                }
                else
                    notifyExecutionError(readsecregisterErrMsg);
                break;
            }

            case setsync:
                if (reply == ack)
                    emit setupContinue();
                else
                    notifyExecutionError(setsyncErrMsg);
                break;

            case enableinterrupt:
            case setmeaspulses:
                if (reply == ack)
                    emit setupContinue();
                else
                    notifyExecutionError(writesecregisterErrMsg);
                break;

            case setmastermux:
            case setslavemux:
                if (reply == ack)
                    emit setupContinue();
                else
                    notifyExecutionError(setmuxErrMsg);
                break;

            case setmastermeasmode:
            case setslavemeasmode:
                if (reply == ack)
                {
                    emit setupContinue();
                }
                else
                    notifyExecutionError(setcmdidErrMsg);
                break;

            case startmeasurement:
                if (reply == ack)
                {
                    emit setupContinue();
                }
                else
                    notifyExecutionError(startmeasErrMsg);
                break;

            case stopmeas:
                if (reply == ack) {
                    emit deactivationContinue();
                }
                else
                    notifyExecutionError(stopmeasErrMsg);
                break;

            case setpcbrefconstantnotifier:
                if (reply == ack) // we only continue if pcb server acknowledges
                    emit activationContinue();
                else
                    notifyActivationError(registerpcbnotifierErrMsg);
                break;

            case setsecintnotifier:
                if (reply == ack) // we only continue if sec server acknowledges
                    emit activationContinue();
                else
                    notifyActivationError(registerpcbnotifierErrMsg);
                break;


            case fetchrefconstant:
            {
                if (reply == ack)  {// we only continue if sec server acknowledges
                    double constant;
                    constant = answer.toDouble(&ok);
                    m_pRefConstantPar->setValue(QVariant(constant));
                    newRefConstant(QVariant(constant));
                }
                else
                    notifyExecutionError(readrefconstantErrMsg);
                break;
            }

            case readintregister:
                if (reply == ack) // we only continue if sec server acknowledges
                {
                    m_nIntReg = answer.toInt(&ok) & 7;
                    emit interruptContinue();
                }
                else
                    notifyExecutionError(readsecregisterErrMsg);
                break;
            case resetintregister:
                if (reply == ack) // we only continue if sec server acknowledges
                    emit interruptContinue();
                else
                    notifyExecutionError(writesecregisterErrMsg);
                break;
            case readvicount:
                if (reply == ack) { // we only continue if sec server acknowledges
                    // On high frequency continuous measurements chances are high
                    // that an abort comes in while we fetch final energy counter.
                    // All aborts (user / change ranges) stop measurement and that
                    // latches incomplete counter values.
                    // We have an abort check in setECResultAndResetInt but that is
                    // not enough. Test case:
                    // * Run high frequency continous measurement
                    // * Stop it (all OK up here)
                    // * Change DUT constant/unit -> Crap results
                    if(m_bMeasurementRunning) {
                        m_nEnergyCounterFinal = answer.toLongLong(&ok);
                    }
                    emit interruptContinue();
                }
                else
                    notifyExecutionError(readsecregisterErrMsg);
                break;

            }
        }
    }
}

cSec1ModuleConfigData *cSec1ModuleMeasProgram::getConfData()
{
    return qobject_cast<cSec1ModuleConfiguration*>(m_pConfiguration.get())->getConfigurationData();
}


void cSec1ModuleMeasProgram::setInterfaceComponents()
{
    cmpDependencies(); // dependant on mode we calculate parameters by ourself

    m_pDutInputPar->setValue(QVariant(mDUTSecInputInfoHash[getConfData()->m_sDutInput.m_sPar]->alias));
    m_pRefInputPar->setValue(QVariant(getRefInputDisplayString(getConfData()->m_sRefInput.m_sPar)));
    m_pDutConstantPar->setValue(QVariant(getConfData()->m_fDutConstant.m_fPar));
    m_pRefConstantPar->setValue(QVariant(getConfData()->m_fRefConstant.m_fPar));
    m_pMRatePar->setValue(QVariant(getConfData()->m_nMRate.m_nPar));
    m_pTargetPar->setValue(QVariant(getConfData()->m_nTarget.m_nPar));
    m_pEnergyPar->setValue(QVariant(getConfData()->m_fEnergy.m_fPar));
    m_pProgressAct->setValue(QVariant(double(0.0)));
    m_pUpperLimitPar->setValue(QVariant(getConfData()->m_fUpperLimit.m_fPar));
    m_pLowerLimitPar->setValue(QVariant(getConfData()->m_fLowerLimit.m_fPar));
    m_pResultUnit->setValue(QVariant(getConfData()->m_sResultUnit.m_sPar));
}


void cSec1ModuleMeasProgram::setValidators()
{
    cStringValidator *sValidator = new cStringValidator(m_DUTAliasList);
    m_pDutInputPar->setValidator(sValidator);

    sValidator = new cStringValidator(m_REFAliasList);
    m_pRefInputPar->setValidator(sValidator);

    m_pDutConstanstUnitValidator = new cStringValidator(getDutConstUnitValidator());
    m_pDutConstantUnitPar->setValidator(m_pDutConstanstUnitValidator);

    QString s = getEnergyUnit();
    m_pEnergyAct->setUnit(s);
    m_pEnergyFinalAct->setUnit(s);
    m_pEnergyPar->setUnit(s);
}


QStringList cSec1ModuleMeasProgram::getDutConstUnitValidator()
{
    QStringList sl;
    QString powType = mREFSecInputInfoHash[getConfData()->m_sRefInput.m_sPar]->alias;
    if (powType.contains('P'))
        sl << QString("I/kWh") << QString("Wh/I");
    if (powType.contains('Q'))
        sl << QString("I/kVarh") << QString("Varh/I");
    if (powType.contains('S'))
        sl << QString("I/kVAh") << QString("VAh/I");
    return sl;
}


QString cSec1ModuleMeasProgram::getEnergyUnit()
{
    QString powerType = mREFSecInputInfoHash[getConfData()->m_sRefInput.m_sPar]->alias;

    return cUnitHelper::getNewEnergyUnit(powerType, QString('k'), 3600);
}


void cSec1ModuleMeasProgram::initDutConstantUnit(QStringList sl)
{
    QString lastDutUnit = getConfData()->m_sDutConstantUnit.m_sPar;
    if(lastDutUnit.isEmpty() || !sl.contains(lastDutUnit)) {
        m_sDutConstantUnit = sl.at(0);
    }
    else {
        m_sDutConstantUnit = lastDutUnit;
    }
    m_pDutConstantUnitPar->setValue(m_sDutConstantUnit);
}


void cSec1ModuleMeasProgram::initDutConstantUnit()
{
    initDutConstantUnit(getDutConstUnitValidator());
}


void cSec1ModuleMeasProgram::handleChangedREFConst()
{
    // we ask for the reference constant of the selected Input
    m_MsgNrCmdList[m_pPCBInterface->getConstantSource(getConfData()->m_sRefInput.m_sPar)] = fetchrefconstant;
    stopMeasurement(true);
}


void cSec1ModuleMeasProgram::handleSECInterrupt()
{
    if (!m_InterrupthandlingStateMachine.isRunning())
        m_InterrupthandlingStateMachine.start();
}


void cSec1ModuleMeasProgram::cmpDependencies()
{
    QString mode;
    double constant;
    bool energyPerImpulse;

    mode = getConfData()->m_sMode.m_sPar;

    constant = getConfData()->m_fDutConstant.m_fPar; // assumed I/kxxx because all computation is based on this
    energyPerImpulse = m_sDutConstantUnit.contains(QString("/I"));

    if (energyPerImpulse)
        constant = (1.0/constant) * 1000.0; // if xxx/I we calculate in I/kxxx

    // We can be sure DUT constant is in I/kxxx here. Therefor the constant is scaled here in case
    // an instrument transformer is used.
    constant=constant*m_dutConstantScalingMem;

    if (mode == "mrate")
    {
       // we calculate the new target value
       getConfData()->m_nTarget.m_nPar = floor(getConfData()->m_nMRate.m_nPar * getConfData()->m_fRefConstant.m_fPar / constant);
       getConfData()->m_fEnergy.m_fPar = getConfData()->m_nMRate.m_nPar / constant;
    }

    else

    if (mode == "energy")
    {
        // we calcute the new mrate and target
        getConfData()->m_nMRate.m_nPar = ceil(constant * getConfData()->m_fEnergy.m_fPar);
        getConfData()->m_nTarget.m_nPar = floor(getConfData()->m_nMRate.m_nPar * getConfData()->m_fRefConstant.m_fPar / constant);
    }

    else

    if (mode == "target")
    {
        constant = getConfData()->m_nMRate.m_nPar * getConfData()->m_fRefConstant.m_fPar / getConfData()->m_nTarget.m_nPar;
        if (energyPerImpulse)
            constant = (1.0/constant) * 1000.0;
        getConfData()->m_fDutConstant.m_fPar = constant;
        getConfData()->m_fEnergy.m_fPar = getConfData()->m_nMRate.m_nPar / getConfData()->m_fDutConstant.m_fPar;
    }
}

void cSec1ModuleMeasProgram::startNext()
{
    // Notes on re-start:
    // * We don't need the whole start state machine here
    // * There is too much magic in startMeasurement so we cannnot use it here either
    m_MsgNrCmdList[m_pSECInterface->stop(m_MasterEcalculator.name)] = stopmeas;
    m_nStatus = ECALCSTATUS::ARMED | ECALCSTATUS::READY;
    m_MsgNrCmdList[m_pSECInterface->start(m_MasterEcalculator.name)] = startmeasurement;
    startMeasurementDone();
}

const QString cSec1ModuleMeasProgram::multiResultToJson()
{
    QJsonObject jsonObject = m_multipleResultHelper.getJSONStatistics();
    jsonObject.insert("limits", m_multipleResultHelper.getJSONLimitsArray());
    jsonObject.insert("values", m_multipleResultHelper.getJSONResultArray());
    QJsonDocument jsonDoc(jsonObject);
    return QString::fromUtf8(jsonDoc.toJson(QJsonDocument::Compact));
}

void cSec1ModuleMeasProgram::multiResultToVein()
{
    m_pMulResultArray->setValue(multiResultToJson());
    m_pMulCountAct->setValue(m_multipleResultHelper.getCountTotal());
}

double cSec1ModuleMeasProgram::getUnitFactor()
{
    double factor = 100.0;
    if(m_pResultUnit->getValue() == QStringLiteral("ppm")) {
        factor = 1e6;
    }
    return factor;
}

QString cSec1ModuleMeasProgram::getRefInputDisplayString(QString inputName)
{
    QString displayString = mREFSecInputInfoHash[inputName]->alias;
    QList<cSec1ModuleConfigData::TRefInput> refInputList = getConfData()->m_refInpList;
    for(const auto &entry : refInputList) {
        if(entry.inputName == inputName) {
            displayString += entry.nameAppend;
            break;
        }
    }
    return displayString;
}

void cSec1ModuleMeasProgram::resourceManagerConnect()
{
    // first we try to get a connection to resource manager over proxy
    m_rmClient = Zera::Proxy::getInstance()->getConnectionSmart(getConfData()->m_RMSocket.m_sIP, getConfData()->m_RMSocket.m_nPort);
    // and then we set connection resource manager interface's connection
    m_rmInterface.setClientSmart(m_rmClient);
    resourceManagerConnectState.addTransition(m_rmClient.get(), &Zera::ProxyClient::connected, &m_IdentifyState);
    connect(&m_rmInterface, &Zera::cRMInterface::serverAnswer, this, &cSec1ModuleMeasProgram::catchInterfaceAnswer);
    // todo insert timer for timeout and/or connect error conditions
    Zera::Proxy::getInstance()->startConnectionSmart(m_rmClient);
}


void cSec1ModuleMeasProgram::sendRMIdent()
{
    m_MsgNrCmdList[m_rmInterface.rmIdent(QString("Sec1Module%1").arg(m_pModule->getModuleNr()))] = sendrmident;
}


void cSec1ModuleMeasProgram::testSEC1Resource()
{
    m_MsgNrCmdList[m_rmInterface.getResourceTypes()] = testsec1resource;
}


void cSec1ModuleMeasProgram::setECResource()
{
    m_MsgNrCmdList[m_rmInterface.setResource("SEC1", "ECALCULATOR", 2)] = setecresource;
}



void cSec1ModuleMeasProgram::readResourceTypes()
{
    //m_MsgNrCmdList[m_rmInterface.getResourceTypes()] = readresourcetypes;
    // instead of taking all resourcetypes we take predefined types if we found them in our config

    if (found(getConfData()->m_dutInpList, "fi") || found(getConfData()->m_refInpList, "fi"))
        m_ResourceTypeList.append("FRQINPUT");
    if (found(getConfData()->m_dutInpList, "fo") || found(getConfData()->m_refInpList, "fo"))
        m_ResourceTypeList.append("SOURCE");
    if (found(getConfData()->m_dutInpList, "sh") || found(getConfData()->m_refInpList, "sh"))
        m_ResourceTypeList.append("SCHEAD");
    if (found(getConfData()->m_dutInpList, "hk") || found(getConfData()->m_refInpList, "hk"))
        m_ResourceTypeList.append("HKEY");

    emit activationContinue();
}


void cSec1ModuleMeasProgram::readResources()
{
    m_nIt = 0; // we want to read all resources from resourcetypelist
    emit activationContinue();
}


void cSec1ModuleMeasProgram::readResource()
{
    QString resourcetype = m_ResourceTypeList.at(m_nIt);
    m_MsgNrCmdList[m_rmInterface.getResources(resourcetype)] = readresource;
}


void cSec1ModuleMeasProgram::testSecInputs()
{
    qint32 referenceInputCount = getConfData()->m_refInpList.count();
    // first we build up a list with properties for all configured Inputs
    for (int i = 0; i < referenceInputCount; i++) {
        siInfo = new cSecInputInfo();
        mREFSecInputInfoHash[getConfData()->m_refInpList.at(i).inputName] = siInfo;
    }

    QList<QString> InputNameList = mREFSecInputInfoHash.keys();
    while (InputNameList.count() > 0) {
        QString name = InputNameList.takeFirst();
        for (int i = 0; i < m_ResourceTypeList.count(); i++) {
            QString resourcelist = m_ResourceHash[m_ResourceTypeList.at(i)];
            if (resourcelist.contains(name)) {
                referenceInputCount--;
                siInfo = mREFSecInputInfoHash.take(name);
                siInfo->name = name;
                siInfo->resource = m_ResourceTypeList.at(i);
                mREFSecInputInfoHash[name] = siInfo;
                break;
            }
        }
    }
    for (int i = 0; i < getConfData()->m_dutInpList.count(); i++) {
        siInfo = new cSecInputInfo();
        mDUTSecInputInfoHash[getConfData()->m_dutInpList.at(i)] = siInfo;
    }

    qint32 dutInputCount = mDUTSecInputInfoHash.count(); // we have n configured dut Inputs
    InputNameList = mDUTSecInputInfoHash.keys();
    while (InputNameList.count() > 0) {
        QString name = InputNameList.takeFirst();
        for (int i = 0; i < m_ResourceTypeList.count(); i++) {
            QString resourcelist = m_ResourceHash[m_ResourceTypeList.at(i)];
            if (resourcelist.contains(name)) {
                dutInputCount--;
                siInfo = mDUTSecInputInfoHash.take(name);
                siInfo->name = name;
                siInfo->resource = m_ResourceTypeList.at(i);
                mDUTSecInputInfoHash[name] = siInfo;
                break;
            }
        }
    }

    if ((referenceInputCount == 0) && (dutInputCount == 0)) // we found all our configured Inputs
        emit activationContinue(); // so lets go on
    else {
        emit errMsg((tr(resourceErrMsg)));
        emit activationError();
    }
}


void cSec1ModuleMeasProgram::ecalcServerConnect()
{
    // we try to get a connection to ecalc server over proxy
    m_pSECClient = Zera::Proxy::getInstance()->getConnection(getConfData()->m_SECServerSocket.m_sIP, getConfData()->m_SECServerSocket.m_nPort);
    // and then we set ecalcalculator interface's connection
    m_pSECInterface->setClient(m_pSECClient); //
    m_ecalcServerConnectState.addTransition(m_pSECClient, &Zera::ProxyClient::connected, &m_fetchECalcUnitsState);
    connect(m_pSECInterface, &Zera::cSECInterface::serverAnswer, this, &cSec1ModuleMeasProgram::catchInterfaceAnswer);
    // todo insert timer for timeout and/or connect error conditions
    Zera::Proxy::getInstance()->startConnection(m_pSECClient);
}


void cSec1ModuleMeasProgram::fetchECalcUnits()
{
    m_MsgNrCmdList[m_pSECInterface->setECalcUnit(2)] = fetchecalcunits; // we need 2 ecalc units to cascade
}


void cSec1ModuleMeasProgram::pcbServerConnect()
{
    // we try to get a connection to ecalc server over proxy
    m_pPCBClient = Zera::Proxy::getInstance()->getConnection(getConfData()->m_PCBServerSocket.m_sIP, getConfData()->m_PCBServerSocket.m_nPort);
    // and then we set ecalcalculator interface's connection
    m_pPCBInterface->setClient(m_pPCBClient); //
    m_pcbServerConnectState.addTransition(m_pPCBClient, &Zera::ProxyClient::connected, &m_readREFInputsState);
    connect(m_pPCBInterface, &Zera::cPCBInterface::serverAnswer, this, &cSec1ModuleMeasProgram::catchInterfaceAnswer);
    // todo insert timer for timeout and/or connect error conditions
    Zera::Proxy::getInstance()->startConnection(m_pPCBClient);
}


void cSec1ModuleMeasProgram::readREFInputs()
{
    m_sItList = mREFSecInputInfoHash.keys();
    emit activationContinue();
}


void cSec1ModuleMeasProgram::readREFInputAlias()
{
    m_sIt = m_sItList.takeFirst();
    siInfo = mREFSecInputInfoHash.take(m_sIt); // if set some info that could be useful later
    siInfo->pcbIFace = m_pPCBInterface; // in case that Inputs would be provided by several servers
    siInfo->pcbServersocket = getConfData()->m_PCBServerSocket;
    //m_MsgNrCmdList[siInfo->pcbIFace->resourceAliasQuery(siInfo->resource, m_sIt)] = readrefInputalias;

    // we will read the powertype of the reference frequency input and will use this as our alias ! for example P, +P ....
    m_MsgNrCmdList[siInfo->pcbIFace->getPowTypeSource(m_sIt)] = readrefInputalias;

}


void cSec1ModuleMeasProgram::readREFInputDone()
{
    mREFSecInputInfoHash[siInfo->name] = siInfo;
    if (m_sItList.isEmpty())
        emit activationContinue();
    else
        emit activationLoop();
}


void cSec1ModuleMeasProgram::readDUTInputs()
{
    m_sItList = mDUTSecInputInfoHash.keys();
    emit activationContinue();
}


void cSec1ModuleMeasProgram::readDUTInputAlias()
{
    m_sIt = m_sItList.takeFirst();
    siInfo = mDUTSecInputInfoHash.take(m_sIt); // if set some info that could be useful later
    siInfo->pcbIFace = m_pPCBInterface; // in case that Inputs would be provided by several servers
    siInfo->pcbServersocket = getConfData()->m_PCBServerSocket;
    m_MsgNrCmdList[siInfo->pcbIFace->resourceAliasQuery(siInfo->resource, m_sIt)] = readdutInputalias;
}


void cSec1ModuleMeasProgram::readDUTInputDone()
{
    mDUTSecInputInfoHash[siInfo->name] = siInfo;
    if (m_sItList.isEmpty())
        emit activationContinue();
    else
        emit activationLoop();
}


void cSec1ModuleMeasProgram::setpcbREFConstantNotifier()
{
    if ( (getConfData()->m_nRefInpCount > 0) && getConfData()->m_bEmbedded ) // if we have some ref. Input and are embedded in meter we register for notification
    {
        m_MsgNrCmdList[m_pPCBInterface->registerNotifier(QString("SOURCE:%1:CONSTANT?").arg(getConfData()->m_sRefInput.m_sPar), irqPCBNotifier)] = setpcbrefconstantnotifier;
        // todo also configure the query for setting this notifier .....very flexible
    }
    else
        emit activationContinue(); // if no ref constant notifier (standalone error calc) we directly go on
}


void cSec1ModuleMeasProgram::setsecINTNotifier()
{
    m_MsgNrCmdList[m_pSECInterface->registerNotifier(QString("ECAL:%1:R%2?").arg(m_MasterEcalculator.name).arg(ECALCREG::INTREG))] = setsecintnotifier;
}


void cSec1ModuleMeasProgram::activationDone()
{
    cSec1ModuleConfigData *confData = getConfData();
    int nref = confData->m_refInpList.count();
    if (nref > 0)
    for (int i = 0; i < nref; i++) {
        QString displayString = getRefInputDisplayString(confData->m_refInpList.at(i).inputName);
        m_REFAliasList.append(displayString); // build up a fixed sorted list of alias
        siInfo = mREFSecInputInfoHash[confData->m_refInpList.at(i).inputName]; // change the hash for access via alias
        mREFSecInputSelectionHash[displayString] = siInfo;
    }

    for (int i = 0; i < confData->m_dutInpList.count(); i++)
    {
        m_DUTAliasList.append(mDUTSecInputInfoHash[confData->m_dutInpList.at(i)]->alias); // build up a fixed sorted list of alias
        siInfo = mDUTSecInputInfoHash[confData->m_dutInpList.at(i)]; // change the hash for access via alias
        mDUTSecInputSelectionHash[siInfo->alias] = siInfo;
    }

    m_ActualizeTimer.setInterval(m_nActualizeIntervallLowFreq);
    connect(&m_ActualizeTimer, &QTimer::timeout, this, &cSec1ModuleMeasProgram::Actualize);
    m_WaitMultiTimer.setSingleShot(true);
    connect(&m_WaitMultiTimer, &QTimer::timeout, this, &cSec1ModuleMeasProgram::startNext);

    connect(m_pStartStopPar, &VfModuleParameter::sigValueChanged, this, &cSec1ModuleMeasProgram::newStartStop);
    connect(m_pDutConstantPar, &VfModuleParameter::sigValueChanged, this, &cSec1ModuleMeasProgram::newDutConstant);
    connect(m_pDutConstantUScaleNum, &VfModuleParameter::sigValueChanged,[this](QVariant val){
        this->newDutConstantScale(val,m_pDutConstantUScaleDenom->getName());
    });
    connect(m_pDutConstantUScaleDenom, &VfModuleParameter::sigValueChanged,[this](QVariant val){

        this->newDutConstantScale(val,m_pDutConstantUScaleDenom->getName());
    });
    connect(m_pDutConstantIScaleNum, &VfModuleParameter::sigValueChanged,[this](QVariant val){
        this->newDutConstantScale(val,m_pDutConstantIScaleNum->getName());
    });
    connect(m_pDutConstantIScaleDenom, &VfModuleParameter::sigValueChanged,[this](QVariant val){

        this->newDutConstantScale(val,m_pDutConstantIScaleDenom->getName());
    });
    connect(m_pDutTypeMeasurePoint, &VfModuleParameter::sigValueChanged,[this](QVariant val){

        this->newDutConstantScale(val,m_pDutTypeMeasurePoint->getName());
    });
    connect(m_pDutConstantUnitPar, &VfModuleParameter::sigValueChanged, this, &cSec1ModuleMeasProgram::newDutConstantUnit);
    connect(m_pRefConstantPar, &VfModuleParameter::sigValueChanged, this, &cSec1ModuleMeasProgram::newRefConstant);
    connect(m_pDutInputPar, &VfModuleParameter::sigValueChanged, this, &cSec1ModuleMeasProgram::newDutInput);
    connect(m_pRefInputPar, &VfModuleParameter::sigValueChanged, this, &cSec1ModuleMeasProgram::newRefInput);
    connect(m_pMRatePar, &VfModuleParameter::sigValueChanged, this, &cSec1ModuleMeasProgram::newMRate);
    connect(m_pTargetPar, &VfModuleParameter::sigValueChanged, this, &cSec1ModuleMeasProgram::newTarget);
    connect(m_pEnergyPar, &VfModuleParameter::sigValueChanged, this, &cSec1ModuleMeasProgram::newEnergy);

    connect(m_pUpperLimitPar, &VfModuleParameter::sigValueChanged, this, &cSec1ModuleMeasProgram::newUpperLimit);
    connect(m_pLowerLimitPar, &VfModuleParameter::sigValueChanged, this, &cSec1ModuleMeasProgram::newLowerLimit);


    initDutConstantUnit();
    setInterfaceComponents(); // actualize interface components

    setValidators();

    // we ask for the reference constant of the selected Input
    m_MsgNrCmdList[m_pPCBInterface->getConstantSource(confData->m_sRefInput.m_sPar)] = fetchrefconstant;

    m_bActive = true;
    emit activated();
}

void cSec1ModuleMeasProgram::stopECCalculator()
{
    stopMeasurement(true);
}


void cSec1ModuleMeasProgram::freeECalculator()
{
    m_bActive = false;
    m_MsgNrCmdList[m_pSECInterface->freeECalcUnits()] = freeecalcunits;
}


void cSec1ModuleMeasProgram::freeECResource()
{
    m_MsgNrCmdList[m_rmInterface.freeResource("SEC1", "ECALCULATOR")] = freeecresource;
}


void cSec1ModuleMeasProgram::deactivationDone()
{
    disconnect(&m_rmInterface, 0, this, 0);
    disconnect(m_pSECInterface, 0, this, 0);
    disconnect(m_pPCBInterface, 0, this, 0);

    disconnect(m_pStartStopPar, 0, this, 0);
    disconnect(m_pDutConstantPar, 0, this, 0);
    disconnect(m_pRefConstantPar, 0, this, 0);
    disconnect(m_pDutInputPar, 0, this, 0);
    disconnect(m_pRefInputPar, 0, this, 0);
    disconnect(m_pMRatePar, 0, this, 0);
    disconnect(m_pTargetPar, 0, this, 0);
    disconnect(m_pEnergyPar, 0, this, 0);

    emit deactivated();
}


void cSec1ModuleMeasProgram::setSync()
{
    m_MsgNrCmdList[m_pSECInterface->setSync(m_SlaveEcalculator.name, m_MasterEcalculator.name)] = setsync;
}


void cSec1ModuleMeasProgram::setMeaspulses()
{
    if (m_pDutInputPar->getValue().toString().contains("HK"))
        m_nDUTPulseCounterStart = 1;
    else
        m_nDUTPulseCounterStart = m_pMRatePar->getValue().toLongLong();
    m_MsgNrCmdList[m_pSECInterface->writeRegister(m_MasterEcalculator.name, ECALCREG::MTCNTin, m_nDUTPulseCounterStart)] = setmeaspulses;
}


void cSec1ModuleMeasProgram::setMasterMux()
{
    m_MsgNrCmdList[m_pSECInterface->setMux(m_MasterEcalculator.name, mDUTSecInputSelectionHash[m_pDutInputPar->getValue().toString()]->name)] = setmastermux;
}


void cSec1ModuleMeasProgram::setSlaveMux()
{
    m_MsgNrCmdList[m_pSECInterface->setMux(m_SlaveEcalculator.name, mREFSecInputSelectionHash[m_pRefInputPar->getValue().toString()]->name)] = setslavemux;
}


void cSec1ModuleMeasProgram::setMasterMeasMode()
{
    if (m_pContinuousPar->getValue().toInt() == 0 || m_pDutInputPar->getValue().toString().contains("HK"))
        m_MsgNrCmdList[m_pSECInterface->setCmdid(m_MasterEcalculator.name, ECALCCMDID::SINGLEERRORMASTER)] = setmastermeasmode;
    else
        m_MsgNrCmdList[m_pSECInterface->setCmdid(m_MasterEcalculator.name, ECALCCMDID::CONTERRORMASTER)] = setmastermeasmode;
}


void cSec1ModuleMeasProgram::setSlaveMeasMode()
{
    m_MsgNrCmdList[m_pSECInterface->setCmdid(m_SlaveEcalculator.name, ECALCCMDID::ERRORMEASSLAVE)] = setslavemeasmode;

}


void cSec1ModuleMeasProgram::enableInterrupt()
{
    m_MsgNrCmdList[m_pSECInterface->writeRegister(m_MasterEcalculator.name, ECALCREG::INTMASK, ECALCINT::MTCount0)] = enableinterrupt;
}


void cSec1ModuleMeasProgram::startMeasurement()
{
    m_nStatus = ECALCSTATUS::ARMED;
    m_fEnergy = 0.0;
    m_pEnergyAct->setValue(m_fEnergy);
    m_pEnergyFinalAct->setValue(m_fEnergy);
    m_fProgress = 0.0;
    m_pProgressAct->setValue(QVariant(m_fProgress));
    m_bMeasurementRunning = true;
    // All preparations done: do start
    m_MsgNrCmdList[m_pSECInterface->start(m_MasterEcalculator.name)] = startmeasurement;
}


void cSec1ModuleMeasProgram::startMeasurementDone() // final state of m_startMeasurementMachine
{
    Actualize(); // we acualize at once after started
    m_ActualizeTimer.start(); // and after current interval
}


void cSec1ModuleMeasProgram::readIntRegister()
{
    m_MsgNrCmdList[m_pSECInterface->readRegister(m_MasterEcalculator.name, ECALCREG::INTREG)] = readintregister;
}


void cSec1ModuleMeasProgram::resetIntRegister()
{
    m_MsgNrCmdList[m_pSECInterface->intAck(m_MasterEcalculator.name, 0xF)] = resetintregister; // we reset all here
}


void cSec1ModuleMeasProgram::readMTCountact()
{
    m_MsgNrCmdList[m_pSECInterface->readRegister(m_SlaveEcalculator.name, ECALCREG::MTCNTfin)] = readvicount;
}


void cSec1ModuleMeasProgram::setECResult()
{
    if (m_nEnergyCounterFinal == 0)
    {
        m_fResult = qQNaN();
        m_eRating = ECALCRESULT::RESULT_UNFINISHED;
    }
    else
    {
        m_fResult = (1.0 * getConfData()->m_nTarget.m_nPar - 1.0 * m_nEnergyCounterFinal) * getUnitFactor() / m_nEnergyCounterFinal;
        setRating();
    }

    m_fEnergy = 1.0 * m_nEnergyCounterFinal / getConfData()->m_fRefConstant.m_fPar;
    m_pResultAct->setValue(QVariant(m_fResult));
    m_pEnergyAct->setValue(m_fEnergy);
    m_pEnergyFinalAct->setValue(m_fEnergy);
}

void cSec1ModuleMeasProgram::setECResultAndResetInt()
{
    // Remember:
    // * this function is called when m_InterrupthandlingStateMachine enters
    //   m_calcResultAndResetIntState.
    // * m_InterrupthandlingStateMachine is started by receipt of async sec
    //   interrupt and runs it's linear states (and should not be aborted).
    //
    // Problem:
    // If an abort is requested (by user or change of ranges) while this machine
    // is running, code below will overwrite m_nStatus and calculate values based on
    // unpredicatble (sec was possibly stopped) crap values.
    //
    // Test case:
    // * Start a continous measurement with high result frequency and abort it either by
    //   requesting abort or changing the ranges
    if(m_bMeasurementRunning) {
        m_bFirstMeas = false;

        if (m_pContinuousPar->getValue().toInt() == 0)
        {
            m_nStatus = ECALCSTATUS::READY;
        }
        else {
            m_nStatus = ECALCSTATUS::READY + ECALCSTATUS::STARTED;
        }
        m_fProgress = 100.0;
        m_pStatusAct->setValue(QVariant(m_nStatus));
        m_pProgressAct->setValue(QVariant(m_fProgress));

        ++m_nMeasurementNo;
        m_pMeasNumAct->setValue(m_nMeasurementNo);
        setECResult();
        if(m_multipleResultHelper.getCountTotal() < m_nMulMeasStoredMax) {
            // TODO: Once we have fancy vf-cpp and want to re-use result array
            // with statistics below, the following should be split into a
            // seperate module/library/?

            // append to our result list
            m_multipleResultHelper.append(m_fResult,
                                          m_eRating,
                                          getConfData()->m_fLowerLimit.m_fPar,
                                          getConfData()->m_fUpperLimit.m_fPar,
                                          getUnitFactor());
            multiResultToVein();
        }
    }

    // enable next int
    resetIntRegister();
}

void cSec1ModuleMeasProgram::checkForRestart()
{
    bool bRestartRequired = false;
    bool bStopRequired = false;
    // ZERA specials:
    bool bContinuous = m_pContinuousPar->getValue().toInt() != 0;
    bool bInputHK = m_pDutInputPar->getValue().toString().contains("HK");

    // Still running?
    if(m_bMeasurementRunning) {
        if(bContinuous) {
            // Continuous measurement on HK is performed as multiple measurement
            if(bInputHK) {
                bRestartRequired = true;
            }
        }
        else {
            if(m_nMeasurementsToGo > 0) {
                bRestartRequired = --m_nMeasurementsToGo > 0;
            }
            if(!bRestartRequired) { // either or
                bStopRequired = true;
            }
        }
    }
    // ECALCSTATUS::ABORT: stopMeasurement(true) has stopped already -> no need to stop here again
    if(bStopRequired) {
        stopMeasurement(false);
    }
    else if(bRestartRequired) {
        int waitTimeMs = m_pMeasWait->getValue().toInt() * 1000;
        // No wait for HK / continuous / zero wait time
        if(bInputHK || bContinuous || waitTimeMs == 0) {
            startNext();
        }
        // and zzz...
        else {
            m_nStatus =  ECALCSTATUS::READY | ECALCSTATUS::WAIT;
            m_pStatusAct->setValue(QVariant(m_nStatus));
            m_WaitStartDateTime = QDateTime::currentDateTime();
            m_WaitMultiTimer.start(waitTimeMs);
        }
    }
}

void cSec1ModuleMeasProgram::setRating()
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


void cSec1ModuleMeasProgram::newStartStop(QVariant startstop)
{
    bool ok;
    int ss = startstop.toInt(&ok);
    if (ss > 0) // we get started
    {
        m_bFirstMeas = true; // it is the first measurement
        m_nMeasurementsToGo = m_pMeasCountPar->getValue().toInt();
        m_nMeasurementNo = 0;
        m_pMeasNumAct->setValue(m_nMeasurementNo);
        m_multipleResultHelper.clear();
        multiResultToVein();
        if (!m_startMeasurementMachine.isRunning())
            m_startMeasurementMachine.start();
        // setsync
        // mtvorwahl setzen
        // master -> Input mux setzen
        // slave -> Input mux setzen
        // meas mode setzen + arm
        // m_ActualizeTimer.start();
    }
    else {
        stopMeasurement(true);
    }
}


void cSec1ModuleMeasProgram::newDutConstant(QVariant dutconst)
{
    bool ok;
    getConfData()->m_fDutConstant.m_fPar = dutconst.toDouble(&ok);
    setInterfaceComponents();
    if (!m_bMeasurementRunning && m_nEnergyCounterFinal != 0) {
        setECResult();
    }

    emit m_pModule->parameterChanged();
}

void cSec1ModuleMeasProgram::newDutConstantScale(QVariant value,const QString componentName)
{
    //parameters are not used because of combined calculation
    //the values are avaialable for logging/Debugging or future applications.
    Q_UNUSED(value)
    Q_UNUSED(componentName)


    QString conf=m_pDutTypeMeasurePoint->getValue().toString();

    float numVal=1;
    float denVal=1;
    if(conf != "CpIpUp" && conf != "CsIsUs"){
        QStringList uNumeratorElements=m_pDutConstantUScaleNum->getValue().toString().split("/");
        QStringList uDenominatorElements=m_pDutConstantUScaleDenom->getValue().toString().split("/");
        QString iNumerator=m_pDutConstantIScaleNum->getValue().toString();
        QString iDenominator=m_pDutConstantIScaleDenom->getValue().toString();
        // parse string equation to float value (see validator)
        numVal=uNumeratorElements[0].toFloat()*iNumerator.toFloat();
        if(uNumeratorElements.length()>1){
            if(uNumeratorElements[1]=="sqrt(3)"){
                numVal=numVal*sqrt(3);
            }
        }

        // parse string equation to float value (see validator)
        denVal=uDenominatorElements[0].toFloat()*iDenominator.toFloat();
        if(uDenominatorElements.length()>1){
            if(uDenominatorElements[1]=="sqrt(3)"){
                denVal=denVal*sqrt(3);
            }
        }
    }
    if(conf == "CpIsUs"){
        m_dutConstantScalingMem=denVal/numVal;
    }else if(conf == "CsIpUp"){
        m_dutConstantScalingMem=numVal/denVal;
    }else{
        m_dutConstantScalingMem=1;
    }
    cmpDependencies();

}


void cSec1ModuleMeasProgram::newDutConstantUnit(QVariant dutconstunit)
{
    m_sDutConstantUnit = dutconstunit.toString();
    getConfData()->m_sDutConstantUnit.m_sPar = m_sDutConstantUnit;
    setInterfaceComponents(); // to compute the dependencies
    quint32 flagsForRecalc = ECALCSTATUS::READY;
    if (m_pContinuousPar->getValue().toInt() != 0) {
        // continuous measurement keeps last result on abort
        flagsForRecalc |= ECALCSTATUS::ABORT;
    }
    if(m_bActive && (m_nStatus & flagsForRecalc)) {
        setECResult();
    }

    emit m_pModule->parameterChanged();
}


void cSec1ModuleMeasProgram::newRefConstant(QVariant refconst)
{
    bool ok;
    getConfData()->m_fRefConstant.m_fPar = refconst.toDouble(&ok);
    setInterfaceComponents();

    emit m_pModule->parameterChanged();
}


void cSec1ModuleMeasProgram::newDutInput(QVariant dutinput)
{
    getConfData()->m_sDutInput.m_sPar = mDUTSecInputSelectionHash[dutinput.toString()]->name;
    setInterfaceComponents();

    emit m_pModule->parameterChanged();
}


void cSec1ModuleMeasProgram::newRefInput(QVariant refinput)
{
    QString refInputName = mREFSecInputSelectionHash[refinput.toString()]->name;
    getConfData()->m_sRefInput.m_sPar = refInputName;
    setInterfaceComponents();

    // if the reference input changes P <-> Q <-> S it is necessary to change dut constanst unit and its validator

    QStringList sl = getDutConstUnitValidator();
    initDutConstantUnit(sl);
    m_pDutConstanstUnitValidator->setValidator(sl);
    QString s = getEnergyUnit();
    m_pEnergyAct->setUnit(s);
    m_pEnergyFinalAct->setUnit(s);
    m_pEnergyPar->setUnit(s);
    m_pEnergyAct->setValue(0.0);
    m_pEnergyFinalAct->setValue(0.0);
    m_pResultAct->setValue(0.0);
    m_pRefFreqInput->setValue(refInputName);
    m_pModule->exportMetaData();

    emit m_pModule->parameterChanged();
}


void cSec1ModuleMeasProgram::newMRate(QVariant mrate)
{
    bool ok;
    getConfData()->m_nMRate.m_nPar = mrate.toInt(&ok);
    setInterfaceComponents();

    emit m_pModule->parameterChanged();
}


void cSec1ModuleMeasProgram::newTarget(QVariant target)
{
    bool ok;
    getConfData()->m_nTarget.m_nPar = target.toInt(&ok);
    setInterfaceComponents();

    emit m_pModule->parameterChanged();
}


void cSec1ModuleMeasProgram::newEnergy(QVariant energy)
{
    bool ok;
    getConfData()->m_fEnergy.m_fPar = energy.toDouble(&ok);
    setInterfaceComponents();

    emit m_pModule->parameterChanged();
}


void cSec1ModuleMeasProgram::newUpperLimit(QVariant limit)
{
    bool ok;
    getConfData()->m_fUpperLimit.m_fPar = limit.toDouble(&ok);
    setInterfaceComponents();
    setRating();

    emit m_pModule->parameterChanged();
}


void cSec1ModuleMeasProgram::newLowerLimit(QVariant limit)
{
    bool ok;
    getConfData()->m_fLowerLimit.m_fPar = limit.toDouble(&ok);
    setInterfaceComponents();
    setRating();

    emit m_pModule->parameterChanged();
}


void cSec1ModuleMeasProgram::Actualize()
{
    if(m_bMeasurementRunning) { // still running
        if((m_nStatus & ECALCSTATUS::WAIT) == 0) { // measurement: next poll
            m_MsgNrCmdList[m_pSECInterface->readRegister(m_MasterEcalculator.name, ECALCREG::STATUS)] = actualizestatus;
            m_MsgNrCmdList[m_pSECInterface->readRegister(m_MasterEcalculator.name, ECALCREG::MTCNTact)] = actualizeprogress;
            m_MsgNrCmdList[m_pSECInterface->readRegister(m_SlaveEcalculator.name, ECALCREG::MTCNTact)] = actualizeenergy;
        }
        else { // wait: actualize progress
            double waitTimeMs = m_pMeasWait->getValue().toInt() * 1000;
            QDateTime now = QDateTime::currentDateTime();
            double elapsedMs = m_WaitStartDateTime.msecsTo(now);
            m_fProgress = elapsedMs / (waitTimeMs / 100+0);
            m_pProgressAct->setValue(QVariant(m_fProgress));
        }
    }
}

void cSec1ModuleMeasProgram::clientActivationChanged(bool bActive)
{
    // Adjust our m_ActualizeTimer timeout to our client's needs
    m_ActualizeTimer.setInterval(bActive ? m_nActualizeIntervallHighFreq : m_nActualizeIntervallLowFreq);
}

void cSec1ModuleMeasProgram::stopMeasurement(bool bAbort)
{
    // Do not post-abort a measurement finished properly e.g by changing
    // ranges...
    if(bAbort && m_bMeasurementRunning) {
        m_nStatus = ECALCSTATUS::ABORT;
        m_pStatusAct->setValue(QVariant(m_nStatus));
    }
    m_bMeasurementRunning = false;
    m_MsgNrCmdList[m_pSECInterface->stop(m_MasterEcalculator.name)] = stopmeas;
    m_pStartStopPar->setValue(QVariant(0));
    m_ActualizeTimer.stop();
    m_WaitMultiTimer.stop();
}


bool cSec1ModuleMeasProgram::found(QList<QString> &list, QString searched)
{
    for (int i = 0; i < list.count(); i++)
        if (list.at(i).contains(searched))
            return true;
    return false;
}

bool cSec1ModuleMeasProgram::found(QList<cSec1ModuleConfigData::TRefInput> &list, QString searched)
{
    for (int i = 0; i < list.count(); i++) {
        if (list.at(i).inputName.contains(searched))
            return true;
    }
    return false;
}


}

