#include <QtGlobal>
#include <QString>
#include <QStateMachine>
#include <QJsonDocument>
#include <QDateTime>
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
#include "sec1moduleconfiguration.h"
#include "unithelper.h"

namespace SEC1MODULE
{

cSec1ModuleMeasProgram::cSec1ModuleMeasProgram(cSec1Module* module, Zera::Proxy::cProxy* proxy, std::shared_ptr<cBaseModuleConfiguration> pConfiguration)
    :cBaseMeasProgram(proxy, pConfiguration), m_pModule(module)
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

    n = getConfData()->m_refInpList.count();
    for (int i = 0; i < n; i++)
    {
        siInfo = mREFSecInputInfoHash.take(getConfData()->m_refInpList.at(i)); // change the hash for access via alias
        delete siInfo;
    }

    n = getConfData()->m_dutInpList.count();
    for (int i = 0; i < n; i++)
    {
        siInfo = mDUTSecInputInfoHash.take(getConfData()->m_dutInpList.at(i)); // change the hash for access via alias
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

    m_pMeasWait = new cVeinModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                           key = QString("PAR_MeasWait"),
                                           QString("Number seconds to wait on multiple measurements between measurements"),
                                           QVariant((int)0));
    m_pMeasWait->setSCPIInfo(new cSCPIInfo("CALCULATE", QString("%1:MWAIT").arg(modNr), "10", m_pMeasWait->getName(), "0", ""));
    m_pModule->veinModuleParameterHash[key] = m_pMeasWait; // for modules use
    iValidator = new cIntValidator(0, std::numeric_limits<int>::max()/1000 /* ms */, 1);
    m_pMeasWait->setValidator(iValidator);
    m_pMeasWait->setUnit("sec");

    m_pClientNotifierPar = new cVeinModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                           key = QString("PAR_ClientActiveNotify"),
                                           QString("By changing this component, a client asks us for max actualize performance"),
                                           QVariant((quint32)0));
    // unfortunately we cannot live without SCPI and validator here...
    m_pClientNotifierPar->setSCPIInfo(new cSCPIInfo("CALCULATE", QString("%1:ACTIVEPING").arg(modNr), "10", m_pClientNotifierPar->getName(), "0", ""));
    m_pModule->veinModuleParameterHash[key] = m_pClientNotifierPar; // for modules use
    iValidator = new cIntValidator(0, std::numeric_limits<quint32>::max(), 1);
    m_pClientNotifierPar->setValidator(iValidator);
    m_ClientActiveNotifier.init(m_pClientNotifierPar);
    connect(&m_ClientActiveNotifier, &ClientActiveComponent::clientActiveStateChanged, this, &cSec1ModuleMeasProgram::clientActivationChanged);

    // after configuration we still have to set the string validators

    m_pStatusAct = new cVeinModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                            key = QString("ACT_Status"),
                                            QString("Component holds status information"),
                                            QVariant((int)0) );
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

    m_pMulCountAct = new cVeinModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                               key = QString("ACT_MulCount"),
                                               QString("Multiple measurements: Count stored"),
                                               QVariant((int) 0));
    m_pModule->veinModuleParameterHash[key] = m_pMulCountAct; // and for the modules interface
    m_pMulCountAct->setSCPIInfo(new cSCPIInfo("CALCULATE",  QString("%1:MULCOUNT").arg(modNr), "2", m_pMulCountAct->getName(), "0", ""));

    m_pMulResultArray = new cVeinModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                               key = QString("ACT_MulResult"),
                                               QString("Multiple measurements: JSON result array / statistics"),
                                               QVariant(multiResultToJson()));
    m_pModule->veinModuleParameterHash[key] = m_pMulResultArray; // and for the modules interface
    m_pMulResultArray->setSCPIInfo(new cSCPIInfo("CALCULATE",  QString("%1:STJARRAY").arg(modNr), "2", m_pMulResultArray->getName(), "0", ""));
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
                    m_MasterEcalculator.secServersocket = getConfData()->m_SECServerSocket;
                    m_SlaveEcalculator.name = sl.at(1);
                    m_SlaveEcalculator.secIFace = m_pSECInterface;
                    m_SlaveEcalculator.secServersocket = getConfData()->m_SECServerSocket;
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
                    // Still running and not waiting for next?
                    if(m_bMeasurementRunning && (m_nStatus & ECALCSTATUS::WAIT) == 0) {
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
                    if(m_bMeasurementRunning) {
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

cSec1ModuleConfigData *cSec1ModuleMeasProgram::getConfData()
{
    return qobject_cast<cSec1ModuleConfiguration*>(m_pConfiguration.get())->getConfigurationData();
}


void cSec1ModuleMeasProgram::setInterfaceComponents()
{
    m_pModePar->setValue(QVariant(getConfData()->m_sMode.m_sPar));

    cmpDependencies(); // dependant on mode we calculate parameters by ourself

    m_pDutInputPar->setValue(QVariant(mDUTSecInputInfoHash[getConfData()->m_sDutInput.m_sPar]->alias));
    m_pRefInputPar->setValue(QVariant(mREFSecInputInfoHash[getConfData()->m_sRefInput.m_sPar]->alias));
    m_pDutConstantPar->setValue(QVariant(getConfData()->m_fDutConstant.m_fPar));
    m_pRefConstantPar->setValue(QVariant(getConfData()->m_fRefConstant.m_fPar));
    m_pMRatePar->setValue(QVariant(getConfData()->m_nMRate.m_nPar));
    m_pTargetPar->setValue(QVariant(getConfData()->m_nTarget.m_nPar));
    m_pEnergyPar->setValue(QVariant(getConfData()->m_fEnergy.m_fPar));
    m_pProgressAct->setValue(QVariant(double(0.0)));
    m_pUpperLimitPar->setValue(QVariant(getConfData()->m_fUpperLimit.m_fPar));
    m_pLowerLimitPar->setValue(QVariant(getConfData()->m_fLowerLimit.m_fPar));
}


void cSec1ModuleMeasProgram::setValidators()
{
    cStringValidator *sValidator;
    QString s;

    sValidator = new cStringValidator(QStringList(getConfData()->m_ModeList));
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

    powType = mREFSecInputInfoHash[getConfData()->m_sRefInput.m_sPar]->alias;

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
        double test = constant;
        test *= getConfData()->m_fEnergy.m_fPar;
        test = floor(test);
        //quint32 itest = (quint32) test;
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

void cSec1ModuleMeasProgram::resourceManagerConnect()
{
    // first we try to get a connection to resource manager over proxy
    m_pRMClient = m_pProxy->getConnection(getConfData()->m_RMSocket.m_sIP, getConfData()->m_RMSocket.m_nPort);
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
    m_MsgNrCmdList[m_pRMInterface->getResources(resourcetype)] = readresource;
}


void cSec1ModuleMeasProgram::testSecInputs()
{
    QList<QString> InputNameList;
    qint32 nref;

    nref = getConfData()->m_refInpList.count();

    // first we build up a list with properties for all configured Inputs
    for (int i = 0; i < nref; i++)
    {
        // siInfo.muxchannel = getConfData()->m_refInpList.at(i).m_nMuxerCode;
        siInfo = new cSecInputInfo();
        mREFSecInputInfoHash[getConfData()->m_refInpList.at(i)] = siInfo;
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

    for (int i = 0; i < getConfData()->m_dutInpList.count(); i++)
    {
        // siInfo.muxchannel = getConfData()->m_dutInpList.at(i).m_nMuxerCode;
        siInfo = new cSecInputInfo();
        mDUTSecInputInfoHash[getConfData()->m_dutInpList.at(i)] = siInfo;
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
    m_pSECClient = m_pProxy->getConnection(getConfData()->m_SECServerSocket.m_sIP, getConfData()->m_SECServerSocket.m_nPort);
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
    m_pPCBClient = m_pProxy->getConnection(getConfData()->m_PCBServerSocket.m_sIP, getConfData()->m_PCBServerSocket.m_nPort);
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
        m_MsgNrCmdList[m_pPCBInterface->registerNotifier(QString("SOURCE:%1:CONSTANT?").arg(getConfData()->m_sRefInput.m_sPar),QString("%1").arg(irqPCBNotifier))] = setpcbrefconstantnotifier;
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

    nref = getConfData()->m_refInpList.count();
    if (nref > 0)
    for (int i = 0; i < nref; i++)
    {
        // we could
        m_REFAliasList.append(mREFSecInputInfoHash[getConfData()->m_refInpList.at(i)]->alias); // build up a fixed sorted list of alias
        siInfo = mREFSecInputInfoHash[getConfData()->m_refInpList.at(i)]; // change the hash for access via alias
        mREFSecInputSelectionHash[siInfo->alias] = siInfo;
    }

    for (int i = 0; i < getConfData()->m_dutInpList.count(); i++)
    {
        m_DUTAliasList.append(mDUTSecInputInfoHash[getConfData()->m_dutInpList.at(i)]->alias); // build up a fixed sorted list of alias
        siInfo = mDUTSecInputInfoHash[getConfData()->m_dutInpList.at(i)]; // change the hash for access via alias
        mDUTSecInputSelectionHash[siInfo->alias] = siInfo;
    }

    m_ActualizeTimer.setInterval(m_nActualizeIntervallLowFreq);
    connect(&m_ActualizeTimer, &QTimer::timeout, this, &cSec1ModuleMeasProgram::Actualize);
    m_WaitMultiTimer.setSingleShot(true);
    connect(&m_WaitMultiTimer, &QTimer::timeout, this, &cSec1ModuleMeasProgram::startNext);

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
    m_MsgNrCmdList[m_pPCBInterface->getConstantSource(getConfData()->m_sRefInput.m_sPar)] = fetchrefconstant;

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
        m_fResult = (1.0 * getConfData()->m_nTarget.m_nPar - 1.0 * m_nEnergyCounterFinal) * 100.0 / m_nEnergyCounterFinal;
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
                                          getConfData()->m_fUpperLimit.m_fPar);
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


void cSec1ModuleMeasProgram::newMode(QVariant mode)
{
    getConfData()->m_sMode.m_sPar = mode.toString();
    setInterfaceComponents();
    m_pEnergyAct->setValue(0.0);
    m_pEnergyFinalAct->setValue(0.0);
    m_pResultAct->setValue(0.0);

    emit m_pModule->parameterChanged();
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
    QStringList sl;
    QString s;

    getConfData()->m_sRefInput.m_sPar = mREFSecInputSelectionHash[refinput.toString()]->name;
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

void cSec1ModuleMeasProgram::MultipleResultHelper::clear()
{
    m_fAccumulSum = 0.0;
    m_fMeanValue = qQNaN();
    m_fStdDevn = qQNaN();
    m_fStdDevn1 = qQNaN();
    m_jsonResultArray = QJsonArray();
    m_iCountPass = 0;
    m_iCountUnfinish = 0;
    m_iCountFail = 0;
    m_iIdxMinValue = -1;
    m_iIdxMaxValue = -1;
    m_fMinError = qQNaN();
    m_fMaxError = qQNaN();
    m_jsonLimitArray = QJsonArray();
    m_fLastLowerLimit = qQNaN();
    m_fLastUpperLimit = qQNaN();
}

void cSec1ModuleMeasProgram::MultipleResultHelper::append(const double fResult,
                                                          const ECALCRESULT::enResultTypes eRating,
                                                          const double fLowerLimit,
                                                          const double fUpperLimit)
{
    // limits
    // note: limits are not results of a calculation so we can comape on equality
    if(m_fLastLowerLimit != fLowerLimit || m_fLastUpperLimit != fUpperLimit) {
        QJsonObject jsonObjectLimits {
            { "IDX", m_jsonResultArray.count() /* value not added yet */ },
            { "LL", fLowerLimit },
            { "UL", fUpperLimit }
        };
        m_jsonLimitArray.append(jsonObjectLimits);
        m_fLastLowerLimit = fLowerLimit;
        m_fLastUpperLimit = fUpperLimit;
    }
    // results - short names / date/time format to reduce size
    QDateTime now = QDateTime::currentDateTime();
    QString dtString = now.toString("yyyyMMddhhmmss");
    QJsonObject jsonObjectValue {
        { "V", fResult },
        { "R", int(eRating) },
        { "T", dtString }
    };
    m_jsonResultArray.append(jsonObjectValue);
    // some statistics
    switch(eRating) {
    case ECALCRESULT::RESULT_UNFINISHED:
        ++m_iCountUnfinish;
        break;
    case ECALCRESULT::RESULT_PASSED:
        ++m_iCountPass;
        break;
    default:
        ++m_iCountFail;
        break;
    }
    double value = fResult;
    // use +/- 100% for inf/nan/unfinished in statistics
    if(qIsNaN(value)) {
        value = -100.0;
    }
    else if(qIsInf(value)) {
        value = 100.0;
    }
    if(eRating == ECALCRESULT::RESULT_UNFINISHED) { // yes have seen that on HK
        value = -100.0;
    }
    // value has been added above
    int iResultCount = m_jsonResultArray.count();
    // range
    if(value < m_fMinError || iResultCount == 1) {
        m_fMinError = value;
        m_iIdxMinValue = iResultCount-1;
    }
    if(value > m_fMaxError || iResultCount == 1) {
        m_fMaxError = value;
        m_iIdxMaxValue = iResultCount-1;
    }
    // mean value
    m_fAccumulSum += value;
    double fResultCount = iResultCount;
    m_fMeanValue = m_fAccumulSum / fResultCount;

    // standard deviations: There have been loads of discussons about the n or
    // n-1 in the past. To avoid that: calc both
    double quadratDevSum = 0.0;
    for(int idx=0; idx<iResultCount; ++idx) {
        double currValue = m_jsonResultArray[idx].toObject()["V"].toDouble();
        double currDeviation = currValue - m_fMeanValue;
        quadratDevSum += currDeviation * currDeviation;
    }
    m_fStdDevn = sqrt(quadratDevSum / fResultCount);
    if(iResultCount > 1) {
        m_fStdDevn1 = sqrt(quadratDevSum / (fResultCount - 1.0));
    }
    else {
        m_fStdDevn1 = qQNaN();
    }
}

const QJsonArray &cSec1ModuleMeasProgram::MultipleResultHelper::getJSONResultArray()
{
    return m_jsonResultArray;
}

const QJsonArray &cSec1ModuleMeasProgram::MultipleResultHelper::getJSONLimitsArray()
{
    return m_jsonLimitArray;
}

const QJsonObject cSec1ModuleMeasProgram::MultipleResultHelper::getJSONStatistics()
{
    QJsonObject jsonObject {
        {QStringLiteral("countPass"), m_iCountPass},
        {QStringLiteral("countFail"), m_iCountFail},
        {QStringLiteral("countUnfinish"), m_iCountUnfinish},
        {QStringLiteral("mean"), m_fMeanValue},
        {QStringLiteral("range"), getCountTotal() > 0 ? m_fMaxError-m_fMinError : qQNaN()},
        {QStringLiteral("idxMin"), m_iIdxMinValue},
        {QStringLiteral("idxMax"), m_iIdxMaxValue},
        {QStringLiteral("stddevN"), m_fStdDevn},
        {QStringLiteral("stddevN1"), m_fStdDevn1}
    };
    return jsonObject;
}

quint32 cSec1ModuleMeasProgram::MultipleResultHelper::getCountTotal()
{
    return m_jsonResultArray.count();
}

}


