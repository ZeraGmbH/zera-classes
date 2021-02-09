#include <QtGlobal>
#include <QString>
#include <QStateMachine>
#include <rminterface.h>
#include <pcbinterface.h>
#include <secinterface.h>
#include <basemodule.h>
#include <proxy.h>
#include <proxyclient.h>
#include <scpiinfo.h>
#include <modulevalidator.h>
#include <doublevalidator.h>
#include <intvalidator.h>
#include <stringvalidator.h>
#include <veinmoduleparameter.h>
#include <veinmoduleactvalue.h>
#include <math.h>

#include "debug.h"
#include "reply.h"
#include "errormessages.h"
#include "sec1module.h"
#include "sec1modulemeasprogram.h"
#include "sec1moduleconfigdata.h"
#include "unithelper.h"

namespace SEC1MODULE
{

cSec1ModuleMeasProgram::cSec1ModuleMeasProgram(cSec1Module* module, Zera::Proxy::cProxy* proxy, cSec1ModuleConfigData& configData)
    :cBaseMeasProgram(proxy), m_pModule(module), m_ConfigData(configData)
{
    // we have to instantiate a working resource manager and secserver interface
    m_pRMInterface = new Zera::Server::cRMInterface();
    m_pSECInterface = new Zera::Server::cSECInterface();
    m_pPCBInterface = new Zera::Server::cPCBInterface();

    m_IdentifyState.addTransition(this, SIGNAL(activationContinue()), &m_testSEC1ResourceState);
    m_testSEC1ResourceState.addTransition(this, SIGNAL(activationContinue()), &m_setECResourceState); // test presence of sec1 resource
    m_setECResourceState.addTransition(this, SIGNAL(activationContinue()), &m_readResourceTypesState); // claim 2 ecalculator units
    m_readResourceTypesState.addTransition(this, SIGNAL(activationContinue()), &m_readResourcesState); // read all resources types
    m_readResourcesState.addTransition(this, SIGNAL(activationContinue()), &m_readResourceState); // init read resources
    m_readResourceState.addTransition(this, SIGNAL(activationLoop()), &m_readResourceState); // read their resources into list
    m_readResourceState.addTransition(this, SIGNAL(activationContinue()), &m_testSecInputsState); // go on if done
    m_testSecInputsState.addTransition(this, SIGNAL(activationContinue()), &m_ecalcServerConnectState); // test all configured Inputs
    //m_ecalcServerConnectState.addTransition(this, SIGNAL(activationContinue()), &m_fetchECalcUnitsState); // connect to ecalc server
    //transition from this state to m_fetch....is done in ecalcServerConnect
    m_fetchECalcUnitsState.addTransition(this, SIGNAL(activationContinue()), &m_pcbServerConnectState); // connect to pcbserver


    //m_pcbServerConnectState.addTransition(this, SIGNAL(activationContinue()), &m_pcbServerConnectState);
    //transition from this state to m_readREFInputsState....is done in pcbServerConnect
    m_readREFInputsState.addTransition(this, SIGNAL(activationContinue()), &m_readREFInputAliasState);
    m_readREFInputAliasState.addTransition(this, SIGNAL(activationContinue()), &m_readREFInputDoneState);
    m_readREFInputDoneState.addTransition(this, SIGNAL(activationLoop()), &m_readREFInputAliasState);
    m_readREFInputDoneState.addTransition(this, SIGNAL(activationContinue()), &m_readDUTInputsState);
    m_readDUTInputsState.addTransition(this, SIGNAL(activationContinue()), &m_readDUTInputAliasState);
    m_readDUTInputAliasState.addTransition(this, SIGNAL(activationContinue()), &m_readDUTInputDoneState);
    m_readDUTInputDoneState.addTransition(this, SIGNAL(activationLoop()), &m_readDUTInputAliasState);
    m_readDUTInputDoneState.addTransition(this, SIGNAL(activationContinue()), &m_setpcbREFConstantNotifierState);

    m_setpcbREFConstantNotifierState.addTransition(this, SIGNAL(activationContinue()), &m_setsecINTNotifierState);
    m_setsecINTNotifierState.addTransition(this, SIGNAL(activationContinue()), &m_activationDoneState);

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

    connect(&resourceManagerConnectState, SIGNAL(entered()), SLOT(resourceManagerConnect()));
    connect(&m_IdentifyState, SIGNAL(entered()), SLOT(sendRMIdent()));
    connect(&m_testSEC1ResourceState, SIGNAL(entered()), SLOT(testSEC1Resource()));
    connect(&m_setECResourceState, SIGNAL(entered()), SLOT(setECResource()));
    connect(&m_readResourceTypesState, SIGNAL(entered()), SLOT(readResourceTypes()));
    connect(&m_readResourcesState, SIGNAL(entered()), SLOT(readResources()));
    connect(&m_readResourceState, SIGNAL(entered()), SLOT(readResource()));
    connect(&m_testSecInputsState, SIGNAL(entered()), SLOT(testSecInputs()));
    connect(&m_ecalcServerConnectState, SIGNAL(entered()), SLOT(ecalcServerConnect()));
    connect(&m_fetchECalcUnitsState, SIGNAL(entered()), SLOT(fetchECalcUnits()));
    connect(&m_pcbServerConnectState, SIGNAL(entered()), SLOT(pcbServerConnect()));
    connect(&m_readREFInputsState, SIGNAL(entered()), SLOT(readREFInputs()));
    connect(&m_readREFInputAliasState, SIGNAL(entered()), SLOT(readREFInputAlias()));
    connect(&m_readREFInputDoneState, SIGNAL(entered()), SLOT(readREFInputDone()));
    connect(&m_readDUTInputsState, SIGNAL(entered()), SLOT(readDUTInputs()));
    connect(&m_readDUTInputAliasState, SIGNAL(entered()), SLOT(readDUTInputAlias()));
    connect(&m_readDUTInputDoneState, SIGNAL(entered()), SLOT(readDUTInputDone()));
    connect(&m_setpcbREFConstantNotifierState, SIGNAL(entered()), SLOT(setpcbREFConstantNotifier()));
    connect(&m_setsecINTNotifierState, SIGNAL(entered()), SLOT(setsecINTNotifier()));
    connect(&m_activationDoneState, SIGNAL(entered()), SLOT(activationDone()));

    // setting up statemachine to free the occupied resources
    m_freeECalculatorState.addTransition(this, SIGNAL(deactivationContinue()), &m_freeECResource);
    m_freeECResource.addTransition(this, SIGNAL(deactivationContinue()), &m_deactivationDoneState);

    m_deactivationMachine.addState(&m_freeECalculatorState);
    m_deactivationMachine.addState(&m_freeECResource);
    m_deactivationMachine.addState(&m_deactivationDoneState);

    m_deactivationMachine.setInitialState(&m_freeECalculatorState);

    connect(&m_freeECalculatorState, SIGNAL(entered()), SLOT(freeECalculator()));
    connect(&m_freeECResource, SIGNAL(entered()), SLOT(freeECResource()));
    connect(&m_deactivationDoneState, SIGNAL(entered()), SLOT(deactivationDone()));

    // setting up statemachine used when starting a measurement

    m_setsyncState.addTransition(this, SIGNAL(setupContinue()), &m_setMeaspulsesState);
    m_setMeaspulsesState.addTransition(this, SIGNAL(setupContinue()), &m_setMasterMuxState);
    m_setMasterMuxState.addTransition(this, SIGNAL(setupContinue()), &m_setSlaveMuxState);
    m_setSlaveMuxState.addTransition(this, SIGNAL(setupContinue()), &m_setMasterMeasModeState);
    m_setMasterMeasModeState.addTransition(this, SIGNAL(setupContinue()), &m_setSlaveMeasModeState);
    m_setSlaveMeasModeState.addTransition(this, SIGNAL(setupContinue()), &m_enableInterruptState);
    m_enableInterruptState.addTransition(this, SIGNAL(setupContinue()), &m_startMeasurementState);
    m_startMeasurementState.addTransition(this, SIGNAL(setupContinue()), &m_startMeasurementDoneState);

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

    connect(&m_setsyncState, SIGNAL(entered()), SLOT(setSync()));
    connect(&m_setMeaspulsesState, SIGNAL(entered()), SLOT(setMeaspulses()));
    connect(&m_setMasterMuxState, SIGNAL(entered()), SLOT(setMasterMux()));
    connect(&m_setSlaveMuxState, SIGNAL(entered()), SLOT(setSlaveMux()));
    connect(&m_setMasterMeasModeState, SIGNAL(entered()), SLOT(setMasterMeasMode()));
    connect(&m_setSlaveMeasModeState, SIGNAL(entered()), SLOT(setSlaveMeasMode()));
    connect(&m_enableInterruptState, SIGNAL(entered()), SLOT(enableInterrupt()));
    connect(&m_startMeasurementState, SIGNAL(entered()), SLOT(startMeasurement()));
    connect(&m_startMeasurementDoneState, SIGNAL(entered()), SLOT(startMeasurementDone()));

    // setting up statemachine for interrupt handling (Interrupt is thrown on measuremnt finished)
    m_readIntRegisterState.addTransition(this, SIGNAL(interruptContinue()), &m_readMTCountactState);
    m_readMTCountactState.addTransition(this, SIGNAL(interruptContinue()), &m_calcResultAndResetIntState);
    m_calcResultAndResetIntState.addTransition(this, SIGNAL(interruptContinue()), &m_FinalState);

    m_InterrupthandlingStateMachine.addState(&m_readIntRegisterState);
    m_InterrupthandlingStateMachine.addState(&m_readMTCountactState);
    m_InterrupthandlingStateMachine.addState(&m_calcResultAndResetIntState);
    m_InterrupthandlingStateMachine.addState(&m_FinalState);

    m_InterrupthandlingStateMachine.setInitialState(&m_readIntRegisterState);

    connect(&m_readIntRegisterState, SIGNAL(entered()), SLOT(readIntRegister()));
    connect(&m_readMTCountactState, SIGNAL(entered()), SLOT(readMTCountact()));
    connect(&m_calcResultAndResetIntState, SIGNAL(entered()), SLOT(setECResultAndResetInt()));
    connect(&m_FinalState, SIGNAL(entered()), SLOT(checkForRestart()));
}


cSec1ModuleMeasProgram::~cSec1ModuleMeasProgram()
{
    int n;

    n = m_ConfigData.m_refInpList.count();
    for (int i = 0; i < n; i++)
    {
        siInfo = mREFSecInputInfoHash.take(m_ConfigData.m_refInpList.at(i)); // change the hash for access via alias
        delete siInfo;
    }

    n = m_ConfigData.m_dutInpList.count();
    for (int i = 0; i < n; i++)
    {
        siInfo = mDUTSecInputInfoHash.take(m_ConfigData.m_dutInpList.at(i)); // change the hash for access via alias
        delete siInfo;
    }

    delete m_pRMInterface;
    m_pProxy->releaseConnection(m_pRMClient);
    delete m_pSECInterface;
    m_pProxy->releaseConnection(m_pSECClient);
    delete m_pPCBInterface;
    m_pProxy->releaseConnection(m_pPCBClient);
}


void cSec1ModuleMeasProgram::start()
{
    //connect(this, SIGNAL(actualValues(QVector<float>*)), this, SLOT(setInterfaceActualValues(QVector<float>*)));
}


void cSec1ModuleMeasProgram::stop()
{
    //disconnect(this, SIGNAL(actualValues(QVector<float>*)), this, SLOT(setInterfaceActualValues(QVector<float>*)));
}


void cSec1ModuleMeasProgram::generateInterface()
{
    QString s;
    QString key;

    QString modNr;
    modNr = QString("%1").arg(m_pModule->getModuleNr(),4,10,QChar('0'));

    m_pModePar = new cVeinModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                          key = QString("PAR_Mode"),
                                          QString("Component for reading and setting the modules mode"),
                                          QVariant(s = "Unknown"));
    m_pModePar->setSCPIInfo(new cSCPIInfo("CALCULATE", QString("%1:MODE").arg(modNr), "10", m_pModePar->getName(), "0", ""));
    m_pModule->veinModuleParameterHash[key] = m_pModePar; // for modules use

    m_pDutInputPar = new cVeinModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                              key = QString("PAR_DutInput"),
                                              QString("Component for reading and setting the modules dut input"),
                                              QVariant(s = "Unknown"));
    m_pDutInputPar->setSCPIInfo(new cSCPIInfo("CALCULATE", QString("%1:DUTSOURCE").arg(modNr), "10", m_pDutInputPar->getName(), "0", ""));
    m_pModule->veinModuleParameterHash[key] = m_pDutInputPar; // for modules use

    m_pRefInputPar = new cVeinModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                              key = QString("PAR_RefInput"),
                                              QString("Component for reading and setting the modules ref input"),
                                              QVariant(s = "Unknown"));
    m_pRefInputPar->setSCPIInfo(new cSCPIInfo("CALCULATE", QString("%1:REFSOURCE").arg(modNr), "10", m_pRefInputPar->getName(), "0", ""));
    m_pModule->veinModuleParameterHash[key] = m_pRefInputPar; // for modules use

    m_pRefConstantPar = new cVeinModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                                 key = QString("PAR_RefConstant"),
                                                 QString("Component for reading and setting the modules ref constant"),
                                                 QVariant((double)0.0));
    m_pRefConstantPar->setSCPIInfo(new cSCPIInfo("CALCULATE", QString("%1:REFCONSTANT").arg(modNr ), "10", m_pRefConstantPar->getName(), "0", ""));
    m_pModule->veinModuleParameterHash[key] = m_pRefConstantPar; // for modules use
    cDoubleValidator *dValidator;
    dValidator = new cDoubleValidator(1.0, 1.0e20, 1e-4);
    m_pRefConstantPar->setValidator(dValidator);

    m_pDutConstantPar = new cVeinModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                                 key = QString("PAR_DutConstant"),
                                                 QString("Component for reading and setting the modules dut constant"),
                                                 QVariant((double)0.0));
    m_pDutConstantPar->setSCPIInfo(new cSCPIInfo("CALCULATE", QString("%1:DUTCONSTANT").arg(modNr), "10", m_pDutConstantPar->getName(), "0", ""));
    m_pModule->veinModuleParameterHash[key] = m_pDutConstantPar; // for modules use
    dValidator = new cDoubleValidator(1e-6, 1.0e20, 1e-5);
    m_pDutConstantPar->setValidator(dValidator);

    m_pDutConstantUnitPar = new cVeinModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                                     key = QString("PAR_DUTConstUnit"),
                                                     QString("Component for reading and setting the modules ref constant unit"),
                                                     QVariant(s = "Unknown"));
    m_pDutConstantUnitPar->setSCPIInfo(new cSCPIInfo("CALCULATE", QString("%1:DCUNIT").arg(modNr), "10", m_pDutConstantUnitPar->getName(), "0", ""));
    m_pModule->veinModuleParameterHash[key] = m_pDutConstantUnitPar; // for modules use

    m_pMRatePar = new cVeinModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                           key = QString("PAR_MRate"),
                                           QString("Component for reading and setting the modules measuring rate"),
                                           QVariant((double)0.0));
    m_pMRatePar->setSCPIInfo(new cSCPIInfo("CALCULATE", QString("%1:MRATE").arg(modNr), "10", m_pMRatePar->getName(), "0", ""));
    m_pModule->veinModuleParameterHash[key] = m_pMRatePar; // for modules use
    cIntValidator *iValidator;
    iValidator = new cIntValidator(1, 4294967295, 1);
    m_pMRatePar->setValidator(iValidator);

    m_pTargetPar = new cVeinModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                            key = QString("PAR_Target"),
                                            QString("Component for reading and setting the modules target value"),
                                            QVariant((double)0.0));
    m_pTargetPar->setSCPIInfo(new cSCPIInfo("CALCULATE",QString("%1:TARGET").arg(modNr), "10", m_pTargetPar->getName(), "0", ""));
    m_pModule->veinModuleParameterHash[key] = m_pTargetPar; // for modules use
    iValidator = new cIntValidator(1, 4294967295, 1);
    m_pTargetPar->setValidator(iValidator);

    m_pEnergyPar = new cVeinModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                            key = QString("PAR_Energy"),
                                            QString("Component for reading and setting the modules energy value"),
                                            QVariant((double)0.0));
    m_pEnergyPar->setSCPIInfo(new cSCPIInfo("CALCULATE",  QString("%1:PARENERGY").arg(modNr), "10", m_pEnergyPar->getName(), "0", ""));
    m_pModule->veinModuleParameterHash[key] = m_pEnergyPar; // for modules use
    dValidator = new cDoubleValidator(0.0, 1.0e7, 1e-5);
    m_pEnergyPar->setValidator(dValidator);

    m_pStartStopPar = new cVeinModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                               key = QString("PAR_StartStop"),
                                               QString("Component start/stops measurement"),
                                               QVariant((int)0));
    m_pStartStopPar->setSCPIInfo(new cSCPIInfo("CALCULATE", QString("%1:START").arg(modNr), "10", m_pStartStopPar->getName(), "0", ""));
    m_pModule->veinModuleParameterHash[key] =  m_pStartStopPar; // for modules use
    iValidator = new cIntValidator(0, 1, 1);
    m_pStartStopPar->setValidator(iValidator);

    m_pContinuousPar = new cVeinModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                                key = QString("PAR_Continuous"),
                                                QString("Component enables/disables continuous measurement"),
                                                QVariant(int(0)));
    m_pContinuousPar->setSCPIInfo(new cSCPIInfo("CALCULATE", QString("%1:CONTINUOUS").arg(modNr), "10", m_pContinuousPar->getName(), "0", ""));
    m_pModule->veinModuleParameterHash[key] =  m_pContinuousPar; // for modules use
    iValidator = new cIntValidator(0, 1, 1);
    m_pContinuousPar->setValidator(iValidator);
    m_pMeasCountPar = new cVeinModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                           key = QString("PAR_MeasCount"),
                                           QString("Component for setting the number of measurements"),
                                           QVariant((int)1));
    m_pMeasCountPar->setSCPIInfo(new cSCPIInfo("CALCULATE", QString("%1:MCOUNT").arg(modNr), "10", m_pMeasCountPar->getName(), "0", ""));
    m_pModule->veinModuleParameterHash[key] = m_pMeasCountPar; // for modules use
    iValidator = new cIntValidator(1, m_nMulMeasStoredMax, 1);
    m_pMeasCountPar->setValidator(iValidator);


    // after configuration we still have to set the string validators

    m_pStatusAct = new cVeinModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                            key = QString("ACT_Status"),
                                            QString("Component holds status information"),
                                            QVariant((int) ECALCSTATUS::IDLE) );
    m_pModule->veinModuleParameterHash[key] =  m_pStatusAct; // for modules use
    m_pStatusAct->setSCPIInfo(new cSCPIInfo("CALCULATE",  QString("%1:STATUS").arg(modNr), "2", m_pStatusAct->getName(), "0", ""));

    m_pProgressAct = new cVeinModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                              key = QString("ACT_Progress"),
                                              QString("Component holds progress information"),
                                              QVariant((double) 0.0));
    m_pModule->veinModuleParameterHash[key] = m_pProgressAct; // and for the modules interface
    m_pProgressAct->setSCPIInfo(new cSCPIInfo("CALCULATE", QString("%1:PROGRESS").arg(modNr), "2", m_pProgressAct->getName(), "0", ""));

    m_pEnergyAct = new cVeinModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                            key = QString("ACT_Energy"),
                                            QString("Component holds energy since last start information"),
                                            QVariant((double) 0.0));
    m_pModule->veinModuleParameterHash[key] = m_pEnergyAct; // and for the modules interface
    m_pEnergyAct->setSCPIInfo(new cSCPIInfo("CALCULATE", QString("%1:ENERGY").arg(modNr), "2", m_pEnergyAct->getName(), "0", ""));


    m_pEnergyFinalAct = new cVeinModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                                 key = QString("ACT_EnergyFinal"),
                                                 QString("Component holds energy from last measurement"),
                                                 QVariant((double) 0.0));
    m_pModule->veinModuleParameterHash[key] = m_pEnergyFinalAct; // and for the modules interface
    m_pEnergyFinalAct->setSCPIInfo(new cSCPIInfo("CALCULATE", QString("%1:ENFINAL").arg(modNr), "2", m_pEnergyFinalAct->getName(), "0", ""));

    m_pResultAct = new cVeinModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                            key = QString("ACT_Result"),
                                            QString("Component holds the result of last measurement"),
                                            QVariant((double) 0.0));
    m_pModule->veinModuleParameterHash[key] = m_pResultAct; // and for the modules interface
    m_pResultAct->setSCPIInfo(new cSCPIInfo("CALCULATE",  QString("%1:RESULT").arg(modNr), "2", m_pResultAct->getName(), "0", ""));


    m_pUpperLimitPar = new cVeinModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                                key = QString("PAR_Uplimit"),
                                                QString("Component for reading and setting the modules upper error limit"),
                                                QVariant((double)10.0));
    m_pUpperLimitPar->setSCPIInfo(new cSCPIInfo("CALCULATE",  QString("%1:UPLIMIT").arg(modNr), "10", m_pUpperLimitPar->getName(), "0", "%"));
    m_pModule->veinModuleParameterHash[key] = m_pUpperLimitPar; // for modules use
    dValidator = new cDoubleValidator(-100.0, 100.0, 1e-6);
    m_pUpperLimitPar->setValidator(dValidator);

    m_pLowerLimitPar = new cVeinModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                                key = QString("PAR_Lolimit"),
                                                QString("Component for reading and setting the modules lower error limit"),
                                                QVariant((double)-10.0));
    m_pLowerLimitPar->setSCPIInfo(new cSCPIInfo("CALCULATE",  QString("%1:LOLIMIT").arg(modNr), "10", m_pLowerLimitPar->getName(), "0", "%"));
    m_pModule->veinModuleParameterHash[key] = m_pLowerLimitPar; // for modules use
    dValidator = new cDoubleValidator(-100.0, 100.0, 1e-6);
    m_pLowerLimitPar->setValidator(dValidator);

    m_pRatingAct = new cVeinModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                               key = QString("ACT_Rating"),
                                               QString("Component holds the rating for the last measurement"),
                                               QVariant((int) -1));
    m_pModule->veinModuleParameterHash[key] = m_pRatingAct; // and for the modules interface
    m_pRatingAct->setSCPIInfo(new cSCPIInfo("CALCULATE",  QString("%1:RATING").arg(modNr), "2", m_pRatingAct->getName(), "0", ""));

    m_pMeasNumAct = new cVeinModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                               key = QString("ACT_MeasNum"),
                                               QString("Multiple measurements: Count"),
                                               QVariant((int) 0));
    m_pModule->veinModuleParameterHash[key] = m_pMeasNumAct; // and for the modules interface
    m_pMeasNumAct->setSCPIInfo(new cSCPIInfo("CALCULATE",  QString("%1:MEASNUM").arg(modNr), "2", m_pMeasNumAct->getName(), "0", ""));

}


void cSec1ModuleMeasProgram::deleteInterface()
{
}


void cSec1ModuleMeasProgram::catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer)
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
                {
                    emit errMsg((tr(rmidentErrMSG)));
#ifdef DEBUG
                    qDebug() << rmidentErrMSG;
#endif
                    emit activationError();
                }
                break;

            case testsec1resource:
                if ((reply == ack) && (answer.toString().contains("SEC1")))
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

            case setecresource:
                if (reply == ack)
                    emit activationContinue();
                else
                {
                    emit errMsg((tr(setresourceErrMsg)));
#ifdef DEBUG
                    qDebug() << setresourceErrMsg;
#endif
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
#ifdef DEBUG
                    qDebug() << resourcetypeErrMsg;
#endif
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
#ifdef DEBUG
                    qDebug() << resourceErrMsg;
#endif
                    emit activationError();
                }
                break;
            }

            case fetchecalcunits:
            {
                QStringList sl;
                sl = answer.toString().split(';');
                if ((reply == ack) && (sl.length() >= 2))
                {
                    m_MasterEcalculator.name = sl.at(0);
                    m_MasterEcalculator.secIFace = m_pSECInterface;
                    m_MasterEcalculator.secServersocket = m_ConfigData.m_SECServerSocket;
                    m_SlaveEcalculator.name = sl.at(1);
                    m_SlaveEcalculator.secIFace = m_pSECInterface;
                    m_SlaveEcalculator.secServersocket = m_ConfigData.m_SECServerSocket;
                    emit activationContinue();
                }
                else
                {
                    emit errMsg((tr(fetchsececalcunitErrMsg)));
#ifdef DEBUG
                    qDebug() << fetchsececalcunitErrMsg;
#endif
                    emit activationError();
                }
                break;
            }

            case readrefInputalias:
            {
                if (reply == ack)
                {
                    siInfo->alias = answer.toString();
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

            case readdutInputalias:
            {
                if (reply == ack)
                {
                    siInfo->alias = answer.toString();
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

            case freeecalcunits:
                if (reply == ack) // we only continue if sec server manager acknowledges
                    emit deactivationContinue();
                else
                {
                    emit errMsg((tr(freesececalcunitErrMsg)));
#ifdef DEBUG
                    qDebug() << freesececalcunitErrMsg;
#endif
                    emit deactivationError();
                }
                break;

            case freeecresource:
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

            case actualizeprogress:
            {
                if (reply == ack)
                {
                    // keep actual values on (pending) abort
                    if((m_nStatus & ECALCSTATUS::ABORT) == 0) {
                        m_nDUTPulseCounterActual = answer.toUInt(&ok);
                        m_fProgress = ((1.0 * m_nDUTPulseCounterStart - 1.0 * m_nDUTPulseCounterActual)/ m_nDUTPulseCounterStart)*100.0;
                        if (m_fProgress > 100.0) {
                            m_fProgress = 100.0;
                        }
                        m_pProgressAct->setValue(QVariant(m_fProgress));
                    }
                }
                else
                {
                    {
                        emit errMsg((tr(readsecregisterErrMsg)));
#ifdef DEBUG
                        qDebug() << readsecregisterErrMsg;
#endif
                        emit executionError();
                    }
                }
                break;
            }

            case actualizeenergy:
            {
                if (reply == ack) {
                    // keep last values on (pending) abort
                    if((m_nStatus & ECALCSTATUS::ABORT) == 0) {
                        m_nEnergyCounterActual = answer.toUInt(&ok);
                        m_fEnergy = m_nEnergyCounterActual / m_ConfigData.m_fRefConstant.m_fPar;
                        m_pEnergyAct->setValue(m_fEnergy);
                        if (m_bFirstMeas) {
                            // keep in final until a result is calculated in
                            m_pEnergyFinalAct->setValue(m_fEnergy);
                        }
                    }
                }
                else
                {
                    {
                        emit errMsg((tr(readsecregisterErrMsg)));
#ifdef DEBUG
                        qDebug() << readsecregisterErrMsg;
#endif
                        emit executionError();
                    }
                }
                break;
            }


            case actualizestatus:
            {
                if (reply == ack)
                {
                    // don't override pending abort
                    if((m_nStatus & ECALCSTATUS::ABORT) == 0) {
                        // once ready we leave status ready (continous mode)
                        m_nStatus = (m_nStatus & ECALCSTATUS::READY) | (answer.toUInt(&ok) & 7);
                        m_pStatusAct->setValue(QVariant(m_nStatus));
                    }
                }
                else
                {
                    {
                        emit errMsg((tr(readsecregisterErrMsg)));
#ifdef DEBUG
                        qDebug() << readsecregisterErrMsg;
#endif
                        emit executionError();
                    }
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
#ifdef DEBUG
                        qDebug() << setsyncErrMsg;
#endif
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
#ifdef DEBUG
                        qDebug() << writesecregisterErrMsg;
#endif
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
#ifdef DEBUG
                        qDebug() << setmuxErrMsg;
#endif
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
#ifdef DEBUG
                        qDebug() << setcmdidErrMsg;
#endif
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
#ifdef DEBUG
                        qDebug() << startmeasErrMsg;
#endif
                        emit executionError();
                    }
                }
                break;

            case stopmeas:
                if (reply == ack)
                {
                }
                else
                {
                    {
                        emit errMsg((tr(stopmeasErrMsg)));
#ifdef DEBUG
                        qDebug() << stopmeasErrMsg;
#endif
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
#ifdef DEBUG
                    qDebug() << registerpcbnotifierErrMsg;
#endif
                    emit activationError();
                }
                break;

            case setsecintnotifier:
                if (reply == ack) // we only continue if sec server acknowledges
                    emit activationContinue();
                else
                {
                    emit errMsg((tr(registerpcbnotifierErrMsg)));
#ifdef DEBUG
                    qDebug() << registerpcbnotifierErrMsg;
#endif
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
#ifdef DEBUG
                    qDebug() << readrefconstantErrMsg;
#endif
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
#ifdef DEBUG
                    qDebug() << readsecregisterErrMsg;
#endif
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
#ifdef DEBUG
                    qDebug() << writesecregisterErrMsg;
#endif
                    emit executionError();
                }
                break;
            case readvicount:
                if (reply == ack) // we only continue if sec server acknowledges
                {
                    // On high frequency continuous measurements chances are high
                    // that an abort comes in while we fetch final energy counter.
                    // All aborts (user / change ranges) stop measurement and that
                    // latches incomplete counter values.
                    // We have an abort check in setECResultAndResetInt but that is
                    // not enough. Test case:
                    // * Run high frequency continous measurement
                    // * Stop it (all OK up here)
                    // * Change DUT constant/unit -> Crap results
                    if((m_nStatus & ECALCSTATUS::ABORT) == 0) {
                        m_nEnergyCounterFinal = answer.toLongLong(&ok);
                    }
                    emit interruptContinue();
                }
                else
                {
                    emit errMsg((tr(readsecregisterErrMsg)));
#ifdef DEBUG
                    qDebug() << readsecregisterErrMsg;
#endif
                    emit executionError();
                }
                break;

            }
        }
    }
}


void cSec1ModuleMeasProgram::setInterfaceComponents()
{
    m_pModePar->setValue(QVariant(m_ConfigData.m_sMode.m_sPar));

    cmpDependencies(); // dependant on mode we calculate parameters by ourself

    m_pDutInputPar->setValue(QVariant(mDUTSecInputInfoHash[m_ConfigData.m_sDutInput.m_sPar]->alias));
    m_pRefInputPar->setValue(QVariant(mREFSecInputInfoHash[m_ConfigData.m_sRefInput.m_sPar]->alias));
    m_pDutConstantPar->setValue(QVariant(m_ConfigData.m_fDutConstant.m_fPar));
    m_pRefConstantPar->setValue(QVariant(m_ConfigData.m_fRefConstant.m_fPar));
    m_pMRatePar->setValue(QVariant(m_ConfigData.m_nMRate.m_nPar));
    m_pTargetPar->setValue(QVariant(m_ConfigData.m_nTarget.m_nPar));
    m_pEnergyPar->setValue(QVariant(m_ConfigData.m_fEnergy.m_fPar));
    m_pProgressAct->setValue(QVariant(double(0.0)));
    m_pUpperLimitPar->setValue(QVariant(m_ConfigData.m_fUpperLimit.m_fPar));
    m_pLowerLimitPar->setValue(QVariant(m_ConfigData.m_fLowerLimit.m_fPar));
}


void cSec1ModuleMeasProgram::setValidators()
{
    cStringValidator *sValidator;
    QString s;

    sValidator = new cStringValidator(QStringList(m_ConfigData.m_ModeList));
    m_pModePar->setValidator(sValidator);

    sValidator = new cStringValidator(m_DUTAliasList);
    m_pDutInputPar->setValidator(sValidator);

    sValidator = new cStringValidator(m_REFAliasList);
    m_pRefInputPar->setValidator(sValidator);

    m_pDutConstanstUnitValidator = new cStringValidator(getDutConstUnitValidator());
    m_pDutConstantUnitPar->setValidator(m_pDutConstanstUnitValidator);

    s = getEnergyUnit();
    m_pEnergyAct->setUnit(s);
    m_pEnergyFinalAct->setUnit(s);
    m_pEnergyPar->setUnit(s);
}


QStringList cSec1ModuleMeasProgram::getDutConstUnitValidator()
{
    QStringList sl;
    QString powType;

    powType = mREFSecInputInfoHash[m_ConfigData.m_sRefInput.m_sPar]->alias;

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
    QString powerType = mREFSecInputInfoHash[m_ConfigData.m_sRefInput.m_sPar]->alias;

    return cUnitHelper::getNewEnergyUnit(powerType, QString('k'), 3600);
}


void cSec1ModuleMeasProgram::initDutConstantUnit(QStringList sl)
{
    m_sDutConstantUnit = sl.at(0);
    m_pDutConstantUnitPar->setValue(m_sDutConstantUnit);
}


void cSec1ModuleMeasProgram::initDutConstantUnit()
{
    initDutConstantUnit(getDutConstUnitValidator());
}


void cSec1ModuleMeasProgram::handleChangedREFConst()
{
    // we ask for the reference constant of the selected Input
    m_MsgNrCmdList[m_pPCBInterface->getConstantSource(m_ConfigData.m_sRefInput.m_sPar)] = fetchrefconstant;
    if ((m_nStatus & (ECALCSTATUS::ARMED | ECALCSTATUS::STARTED)) != 0) {
        m_nStatus = ECALCSTATUS::ABORT;
        m_MsgNrCmdList[m_pSECInterface->stop(m_MasterEcalculator.name)] = stopmeas;
        m_pStatusAct->setValue(QVariant(m_nStatus));
        m_pStartStopPar->setValue(QVariant(0));
        m_ActualizeTimer.stop();
    }
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

    mode = m_ConfigData.m_sMode.m_sPar;

    constant = m_ConfigData.m_fDutConstant.m_fPar; // assumed I/kxxx because all computation is based on this
    energyPerImpulse = m_sDutConstantUnit.contains(QString("/I"));

    if (energyPerImpulse)
        constant = (1.0/constant) * 1000.0; // if xxx/I we calculate in I/kxxx

    if (mode == "mrate")
    {
       // we calculate the new target value
       m_ConfigData.m_nTarget.m_nPar = floor(m_ConfigData.m_nMRate.m_nPar * m_ConfigData.m_fRefConstant.m_fPar / constant);
       m_ConfigData.m_fEnergy.m_fPar = m_ConfigData.m_nMRate.m_nPar / constant;
    }

    else

    if (mode == "energy")
    {
        // we calcute the new mrate and target
        double test = constant;
        test *= m_ConfigData.m_fEnergy.m_fPar;
        test = floor(test);
        //quint32 itest = (quint32) test;
        m_ConfigData.m_nMRate.m_nPar = ceil(constant * m_ConfigData.m_fEnergy.m_fPar);
        m_ConfigData.m_nTarget.m_nPar = floor(m_ConfigData.m_nMRate.m_nPar * m_ConfigData.m_fRefConstant.m_fPar / constant);
    }

    else

    if (mode == "target")
    {
        constant = m_ConfigData.m_nMRate.m_nPar * m_ConfigData.m_fRefConstant.m_fPar / m_ConfigData.m_nTarget.m_nPar;
        if (energyPerImpulse)
            constant = (1.0/constant) * 1000.0;
        m_ConfigData.m_fDutConstant.m_fPar = constant;
        m_ConfigData.m_fEnergy.m_fPar = m_ConfigData.m_nMRate.m_nPar / m_ConfigData.m_fDutConstant.m_fPar;
    }
}


void cSec1ModuleMeasProgram::resourceManagerConnect()
{
    // first we try to get a connection to resource manager over proxy
    m_pRMClient = m_pProxy->getConnection(m_ConfigData.m_RMSocket.m_sIP, m_ConfigData.m_RMSocket.m_nPort);
    // and then we set connection resource manager interface's connection
    m_pRMInterface->setClient(m_pRMClient); //
    resourceManagerConnectState.addTransition(m_pRMClient, SIGNAL(connected()), &m_IdentifyState);
    connect(m_pRMInterface, SIGNAL(serverAnswer(quint32, quint8, QVariant)), this, SLOT(catchInterfaceAnswer(quint32, quint8, QVariant)));
    // todo insert timer for timeout and/or connect error conditions
    m_pProxy->startConnection(m_pRMClient);
}


void cSec1ModuleMeasProgram::sendRMIdent()
{
    m_MsgNrCmdList[m_pRMInterface->rmIdent(QString("Sec1Module%1").arg(m_pModule->getModuleNr()))] = sendrmident;
}


void cSec1ModuleMeasProgram::testSEC1Resource()
{
    m_MsgNrCmdList[m_pRMInterface->getResourceTypes()] = testsec1resource;
}


void cSec1ModuleMeasProgram::setECResource()
{
    m_MsgNrCmdList[m_pRMInterface->setResource("SEC1", "ECALCULATOR", 2)] = setecresource;
}



void cSec1ModuleMeasProgram::readResourceTypes()
{
    //m_MsgNrCmdList[m_pRMInterface->getResourceTypes()] = readresourcetypes;
    // instead of taking all resourcetypes we take predefined types if we found them in our config

    if (found(m_ConfigData.m_dutInpList, "fi") || found(m_ConfigData.m_refInpList, "fi"))
        m_ResourceTypeList.append("FRQINPUT");
    if (found(m_ConfigData.m_dutInpList, "fo") || found(m_ConfigData.m_refInpList, "fo"))
        m_ResourceTypeList.append("SOURCE");
    if (found(m_ConfigData.m_dutInpList, "sh") || found(m_ConfigData.m_refInpList, "sh"))
        m_ResourceTypeList.append("SCHEAD");
    if (found(m_ConfigData.m_dutInpList, "hk") || found(m_ConfigData.m_refInpList, "hk"))
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
    m_MsgNrCmdList[m_pRMInterface->getResources(resourcetype)] = readresource;
}


void cSec1ModuleMeasProgram::testSecInputs()
{
    QList<QString> InputNameList;
    qint32 nref;

    nref = m_ConfigData.m_refInpList.count();

    // first we build up a list with properties for all configured Inputs
    for (int i = 0; i < nref; i++)
    {
        // siInfo.muxchannel = m_ConfigData.m_refInpList.at(i).m_nMuxerCode;
        siInfo = new cSecInputInfo();
        mREFSecInputInfoHash[m_ConfigData.m_refInpList.at(i)] = siInfo;
    }

    InputNameList = mREFSecInputInfoHash.keys();

    while (InputNameList.count() > 0)
    {
        QString name;
        name = InputNameList.takeFirst();

        for (int i = 0; i < m_ResourceTypeList.count(); i++)
        {
            QString resourcelist;
            resourcelist = m_ResourceHash[m_ResourceTypeList.at(i)];
            if (resourcelist.contains(name))
            {
                nref--;
                siInfo = mREFSecInputInfoHash.take(name);
                siInfo->name = name;
                siInfo->resource = m_ResourceTypeList.at(i);
                mREFSecInputInfoHash[name] = siInfo;
                break;
            }
        }
    }

    for (int i = 0; i < m_ConfigData.m_dutInpList.count(); i++)
    {
        // siInfo.muxchannel = m_ConfigData.m_dutInpList.at(i).m_nMuxerCode;
        siInfo = new cSecInputInfo();
        mDUTSecInputInfoHash[m_ConfigData.m_dutInpList.at(i)] = siInfo;
    }

    qint32 ndut;

    ndut = mDUTSecInputInfoHash.count(); // we have n configured dut Inputs
    InputNameList = mDUTSecInputInfoHash.keys();

    while (InputNameList.count() > 0)
    {
        QString name;
        name = InputNameList.takeFirst();

        for (int i = 0; i < m_ResourceTypeList.count(); i++)
        {
            QString resourcelist;
            resourcelist = m_ResourceHash[m_ResourceTypeList.at(i)];
            if (resourcelist.contains(name))
            {
                ndut--;
                siInfo = mDUTSecInputInfoHash.take(name);
                siInfo->name = name;
                siInfo->resource = m_ResourceTypeList.at(i);
                mDUTSecInputInfoHash[name] = siInfo;
                break;
            }
        }
    }

    if ((nref == 0) && (ndut == 0)) // we found all our configured Inputs
    {
        emit activationContinue(); // so lets go on
    }
    else
    {
        emit errMsg((tr(resourceErrMsg)));
        #ifdef DEBUG
            qDebug() << resourceErrMsg;
        #endif
        emit activationError();
    }

}


void cSec1ModuleMeasProgram::ecalcServerConnect()
{
    // we try to get a connection to ecalc server over proxy
    m_pSECClient = m_pProxy->getConnection(m_ConfigData.m_SECServerSocket.m_sIP, m_ConfigData.m_SECServerSocket.m_nPort);
    // and then we set ecalcalculator interface's connection
    m_pSECInterface->setClient(m_pSECClient); //
    m_ecalcServerConnectState.addTransition(m_pSECClient, SIGNAL(connected()), &m_fetchECalcUnitsState);
    connect(m_pSECInterface, SIGNAL(serverAnswer(quint32, quint8, QVariant)), this, SLOT(catchInterfaceAnswer(quint32, quint8, QVariant)));
    // todo insert timer for timeout and/or connect error conditions
    m_pProxy->startConnection(m_pSECClient);
}


void cSec1ModuleMeasProgram::fetchECalcUnits()
{
    m_MsgNrCmdList[m_pSECInterface->setECalcUnit(2)] = fetchecalcunits; // we need 2 ecalc units to cascade
}


void cSec1ModuleMeasProgram::pcbServerConnect()
{
    // we try to get a connection to ecalc server over proxy
    m_pPCBClient = m_pProxy->getConnection(m_ConfigData.m_PCBServerSocket.m_sIP, m_ConfigData.m_PCBServerSocket.m_nPort);
    // and then we set ecalcalculator interface's connection
    m_pPCBInterface->setClient(m_pPCBClient); //
    m_pcbServerConnectState.addTransition(m_pPCBClient, SIGNAL(connected()), &m_readREFInputsState);
    connect(m_pPCBInterface, SIGNAL(serverAnswer(quint32, quint8, QVariant)), this, SLOT(catchInterfaceAnswer(quint32, quint8, QVariant)));
    // todo insert timer for timeout and/or connect error conditions
    m_pProxy->startConnection(m_pPCBClient);
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
    siInfo->pcbServersocket = m_ConfigData.m_PCBServerSocket;
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
    siInfo->pcbServersocket = m_ConfigData.m_PCBServerSocket;
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
    if ( (m_ConfigData.m_nRefInpCount > 0) && m_ConfigData.m_bEmbedded ) // if we have some ref. Input and are embedded in meter we register for notification
    {
        m_MsgNrCmdList[m_pPCBInterface->registerNotifier(QString("SOURCE:%1:CONSTANT?").arg(m_ConfigData.m_sRefInput.m_sPar),QString("%1").arg(irqPCBNotifier))] = setpcbrefconstantnotifier;
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
    int nref;

    nref = m_ConfigData.m_refInpList.count();
    if (nref > 0)
    for (int i = 0; i < nref; i++)
    {
        // we could
        m_REFAliasList.append(mREFSecInputInfoHash[m_ConfigData.m_refInpList.at(i)]->alias); // build up a fixed sorted list of alias
        siInfo = mREFSecInputInfoHash[m_ConfigData.m_refInpList.at(i)]; // change the hash for access via alias
        mREFSecInputSelectionHash[siInfo->alias] = siInfo;
    }

    for (int i = 0; i < m_ConfigData.m_dutInpList.count(); i++)
    {
        m_DUTAliasList.append(mDUTSecInputInfoHash[m_ConfigData.m_dutInpList.at(i)]->alias); // build up a fixed sorted list of alias
        siInfo = mDUTSecInputInfoHash[m_ConfigData.m_dutInpList.at(i)]; // change the hash for access via alias
        mDUTSecInputSelectionHash[siInfo->alias] = siInfo;
    }

    connect(&m_ActualizeTimer, SIGNAL(timeout()), this, SLOT(Actualize()));
    connect(m_pStartStopPar, SIGNAL(sigValueChanged(QVariant)), this, SLOT(newStartStop(QVariant)));
    connect(m_pModePar, SIGNAL(sigValueChanged(QVariant)), this, SLOT(newMode(QVariant)));
    connect(m_pDutConstantPar, SIGNAL(sigValueChanged(QVariant)), this, SLOT(newDutConstant(QVariant)));
    connect(m_pDutConstantUnitPar, SIGNAL(sigValueChanged(QVariant)), this, SLOT(newDutConstantUnit(QVariant)));
    connect(m_pRefConstantPar, SIGNAL(sigValueChanged(QVariant)), this, SLOT(newRefConstant(QVariant)));
    connect(m_pDutInputPar, SIGNAL(sigValueChanged(QVariant)), this, SLOT(newDutInput(QVariant)));
    connect(m_pRefInputPar, SIGNAL(sigValueChanged(QVariant)), this, SLOT(newRefInput(QVariant)));
    connect(m_pMRatePar, SIGNAL(sigValueChanged(QVariant)), this, SLOT(newMRate(QVariant)));
    connect(m_pTargetPar, SIGNAL(sigValueChanged(QVariant)), this, SLOT(newTarget(QVariant)));
    connect(m_pEnergyPar, SIGNAL(sigValueChanged(QVariant)), this, SLOT(newEnergy(QVariant)));

    connect(m_pUpperLimitPar, SIGNAL(sigValueChanged(QVariant)), this, SLOT(newUpperLimit(QVariant)));
    connect(m_pLowerLimitPar, SIGNAL(sigValueChanged(QVariant)), this, SLOT(newLowerLimit(QVariant)));


    initDutConstantUnit();
    setInterfaceComponents(); // actualize interface components

    setValidators();

    // we ask for the reference constant of the selected Input
    m_MsgNrCmdList[m_pPCBInterface->getConstantSource(m_ConfigData.m_sRefInput.m_sPar)] = fetchrefconstant;

    m_bActive = true;
    emit activated();
}


void cSec1ModuleMeasProgram::freeECalculator()
{
    m_bActive = false;
    m_MsgNrCmdList[m_pSECInterface->freeECalcUnits()] = freeecalcunits;
}


void cSec1ModuleMeasProgram::freeECResource()
{
    m_MsgNrCmdList[m_pRMInterface->freeResource("SEC1", "ECALCULATOR")] = freeecresource;
}


void cSec1ModuleMeasProgram::deactivationDone()
{
    m_pProxy->releaseConnection(m_pRMClient);

    disconnect(m_pRMInterface, 0, this, 0);
    disconnect(m_pSECInterface, 0, this, 0);
    disconnect(m_pPCBInterface, 0, this, 0);

    disconnect(m_pStartStopPar, 0, this, 0);
    disconnect(m_pModePar, 0, this, 0);
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
    // All preparations done: do start
    m_MsgNrCmdList[m_pSECInterface->start(m_MasterEcalculator.name)] = startmeasurement;
}


void cSec1ModuleMeasProgram::startMeasurementDone() // final state of m_startMeasurementMachine
{
    Actualize(); // we acualize at once after started
    m_ActualizeTimer.start(m_ConfigData.m_fMeasInterval*1000.0); // and after configured interval
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
        m_nRating = -1;
    }
    else
    {
        m_fResult = (1.0 * m_ConfigData.m_nTarget.m_nPar - 1.0 * m_nEnergyCounterFinal) * 100.0 / m_nEnergyCounterFinal;
        setRating();
    }

    m_fEnergy = 1.0 * m_nEnergyCounterFinal / m_ConfigData.m_fRefConstant.m_fPar;
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
    if((m_nStatus & ECALCSTATUS::ABORT) == 0) {
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
    }

    // enable next int
    resetIntRegister();
}

void cSec1ModuleMeasProgram::checkForRestart()
{
    bool bRestartRequired = false;
    bool bStopRequired = false;

    if(m_pContinuousPar->getValue().toInt() != 0) {
        // Continuous measurement on HK is performed as multiple measurement
        if(m_pDutInputPar->getValue().toString().contains("HK")) {
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

    if(bStopRequired) {
        m_pStartStopPar->setValue(QVariant(0)); // restart enable
        newStartStop(QVariant(0)); // we don't get a signal from notification of setvalue ....
    }
    else if(bRestartRequired) {
        // Notes on re-start:
        // * We don't need the whole start state machine here
        // * There is too much magic in startMeasurement so we cannnot use it here either
        m_MsgNrCmdList[m_pSECInterface->stop(m_MasterEcalculator.name)] = stopmeas;
        m_nStatus = ECALCSTATUS::ARMED | ECALCSTATUS::READY;
        m_MsgNrCmdList[m_pSECInterface->start(m_MasterEcalculator.name)] = startmeasurement;
        startMeasurementDone();
    }
}

void cSec1ModuleMeasProgram::setRating()
{
    if (m_nStatus & ECALCSTATUS::READY)
    {
        if ( (m_fResult >= m_ConfigData.m_fLowerLimit.m_fPar) && (m_fResult <= m_ConfigData.m_fUpperLimit.m_fPar))
            m_nRating = 1;
        else
            m_nRating = 0;
    }
    else
        m_nRating = -1;

    m_pRatingAct->setValue(m_nRating);
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
        if (!m_startMeasurementMachine.isRunning())
            m_startMeasurementMachine.start();
        // setsync
        // mtvorwahl setzen
        // master -> Input mux setzen
        // slave -> Input mux setzen
        // meas mode setzen + arm
        // m_ActualizeTimer.start(m_ConfigData.m_fMeasInterval*1000.0);
    }
    else
    {
        if ((m_nStatus & (ECALCSTATUS::ARMED | ECALCSTATUS::STARTED)) != 0)
            m_nStatus = ECALCSTATUS::ABORT;
        m_pStatusAct->setValue(QVariant(m_nStatus));
        m_MsgNrCmdList[m_pSECInterface->stop(m_MasterEcalculator.name)] = stopmeas;
        m_pStartStopPar->setValue(QVariant(0));
        m_ActualizeTimer.stop();
    }
}


void cSec1ModuleMeasProgram::newMode(QVariant mode)
{
    m_ConfigData.m_sMode.m_sPar = mode.toString();
    setInterfaceComponents();
    m_pEnergyAct->setValue(0.0);
    m_pEnergyFinalAct->setValue(0.0);
    m_pResultAct->setValue(0.0);

    emit m_pModule->parameterChanged();
}


void cSec1ModuleMeasProgram::newDutConstant(QVariant dutconst)
{
    bool ok;
    m_ConfigData.m_fDutConstant.m_fPar = dutconst.toDouble(&ok);
    setInterfaceComponents();
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


void cSec1ModuleMeasProgram::newDutConstantUnit(QVariant dutconstunit)
{
    m_sDutConstantUnit = dutconstunit.toString();
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
    m_ConfigData.m_fRefConstant.m_fPar = refconst.toDouble(&ok);
    setInterfaceComponents();

    emit m_pModule->parameterChanged();
}


void cSec1ModuleMeasProgram::newDutInput(QVariant dutinput)
{
    m_ConfigData.m_sDutInput.m_sPar = mDUTSecInputSelectionHash[dutinput.toString()]->name;
    setInterfaceComponents();

    emit m_pModule->parameterChanged();
}


void cSec1ModuleMeasProgram::newRefInput(QVariant refinput)
{
    QStringList sl;
    QString s;

    m_ConfigData.m_sRefInput.m_sPar = mREFSecInputSelectionHash[refinput.toString()]->name;
    setInterfaceComponents();

    // if the reference input changes P <-> Q <-> S it is necessary to change dut constanst unit and its validator

    sl = getDutConstUnitValidator();
    initDutConstantUnit(sl);
    m_pDutConstanstUnitValidator->setValidator(sl);
    s = getEnergyUnit();
    m_pEnergyAct->setUnit(s);
    m_pEnergyFinalAct->setUnit(s);
    m_pEnergyPar->setUnit(s);
    m_pEnergyAct->setValue(0.0);
    m_pEnergyFinalAct->setValue(0.0);
    m_pResultAct->setValue(0.0);
    m_pModule->exportMetaData();

    emit m_pModule->parameterChanged();
}


void cSec1ModuleMeasProgram::newMRate(QVariant mrate)
{
    bool ok;
    m_ConfigData.m_nMRate.m_nPar = mrate.toInt(&ok);
    setInterfaceComponents();

    emit m_pModule->parameterChanged();
}


void cSec1ModuleMeasProgram::newTarget(QVariant target)
{
    bool ok;
    m_ConfigData.m_nTarget.m_nPar = target.toInt(&ok);
    setInterfaceComponents();

    emit m_pModule->parameterChanged();
}


void cSec1ModuleMeasProgram::newEnergy(QVariant energy)
{
    bool ok;
    m_ConfigData.m_fEnergy.m_fPar = energy.toDouble(&ok);
    setInterfaceComponents();

    emit m_pModule->parameterChanged();
}


void cSec1ModuleMeasProgram::newUpperLimit(QVariant limit)
{
    bool ok;
    m_ConfigData.m_fUpperLimit.m_fPar = limit.toDouble(&ok);
    setInterfaceComponents();
    setRating();

    emit m_pModule->parameterChanged();
}


void cSec1ModuleMeasProgram::newLowerLimit(QVariant limit)
{
    bool ok;
    m_ConfigData.m_fLowerLimit.m_fPar = limit.toDouble(&ok);
    setInterfaceComponents();
    setRating();

    emit m_pModule->parameterChanged();
}


void cSec1ModuleMeasProgram::Actualize()
{
    m_MsgNrCmdList[m_pSECInterface->readRegister(m_MasterEcalculator.name, ECALCREG::STATUS)] = actualizestatus;
    m_MsgNrCmdList[m_pSECInterface->readRegister(m_MasterEcalculator.name, ECALCREG::MTCNTact)] = actualizeprogress;
    m_MsgNrCmdList[m_pSECInterface->readRegister(m_SlaveEcalculator.name, ECALCREG::MTCNTact)] = actualizeenergy;
}


bool cSec1ModuleMeasProgram::found(QList<QString> &list, QString searched)
{
    for (int i = 0; i < list.count(); i++)
        if (list.at(i).contains(searched))
            return true;
    return false;
}

}















