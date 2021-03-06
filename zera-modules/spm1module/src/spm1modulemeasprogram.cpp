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
#include "spm1module.h"
#include "spm1modulemeasprogram.h"
#include "spm1moduleconfigdata.h"
#include "spm1moduleconfiguration.h"
#include "unithelper.h"

namespace SPM1MODULE
{

cSpm1ModuleMeasProgram::cSpm1ModuleMeasProgram(cSpm1Module* module, Zera::Proxy::cProxy* proxy, std::shared_ptr<cBaseModuleConfiguration> pConfiguration)
    :cBaseMeasProgram(proxy, pConfiguration), m_pModule(module)
{
    // we have to instantiate a working resource manager and secserver interface
    m_pRMInterface = new Zera::Server::cRMInterface();
    m_pSECInterface = new Zera::Server::cSECInterface();
    m_pPCBInterface = new Zera::Server::cPCBInterface();

    m_IdentifyState.addTransition(this, SIGNAL(activationContinue()), &m_testSEC1ResourceState);
    m_testSEC1ResourceState.addTransition(this, SIGNAL(activationContinue()), &m_setECResourceState); // test presence of sec1 resource
    m_setECResourceState.addTransition(this, SIGNAL(activationContinue()), &m_readResourceTypesState); // claim 3 ecalculator units
    m_readResourceTypesState.addTransition(this, SIGNAL(activationContinue()), &m_readResourcesState); // read all resources types
    m_readResourcesState.addTransition(this, SIGNAL(activationContinue()), &m_readResourceState); // init read resources
    m_readResourceState.addTransition(this, SIGNAL(activationLoop()), &m_readResourceState); // read their resources into list
    m_readResourceState.addTransition(this, SIGNAL(activationContinue()), &m_testSpmInputsState); // go on if done
    m_testSpmInputsState.addTransition(this, SIGNAL(activationContinue()), &m_ecalcServerConnectState); // test all configured Inputs
    //m_ecalcServerConnectState.addTransition(this, SIGNAL(activationContinue()), &m_fetchECalcUnitsState); // connect to ecalc server
    //transition from this state to m_fetch....is done in ecalcServerConnect
    m_fetchECalcUnitsState.addTransition(this, SIGNAL(activationContinue()), &m_pcbServerConnectState); // connect to pcbserver


    //m_pcbServerConnectState.addTransition(this, SIGNAL(activationContinue()), &m_pcbServerConnectState);
    //transition from this state to m_readREFInputsState....is done in pcbServerConnect
    m_readREFInputsState.addTransition(this, SIGNAL(activationContinue()), &m_readREFInputAliasState);
    m_readREFInputAliasState.addTransition(this, SIGNAL(activationContinue()), &m_readREFInputDoneState);
    m_readREFInputDoneState.addTransition(this, SIGNAL(activationLoop()), &m_readREFInputAliasState);
    m_readREFInputDoneState.addTransition(this, SIGNAL(activationContinue()), &m_setpcbREFConstantNotifierState);

    m_setpcbREFConstantNotifierState.addTransition(this, SIGNAL(activationContinue()), &m_setsecINTNotifierState);
    m_setsecINTNotifierState.addTransition(this, SIGNAL(activationContinue()), &m_activationDoneState);

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

    connect(&resourceManagerConnectState, SIGNAL(entered()), SLOT(resourceManagerConnect()));
    connect(&m_IdentifyState, SIGNAL(entered()), SLOT(sendRMIdent()));
    connect(&m_testSEC1ResourceState, SIGNAL(entered()), SLOT(testSEC1Resource()));
    connect(&m_setECResourceState, SIGNAL(entered()), SLOT(setECResource()));
    connect(&m_readResourceTypesState, SIGNAL(entered()), SLOT(readResourceTypes()));
    connect(&m_readResourcesState, SIGNAL(entered()), SLOT(readResources()));
    connect(&m_readResourceState, SIGNAL(entered()), SLOT(readResource()));
    connect(&m_testSpmInputsState, SIGNAL(entered()), SLOT(testSpmInputs()));
    connect(&m_ecalcServerConnectState, SIGNAL(entered()), SLOT(ecalcServerConnect()));
    connect(&m_fetchECalcUnitsState, SIGNAL(entered()), SLOT(fetchECalcUnits()));
    connect(&m_pcbServerConnectState, SIGNAL(entered()), SLOT(pcbServerConnect()));
    connect(&m_readREFInputsState, SIGNAL(entered()), SLOT(readREFInputs()));
    connect(&m_readREFInputAliasState, SIGNAL(entered()), SLOT(readREFInputAlias()));
    connect(&m_readREFInputDoneState, SIGNAL(entered()), SLOT(readREFInputDone()));
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

    m_setsyncState.addTransition(this, SIGNAL(setupContinue()), &m_setsync2State);
    m_setsync2State.addTransition(this, SIGNAL(setupContinue()), &m_setMeaspulsesState);
    m_setMeaspulsesState.addTransition(this, SIGNAL(setupContinue()), &m_setMasterMuxState);
    m_setMasterMuxState.addTransition(this, SIGNAL(setupContinue()), &m_setSlaveMuxState);
    m_setSlaveMuxState.addTransition(this, SIGNAL(setupContinue()), &m_setSlave2MuxState);
    m_setSlave2MuxState.addTransition(this, SIGNAL(setupContinue()), &m_setMasterMeasModeState);
    m_setMasterMeasModeState.addTransition(this, SIGNAL(setupContinue()), &m_setSlaveMeasModeState);
    m_setSlaveMeasModeState.addTransition(this, SIGNAL(setupContinue()), &m_setSlave2MeasModeState);
    m_setSlave2MeasModeState.addTransition(this, SIGNAL(setupContinue()), &m_enableInterruptState);
    m_enableInterruptState.addTransition(this, SIGNAL(setupContinue()), &m_startMeasurementState);
    m_startMeasurementState.addTransition(this, SIGNAL(setupContinue()), &m_startMeasurementDoneState);

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

    connect(&m_setsyncState, SIGNAL(entered()), SLOT(setSync()));
    connect(&m_setsync2State, SIGNAL(entered()), SLOT(setSync2()));
    connect(&m_setMeaspulsesState, SIGNAL(entered()), SLOT(setMeaspulses()));
    connect(&m_setMasterMuxState, SIGNAL(entered()), SLOT(setMasterMux()));
    connect(&m_setSlaveMuxState, SIGNAL(entered()), SLOT(setSlaveMux()));
    connect(&m_setSlave2MuxState, SIGNAL(entered()), SLOT(setSlave2Mux()));
    connect(&m_setMasterMeasModeState, SIGNAL(entered()), SLOT(setMasterMeasMode()));
    connect(&m_setSlaveMeasModeState, SIGNAL(entered()), SLOT(setSlaveMeasMode()));
    connect(&m_setSlave2MeasModeState, SIGNAL(entered()), SLOT(setSlave2MeasMode()));
    connect(&m_enableInterruptState, SIGNAL(entered()), SLOT(enableInterrupt()));
    connect(&m_startMeasurementState, SIGNAL(entered()), SLOT(startMeasurement()));
    connect(&m_startMeasurementDoneState, SIGNAL(entered()), SLOT(startMeasurementDone()));

    // setting up statemachine m_finalResultStateMachine
    // mode targeted:
    // * for interrupt handling (Interrupt is thrown on measurement finished)
    // mode non targeted (Start/Stop):
    // * on user stop this machine is started.
    m_readIntRegisterState.addTransition(this, SIGNAL(interruptContinue()), &m_resetIntRegisterState);
    m_resetIntRegisterState.addTransition(this, SIGNAL(interruptContinue()), &m_readFinalEnergyCounterState);
    m_readFinalEnergyCounterState.addTransition(this, SIGNAL(interruptContinue()), &m_readFinalTimeCounterState);
    m_readFinalTimeCounterState.addTransition(this, SIGNAL(interruptContinue()), &m_setEMResultState);

    m_finalResultStateMachine.addState(&m_readIntRegisterState);
    m_finalResultStateMachine.addState(&m_resetIntRegisterState);
    m_finalResultStateMachine.addState(&m_readFinalEnergyCounterState);
    m_finalResultStateMachine.addState(&m_readFinalTimeCounterState);
    m_finalResultStateMachine.addState(&m_setEMResultState);

    m_finalResultStateMachine.setInitialState(&m_readIntRegisterState);

    connect(&m_readIntRegisterState, SIGNAL(entered()), SLOT(readIntRegister()));
    connect(&m_resetIntRegisterState, SIGNAL(entered()), SLOT(resetIntRegister()));
    connect(&m_readFinalEnergyCounterState, SIGNAL(entered()), SLOT(readVICountact()));
    connect(&m_readFinalTimeCounterState, SIGNAL(entered()), SLOT(readTCountact()));
    connect(&m_setEMResultState, SIGNAL(entered()), SLOT(setEMResult()));

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
    int n;

    n = getConfData()->m_refInpList.count();
    for (int i = 0; i < n; i++)
    {
        siInfo = mREFSpmInputInfoHash.take(getConfData()->m_refInpList.at(i));
        delete siInfo;
    }

    delete m_pRMInterface;
    m_pProxy->releaseConnection(m_pRMClient);
    delete m_pSECInterface;
    m_pProxy->releaseConnection(m_pSECClient);
    delete m_pPCBInterface;
    m_pProxy->releaseConnection(m_pPCBClient);
}


void cSpm1ModuleMeasProgram::start()
{
    //connect(this, SIGNAL(actualValues(QVector<float>*)), this, SLOT(setInterfaceActualValues(QVector<float>*)));
}


void cSpm1ModuleMeasProgram::stop()
{
    //disconnect(this, SIGNAL(actualValues(QVector<float>*)), this, SLOT(setInterfaceActualValues(QVector<float>*)));
}


void cSpm1ModuleMeasProgram::generateInterface()
{
    QString s;
    QString key;

    QString modNr;
    modNr = QString("%1").arg(m_pModule->getModuleNr(),4,10,QChar('0'));

    m_pRefInputPar = new cVeinModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                              key = QString("PAR_RefInput"),
                                              QString("Component for reading and setting the modules ref input"),
                                              QVariant(s = "Unknown"));
    m_pRefInputPar->setSCPIInfo(new cSCPIInfo("CALCULATE", QString("%1:REFSOURCE").arg(modNr), "10", m_pRefInputPar->getName(), "0", ""));
    m_pModule->veinModuleParameterHash[key] = m_pRefInputPar; // for modules use

    m_pRefConstantPar = new cVeinModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                                 key = QString("PAR_RefConstant"),
                                                 QString("Component for reading the modules ref constant"),
                                                 QVariant((double)0.0));
    m_pRefConstantPar->setSCPIInfo(new cSCPIInfo("CALCULATE", QString("%1:REFCONSTANT").arg(modNr ), "2", m_pRefConstantPar->getName(), "0", ""));
    m_pModule->veinModuleParameterHash[key] = m_pRefConstantPar; // for modules use
    // the reference constant is derived automatically from selected reference input but will be forwarded for queries
    // but so we don't need a validator

    m_pTargetedPar = new cVeinModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                              key = QString("PAR_Targeted"),
                                              QString("Component for reading and setting the modules stop mode"),
                                              QVariant((int)0));
    m_pTargetedPar->setSCPIInfo(new cSCPIInfo("CALCULATE", QString("%1:MODE").arg(modNr), "10", m_pTargetedPar->getName(), "0", ""));
    m_pModule->veinModuleParameterHash[key] = m_pTargetedPar; // for modules use
    cIntValidator *iValidator;
    iValidator = new cIntValidator(0,1,1);
    m_pTargetedPar->setValidator(iValidator);

    m_pMeasTimePar = new cVeinModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                              key = QString("PAR_MeasTime"),
                                              QString("Component for reading and setting the modules measuring time"),
                                              QVariant((quint32)10));
    m_pMeasTimePar->setSCPIInfo(new cSCPIInfo("CALCULATE", QString("%1:MTIME").arg(modNr), "10", m_pMeasTimePar->getName(), "0", "sec"));
    m_pModule->veinModuleParameterHash[key] = m_pMeasTimePar; // for modules use
    iValidator = new cIntValidator(1, Zera::Server::cSECInterface::maxSecCounterInitVal / 1000, 1); // counter in ms
    m_pMeasTimePar->setValidator(iValidator);

    m_pT0InputPar = new cVeinModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                             key = QString("PAR_T0Input"),
                                             QString("Component for reading and setting the modules start energy value"),
                                             QVariant((double)0.0));
    m_pT0InputPar->setSCPIInfo(new cSCPIInfo("CALCULATE",  QString("%1:T0INPUT").arg(modNr), "10", m_pT0InputPar->getName(), "0", ""));
    m_pModule->veinModuleParameterHash[key] = m_pT0InputPar; // for modules use
    cDoubleValidator *dValidator = new cDoubleValidator(0.0, 1.0e7, 1e-7);
    m_pT0InputPar->setValidator(dValidator);

    m_pT1InputPar = new cVeinModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                             key = QString("PAR_T1input"),
                                             QString("Component for reading and setting the modules stop energy value"),
                                             QVariant((double)0.0));
    m_pT1InputPar->setSCPIInfo(new cSCPIInfo("CALCULATE",  QString("%1:T1INPUT").arg(modNr), "10", m_pT1InputPar->getName(), "0", ""));
    m_pModule->veinModuleParameterHash[key] = m_pT1InputPar; // for modules use
    dValidator = new cDoubleValidator(0.0, 1.0e7, 1e-7);
    m_pT1InputPar->setValidator(dValidator);

    m_pInputUnitPar = new cVeinModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                               key = QString("PAR_TXUNIT"),
                                               QString("Component for reading and setting the modules energy unit"),
                                               QVariant("Unknown"));
    m_pInputUnitPar->setSCPIInfo(new cSCPIInfo("CALCULATE",  QString("%1:TXUNIT").arg(modNr), "10", m_pInputUnitPar->getName(), "0", ""));
    m_pModule->veinModuleParameterHash[key] = m_pInputUnitPar; // for modules use

    m_pStartStopPar = new cVeinModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                               key = QString("PAR_StartStop"),
                                               QString("Component start/stops measurement"),
                                               QVariant((int)0));
    m_pStartStopPar->setSCPIInfo(new cSCPIInfo("CALCULATE", QString("%1:START").arg(modNr), "10", "PAR_StartStop", "0", ""));
    m_pModule->veinModuleParameterHash[key] =  m_pStartStopPar; // for modules use
    iValidator = new cIntValidator(0, 1, 1);
    m_pStartStopPar->setValidator(iValidator);

    m_pStatusAct = new cVeinModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                            key = QString("ACT_Status"),
                                            QString("Component holds status information"),
                                            QVariant((int)0) );
    m_pModule->veinModuleParameterHash[key] =  m_pStatusAct; // and for the modules interface
    m_pStatusAct->setSCPIInfo(new cSCPIInfo("CALCULATE",  QString("%1:STATUS").arg(modNr), "2", m_pStatusAct->getName(), "0", ""));

    m_pTimeAct = new cVeinModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                          key = QString("ACT_Time"),
                                          QString("Component holds measuring time information"),
                                          QVariant((double) 0.0));
    m_pModule->veinModuleParameterHash[key] = m_pTimeAct; // and for the modules interface
    m_pTimeAct->setSCPIInfo(new cSCPIInfo("CALCULATE", QString("%1:TIME").arg(modNr), "2", m_pTimeAct->getName(), "0", "sec"));

    m_pEnergyAct = new cVeinModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                            key = QString("ACT_Energy"),
                                            QString("Component holds energy since last start information"),
                                            QVariant((double) 0.0));
    m_pModule->veinModuleParameterHash[key] = m_pEnergyAct; // and for the modules interface
    m_pEnergyAct->setSCPIInfo(new cSCPIInfo("CALCULATE", QString("%1:ENERGY").arg(modNr), "2", m_pEnergyAct->getName(), "0", ""));

    m_pPowerAct = new cVeinModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                           key = QString("ACT_Power"),
                                           QString("Component holds mean power since last start information"),
                                           QVariant((double) 0.0));
    m_pModule->veinModuleParameterHash[key] = m_pPowerAct; // and for the modules interface
    m_pPowerAct->setSCPIInfo(new cSCPIInfo("CALCULATE", QString("%1:POWER").arg(modNr), "2", m_pPowerAct->getName(), "0", ""));

    m_pResultAct = new cVeinModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                            key = QString("ACT_Result"),
                                            QString("Component holds the result of last measurement"),
                                            QVariant((double) 0.0));
    m_pModule->veinModuleParameterHash[key] = m_pResultAct; // and for the modules interface
    m_pResultAct->setSCPIInfo(new cSCPIInfo("CALCULATE",  QString("%1:RESULT").arg(modNr), "2", m_pResultAct->getName(), "0", "%"));

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
    connect(&m_ClientActiveNotifier, &ClientActiveComponent::clientActiveStateChanged, this, &cSpm1ModuleMeasProgram::clientActivationChanged);

}


void cSpm1ModuleMeasProgram::deleteInterface()
{
}


void cSpm1ModuleMeasProgram::catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer)
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
                if ((reply == ack) && (sl.length() >= 3))
                {
                    m_MasterEcalculator.name = sl.at(0);
                    m_MasterEcalculator.secIFace = m_pSECInterface;
                    m_MasterEcalculator.secServersocket = getConfData()->m_SECServerSocket;
                    m_SlaveEcalculator.name = sl.at(1);
                    m_SlaveEcalculator.secIFace = m_pSECInterface;
                    m_SlaveEcalculator.secServersocket = getConfData()->m_SECServerSocket;
                    m_Slave2Ecalculator.name = sl.at(2);
                    m_Slave2Ecalculator.secIFace = m_pSECInterface;
                    m_Slave2Ecalculator.secServersocket = getConfData()->m_SECServerSocket;
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
#ifdef DEBUG
                    qDebug() << readsecregisterErrMsg;
#endif
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
#ifdef DEBUG
                    qDebug() << readsecregisterErrMsg;
#endif
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
#ifdef DEBUG
                    qDebug() << readsecregisterErrMsg;
#endif
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
                if (reply == ack) {
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
#ifdef DEBUG
                    qDebug() << readsecregisterErrMsg;
#endif
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
    QStringList sl;

    sl = getPowerUnitValidator();

    for (int i = 0; i < sl.count(); i++)
        sl.replace(i, sl.at(i)+"h");

    return sl;
}


QString cSpm1ModuleMeasProgram::getEnergyUnit()
{
    QString s;

    s = getPowerUnit();
    s.append("h");

    return s;
}


QStringList cSpm1ModuleMeasProgram::getPowerUnitValidator()
{
    QStringList sl;
    QString powType;

    powType = mREFSpmInputInfoHash[getConfData()->m_sRefInput.m_sPar]->alias;

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
    m_pRMClient = m_pProxy->getConnection(getConfData()->m_RMSocket.m_sIP, getConfData()->m_RMSocket.m_nPort);
    // and then we set connection resource manager interface's connection
    m_pRMInterface->setClient(m_pRMClient); //
    resourceManagerConnectState.addTransition(m_pRMClient, SIGNAL(connected()), &m_IdentifyState);
    connect(m_pRMInterface, SIGNAL(serverAnswer(quint32, quint8, QVariant)), this, SLOT(catchInterfaceAnswer(quint32, quint8, QVariant)));
    // todo insert timer for timeout and/or connect error conditions
    m_pProxy->startConnection(m_pRMClient);
}


void cSpm1ModuleMeasProgram::sendRMIdent()
{
    m_MsgNrCmdList[m_pRMInterface->rmIdent(QString("Sem1Module%1").arg(m_pModule->getModuleNr()))] = sendrmident;
}


void cSpm1ModuleMeasProgram::testSEC1Resource()
{
    m_MsgNrCmdList[m_pRMInterface->getResourceTypes()] = testsec1resource;
}


void cSpm1ModuleMeasProgram::setECResource()
{
    m_MsgNrCmdList[m_pRMInterface->setResource("SEC1", "ECALCULATOR", 3)] = setecresource;
}



void cSpm1ModuleMeasProgram::readResourceTypes()
{
    //m_MsgNrCmdList[m_pRMInterface->getResourceTypes()] = readresourcetypes;
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
    m_MsgNrCmdList[m_pRMInterface->getResources(resourcetype)] = readresource;
}


void cSpm1ModuleMeasProgram::testSpmInputs()
{
    QList<QString> InputNameList;
    qint32 nref;

    nref = getConfData()->m_refInpList.count();

    // first we build up a list with properties for all configured Inputs
    for (int i = 0; i < nref; i++)
    {
        // siInfo.muxchannel = getConfData()->m_refInpList.at(i).m_nMuxerCode;
        siInfo = new cSecInputInfo();
        mREFSpmInputInfoHash[getConfData()->m_refInpList.at(i)] = siInfo;
    }

    InputNameList = mREFSpmInputInfoHash.keys();

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
                siInfo = mREFSpmInputInfoHash.take(name);
                siInfo->name = name;
                siInfo->resource = m_ResourceTypeList.at(i);
                mREFSpmInputInfoHash[name] = siInfo;
                break;
            }
        }
    }

    if (nref == 0) // we found all our configured Inputs
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


void cSpm1ModuleMeasProgram::ecalcServerConnect()
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


void cSpm1ModuleMeasProgram::fetchECalcUnits()
{
    m_MsgNrCmdList[m_pSECInterface->setECalcUnit(3)] = fetchecalcunits; // we need 3 ecalc units to cascade
}


void cSpm1ModuleMeasProgram::pcbServerConnect()
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


void cSpm1ModuleMeasProgram::readREFInputs()
{
    m_sItList = mREFSpmInputInfoHash.keys();
    emit activationContinue();
}


void cSpm1ModuleMeasProgram::readREFInputAlias()
{
    m_sIt = m_sItList.takeFirst();
    siInfo = mREFSpmInputInfoHash.take(m_sIt); // if set some info that could be useful later
    siInfo->pcbIFace = m_pPCBInterface; // in case that Inputs would be provided by several servers
    siInfo->pcbServersocket = getConfData()->m_PCBServerSocket;
    //m_MsgNrCmdList[siInfo->pcbIFace->resourceAliasQuery(siInfo->resource, m_sIt)] = readrefInputalias;

    // we will read the powertype of the reference frequency input and will use this as our alias ! for example P, +P ....
    m_MsgNrCmdList[siInfo->pcbIFace->getPowTypeSource(m_sIt)] = readrefInputalias;

}


void cSpm1ModuleMeasProgram::readREFInputDone()
{
    mREFSpmInputInfoHash[siInfo->name] = siInfo;
    if (m_sItList.isEmpty())
        emit activationContinue();
    else
        emit activationLoop();
}


void cSpm1ModuleMeasProgram::setpcbREFConstantNotifier()
{
    if ( (getConfData()->m_nRefInpCount > 0) && getConfData()->m_bEmbedded ) // if we have some ref. Input and are embedded in meter we register for notification
    {
        m_MsgNrCmdList[m_pPCBInterface->registerNotifier(QString("SOURCE:%1:CONSTANT?").arg(getConfData()->m_sRefInput.m_sPar),QString("%1").arg(irqPCBNotifier))] = setpcbrefconstantnotifier;
        // todo also configure the query for setting this notifier .....very flexible
    }
    else
        emit activationContinue(); // if no ref constant notifier (standalone error calc) we directly go on
}


void cSpm1ModuleMeasProgram::setsecINTNotifier()
{
    m_MsgNrCmdList[m_pSECInterface->registerNotifier(QString("ECAL:%1:R%2?").arg(m_MasterEcalculator.name).arg(ECALCREG::INTREG))] = setsecintnotifier;
}


void cSpm1ModuleMeasProgram::activationDone()
{
    int nref;

    nref = getConfData()->m_refInpList.count();
    if (nref > 0)
    for (int i = 0; i < nref; i++)
    {
        m_REFAliasList.append(mREFSpmInputInfoHash[getConfData()->m_refInpList.at(i)]->alias); // build up a fixed sorted list of alias
        siInfo = mREFSpmInputInfoHash[getConfData()->m_refInpList.at(i)]; // change the hash for access via alias
        mREFSpmInputSelectionHash[siInfo->alias] = siInfo;
    }

    connect(&m_ActualizeTimer, SIGNAL(timeout()), this, SLOT(Actualize()));

    connect(m_pStartStopPar, SIGNAL(sigValueChanged(QVariant)), this, SLOT(newStartStop(QVariant)));
    connect(m_pRefInputPar, SIGNAL(sigValueChanged(QVariant)), this, SLOT(newRefInput(QVariant)));

    connect(m_pTargetedPar, SIGNAL(sigValueChanged(QVariant)), this , SLOT(newTargeted(QVariant)));
    connect(m_pMeasTimePar, SIGNAL(sigValueChanged(QVariant)), this , SLOT(newMeasTime(QVariant)));
    connect(m_pT0InputPar, SIGNAL(sigValueChanged(QVariant)), this , SLOT(newT0Input(QVariant)));
    connect(m_pT1InputPar, SIGNAL(sigValueChanged(QVariant)), this , SLOT(newT1Input(QVariant)));
    connect(m_pInputUnitPar, SIGNAL(sigValueChanged(QVariant)), this, SLOT(newUnit(QVariant)));
    connect(m_pUpperLimitPar, SIGNAL(sigValueChanged(QVariant)), this, SLOT(newUpperLimit(QVariant)));
    connect(m_pLowerLimitPar, SIGNAL(sigValueChanged(QVariant)), this, SLOT(newLowerLimit(QVariant)));

    setInterfaceComponents(); // actualize interface components
    setValidators();
    setUnits();

    // we ask for the reference constant of the selected Input
    m_MsgNrCmdList[m_pPCBInterface->getConstantSource(getConfData()->m_sRefInput.m_sPar)] = fetchrefconstant;

    m_bActive = true;
    emit activated();
}


void cSpm1ModuleMeasProgram::freeECalculator()
{
    m_bActive = false;
    m_MsgNrCmdList[m_pSECInterface->freeECalcUnits()] = freeecalcunits;
}


void cSpm1ModuleMeasProgram::freeECResource()
{
    m_MsgNrCmdList[m_pRMInterface->freeResource("SEC1", "ECALCULATOR")] = freeecresource;
}


void cSpm1ModuleMeasProgram::deactivationDone()
{
    m_pProxy->releaseConnection(m_pRMClient);

    disconnect(m_pRMInterface, 0, this, 0);
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
    m_MsgNrCmdList[m_pSECInterface->setSync(m_SlaveEcalculator.name, m_MasterEcalculator.name)] = setsync;
}


void cSpm1ModuleMeasProgram::setSync2()
{
    m_MsgNrCmdList[m_pSECInterface->setSync(m_Slave2Ecalculator.name, m_MasterEcalculator.name)] = setsync;
}


void cSpm1ModuleMeasProgram::setMeaspulses()
{
    if (m_pTargetedPar->getValue().toInt() == 0)
        m_nTimerCountStart = Zera::Server::cSECInterface::maxSecCounterInitVal; // we simply set max. time -> approx. 50 days
    else
        m_nTimerCountStart = m_pMeasTimePar->getValue().toLongLong() * 1000;

    m_MsgNrCmdList[m_pSECInterface->writeRegister(m_MasterEcalculator.name, ECALCREG::MTCNTin, m_nTimerCountStart)] = setmeaspulses;
}


void cSpm1ModuleMeasProgram::setMasterMux()
{
    m_MsgNrCmdList[m_pSECInterface->setMux(m_MasterEcalculator.name, QString("t1ms"))] = setmastermux; // to be discussed
}


void cSpm1ModuleMeasProgram::setSlaveMux()
{
    m_MsgNrCmdList[m_pSECInterface->setMux(m_SlaveEcalculator.name, mREFSpmInputSelectionHash[m_pRefInputPar->getValue().toString()]->name)] = setslavemux;
}


void cSpm1ModuleMeasProgram::setSlave2Mux()
{
    m_MsgNrCmdList[m_pSECInterface->setMux(m_Slave2Ecalculator.name, QString("t1ms"))] = setslavemux;
}


void cSpm1ModuleMeasProgram::setMasterMeasMode()
{
    m_MsgNrCmdList[m_pSECInterface->setCmdid(m_MasterEcalculator.name, ECALCCMDID::SINGLEERRORMASTER)] = setmastermeasmode;
}


void cSpm1ModuleMeasProgram::setSlaveMeasMode()
{
    m_MsgNrCmdList[m_pSECInterface->setCmdid(m_SlaveEcalculator.name, ECALCCMDID::ERRORMEASSLAVE)] = setslavemeasmode;
}


void cSpm1ModuleMeasProgram::setSlave2MeasMode()
{
    m_MsgNrCmdList[m_pSECInterface->setCmdid(m_Slave2Ecalculator.name, ECALCCMDID::ERRORMEASSLAVE)] = setslavemeasmode;
}


void cSpm1ModuleMeasProgram::enableInterrupt()
{
    // in case of targeted mode we want an interrupt when ready
    // in case of not targeted mode we set the time to maximum and the user will stop the measurement by pressing stop
    // so we program enable int. in any case
    m_MsgNrCmdList[m_pSECInterface->writeRegister(m_MasterEcalculator.name, ECALCREG::INTMASK, ECALCINT::MTCount0)] = enableinterrupt;
}


void cSpm1ModuleMeasProgram::startMeasurement()
{
    m_MsgNrCmdList[m_pSECInterface->start(m_MasterEcalculator.name)] = startmeasurement;
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
    m_MsgNrCmdList[m_pSECInterface->readRegister(m_MasterEcalculator.name, ECALCREG::INTREG)] = readintregister;
}


void cSpm1ModuleMeasProgram::resetIntRegister()
{
    m_MsgNrCmdList[m_pSECInterface->intAck(m_MasterEcalculator.name, 0xF)] = resetintregister; // we reset all here
}


void cSpm1ModuleMeasProgram::readVICountact()
{
    m_MsgNrCmdList[m_pSECInterface->readRegister(m_SlaveEcalculator.name, ECALCREG::MTCNTfin)] = readvicount;
}


void cSpm1ModuleMeasProgram::readTCountact()
{
    m_MsgNrCmdList[m_pSECInterface->readRegister(m_Slave2Ecalculator.name, ECALCREG::MTCNTfin)] = readtcount;
    // non targeted has been stopped already in newStartStop()
    if(getConfData()->m_bTargeted.m_nActive) {
        m_MsgNrCmdList[m_pSECInterface->stop(m_MasterEcalculator.name)] = stopmeas;
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
            m_MsgNrCmdList[m_pSECInterface->stop(m_MasterEcalculator.name)] = stopmeas;
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
    getConfData()->m_sRefInput.m_sPar = mREFSpmInputSelectionHash[refinput.toString()]->name;
    setInterfaceComponents();
    // if the reference input changes P <-> Q <-> S it is necessary to change energyunit and powerunit and their validators

    setValidators();
    setUnits();

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
    m_MsgNrCmdList[m_pSECInterface->readRegister(m_MasterEcalculator.name, ECALCREG::STATUS)] = actualizestatus;
    m_MsgNrCmdList[m_pSECInterface->readRegister(m_SlaveEcalculator.name, ECALCREG::MTCNTact)] = actualizeenergy;
    m_MsgNrCmdList[m_pSECInterface->readRegister(m_Slave2Ecalculator.name, ECALCREG::MTCNTact)] = actualizepower;
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
    m_MsgNrCmdList[m_pSECInterface->stop(m_MasterEcalculator.name)] = stopmeas;
    m_pStartStopPar->setValue(QVariant(0));
    m_ActualizeTimer.stop();
}

}















