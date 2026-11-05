#include "sfcmodulemeasprogram.h"
#include "sfcmodule.h"
#include <errormessages.h>
#include <intvalidator.h>
#include <scpi.h>
#include <reply.h>
#include <proxy.h>
#include <timerfactoryqt.h>

namespace SFCMODULE
{

cSfcModuleMeasProgram::cSfcModuleMeasProgram(cSfcModule *module) :
    cBaseMeasProgram(module->getVeinModuleName()),
    m_pModule(module),
    m_secInterface(std::make_unique<Zera::cSECInterface>())
{
    const cSfcModuleConfigData *configData = m_pModule->getConfigData();
    m_dutInputDictionary.fillFromConfig(configData->m_dutConfigs);

    //m_ecalcServerConnectState.addTransition(this, &cSfcModuleMeasProgram::activationContinue, &m_fetchECalcUnitsState); // connect to ecalc server
    //transition from this state to m_fetch....is done in ecalcServerConnect
    m_fetchECalcUnitsState.addTransition(this, &cSfcModuleMeasProgram::activationContinue, &m_activationDoneState);

    m_setMasterMuxState.addTransition(this, &cSfcModuleMeasProgram::setupContinue, &m_setMasterMeasModeState);
    m_setMasterMeasModeState.addTransition(this, &cSfcModuleMeasProgram::setupContinue, &m_activationDoneState);

    m_activationMachine.addState(&m_ecalcServerConnectState);
    m_activationMachine.addState(&m_fetchECalcUnitsState);
    m_activationMachine.addState(&m_setMasterMuxState);
    m_activationMachine.addState(&m_setMasterMeasModeState);
    m_activationMachine.addState(&m_activationDoneState);

    m_activationMachine.setInitialState(&m_ecalcServerConnectState);

    connect(&m_ecalcServerConnectState, &QState::entered, this, &cSfcModuleMeasProgram::ecalcServerConnect);
    connect(&m_fetchECalcUnitsState, &QState::entered, this, &cSfcModuleMeasProgram::fetchECalcUnits);
    connect(&m_setMasterMuxState, &QState::entered, this, &cSfcModuleMeasProgram::setMasterMux);
    connect(&m_setMasterMeasModeState, &QState::entered, this, &cSfcModuleMeasProgram::setMasterMeasMode);
    connect(&m_activationDoneState, &QState::entered, this, &cSfcModuleMeasProgram::activationDone);


    // setting up statemachine used when starting a measurement
    m_startMeasurementState.addTransition(this, &cSfcModuleMeasProgram::setupContinue, &m_startMeasurementDoneState);

    m_startMeasurementMachine.addState(&m_startMeasurementState);
    m_startMeasurementMachine.addState(&m_startMeasurementDoneState);

    m_startMeasurementMachine.setInitialState(&m_startMeasurementState);

    connect(&m_startMeasurementState, &QState::entered, this, &cSfcModuleMeasProgram::startMeasurement);
    connect(&m_startMeasurementDoneState, &QState::entered, this, &cSfcModuleMeasProgram::startMeasurementDone);

    // setting up statemachine to free the occupied resources
    m_stopECalculatorState.addTransition(this, &cSfcModuleMeasProgram::deactivationContinue, &m_freeECalculatorState);
    m_freeECalculatorState.addTransition(this, &cSfcModuleMeasProgram::deactivationContinue, &m_deactivationDoneState);

    m_deactivationMachine.addState(&m_stopECalculatorState);
    m_deactivationMachine.addState(&m_freeECalculatorState);
    m_deactivationMachine.addState(&m_deactivationDoneState);

    m_deactivationMachine.setInitialState(&m_stopECalculatorState);

    connect(&m_stopECalculatorState, &QState::entered, this, &cSfcModuleMeasProgram::stopECCalculator);
    connect(&m_freeECalculatorState, &QState::entered, this, &cSfcModuleMeasProgram::freeECalculator);
    connect(&m_deactivationDoneState, &QState::entered, this, &cModuleActivist::deactivated);
}

void cSfcModuleMeasProgram::start()
{
}

void cSfcModuleMeasProgram::stop()
{
}

void cSfcModuleMeasProgram::updateProgress(quint32 flankCountActual)
{
    m_pFlankCountAct->setValue(flankCountActual);
}

enum sfcmoduleCmds
{
    fetchecalcunits,

    freeecalcunits,

    stopmeas,

    setmastermux,
    setmastermeasmode,
    startmeasurement,

    actualizeprogress,
    readstatus
};

void cSfcModuleMeasProgram::stopMeasurement(bool bAbort)
{
    Q_UNUSED(bAbort)
    setStatus(ECALCSTATUS::READY);
    m_bMeasurementRunning = false;
    m_MsgNrCmdList[m_secInterface->stop(m_masterEcName)] = stopmeas;
    m_ActualizeTimer->stop();
}

void cSfcModuleMeasProgram::setStatus(quint32 status)
{
    m_pStatusAct->setValue(QVariant::fromValue<quint32>(status));
}

void cSfcModuleMeasProgram::ecalcServerConnect()
{
    m_secInterface->setClientSuperSmart(m_pModule->getNetworkConfig()->m_secServiceConnectionInfo,
                                        m_pModule->getNetworkConfig()->m_tcpNetworkFactory);
    m_ecalcServerConnectState.addTransition(m_secInterface->getClientSmart().get(), &Zera::ProxyClient::connected, &m_fetchECalcUnitsState);
    connect(m_secInterface.get(), &AbstractServerInterface::serverAnswer, this, &cSfcModuleMeasProgram::catchInterfaceAnswer);
    Zera::Proxy::getInstance()->startConnectionSmart(m_secInterface->getClientSmart());
}

void cSfcModuleMeasProgram::fetchECalcUnits()
{
    m_MsgNrCmdList[m_secInterface->setECalcUnit(1)] = fetchecalcunits; // we need 1 ecalc units to cascade
}

void cSfcModuleMeasProgram::setMasterMux()
{
    const QString &dutInputName = m_pModule->getConfigData()->m_dutConfigs.m_input.m_sValue;
    m_MsgNrCmdList[m_secInterface->setMux(m_masterEcName, dutInputName)] = setmastermux;
}

void cSfcModuleMeasProgram::setMasterMeasMode()
{
    m_MsgNrCmdList[m_secInterface->setCmdid(m_masterEcName, ECALCCMDID::FLANKCOUNT)] = setmastermeasmode;
}

void cSfcModuleMeasProgram::activationDone()
{
    connect(m_pStartStopPar, &VfModuleParameter::sigValueChanged, this, &cSfcModuleMeasProgram::onStartStopChanged);
    m_ActualizeTimer = TimerFactoryQt::createPeriodic(m_nActualizeIntervall);
    connect(m_ActualizeTimer.get(), &TimerTemplateQt::sigExpired, this, &cSfcModuleMeasProgram::Actualize);

    m_ContinousTimer = TimerFactoryQt::createPeriodic(m_nActualizeIntervall);
    connect(m_ContinousTimer.get(), &TimerTemplateQt::sigExpired, this, &cSfcModuleMeasProgram::readStatusRegister);

    m_bActive = true;

    m_ContinousTimer->start();

    emit activated();
}

void cSfcModuleMeasProgram::catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer)
{
    if (msgnr == 0) { // 0 was reserved for async. messages
    }
    else {
        if (m_MsgNrCmdList.contains(msgnr))
        {
            int cmd = m_MsgNrCmdList.take(msgnr);
            switch (cmd)
            {
            case fetchecalcunits:
            {
                qInfo("%s: Units fetched: %s", qPrintable(m_pModule->getVeinModuleName()), qPrintable(answer.toString()));
                QStringList sl = answer.toString().split(';');
                if ((reply == ack) && (sl.length() >= 2)) {
                    m_masterEcName = sl.at(0);
                    emit activationContinue();
                }
                else
                    notifyError(fetchsececalcunitErrMsg);
                break;
            }
            case stopmeas:
                if (reply == ack)
                    emit deactivationContinue();
                else
                    notifyError(stopmeasErrMsg);
                break;
            case freeecalcunits:
                if (reply == ack) // we only continue if sec server manager acknowledges
                    emit deactivationContinue();
                else
                    notifyError(freesececalcunitErrMsg);
                break;
            case setmastermux:
                if (reply == ack)
                    emit setupContinue();
                else
                    notifyError(setmuxErrMsg);
                break;
            case setmastermeasmode:
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
            case readstatus:
                if (reply == ack) {
                    quint32 statusRegister = answer.toUInt();
                    if (m_bMeasurementRunning) // FPGA doe not supply READY
                        setStatus(statusRegister & 7);

                    m_pLedStateAct->setValue((statusRegister & (1<<8)) == 0 ? 1 : 0);
                    m_pLedInitialStateAct->setValue((statusRegister & (1<<9)) == 0 ? 1 : 0);
                }
                else
                    notifyError(readsecregisterErrMsg);
                break;

            case actualizeprogress:
                if (reply == ack)
                    updateProgress(answer.toUInt());
                else
                    notifyError(readsecregisterErrMsg);
                break;
            }
        }
    }
}

void cSfcModuleMeasProgram::generateVeinInterface()
{
    QString key;
    m_pStartStopPar = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                            key = QString("PAR_StartStop"),
                                            QString("Start/stop measurement:\n"
                                                    "0: stop\n"
                                                    "1: start"),
                                            QVariant((int)0));
    m_pStartStopPar->setValidator(new cIntValidator(0,1,1));
    m_pStartStopPar->setScpiInfo("CALCULATE", "START", SCPI::isCmdwP | SCPI::isQuery);
    m_pModule->m_veinModuleParameterMap[key] = m_pStartStopPar;

    m_pStatusAct = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                         key = QString("ACT_Status"),
                                         QString("Status:\n"
                                                 "0: Idle\n"
                                                 "1: Waiting for first pulse\n"
                                                 "2: Started\n"
                                                 "4: Ready"),
                                         QVariant((quint32)0) );
    m_pStatusAct->setScpiInfo("CALCULATE", "STATUS", SCPI::isQuery);
    m_pModule->m_veinModuleParameterMap[key] = m_pStatusAct;

    m_pFlankCountAct = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                             key = QString("ACT_FlankCount"),
                                             QString("Measurement of flank count"),
                                             QVariant((quint32)0));
    m_pFlankCountAct->setScpiInfo("CALCULATE", "FLANKCOUNT", SCPI::isQuery);
    m_pModule->m_veinModuleParameterMap[key] = m_pFlankCountAct;

    m_pLedStateAct = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                             key = QString("ACT_LedState"),
                                             QString("Current state of scanning head LED (on=1, off=0)"),
                                             QVariant((int)0));
    m_pLedStateAct->setScpiInfo("CALCULATE", "LEDSTATUS", SCPI::isQuery);
    m_pModule->m_veinModuleParameterMap[key] = m_pLedStateAct;

    m_pLedInitialStateAct = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                           key = QString("ACT_InitialLedState"),
                                           QString("Initial state of scanning head LED (on=1, off=0)"),
                                           QVariant((int)0));
    m_pLedInitialStateAct->setScpiInfo("CALCULATE", "LEDSTATUS", SCPI::isQuery);
    m_pModule->m_veinModuleParameterMap[key] = m_pLedInitialStateAct;
}

void cSfcModuleMeasProgram::onStartStopChanged(QVariant newValue)
{
    bool ok;
    if(newValue.toInt(&ok) > 0)
    {
        if (!m_startMeasurementMachine.isRunning())
            m_startMeasurementMachine.start();
    }else
        stopMeasurement(true);
}

void cSfcModuleMeasProgram::stopECCalculator()
{
    m_MsgNrCmdList[m_secInterface->stop(m_masterEcName)] = stopmeas;
}

void cSfcModuleMeasProgram::freeECalculator()
{
    m_ContinousTimer->stop();
    m_bActive = false;
    m_MsgNrCmdList[m_secInterface->freeECalcUnits()] = freeecalcunits;
}

void cSfcModuleMeasProgram::startMeasurement()
{
    m_pFlankCountAct->setValue(0);
    if(!m_pModule->getDemo())
        m_MsgNrCmdList[m_secInterface->start(m_masterEcName)] = startmeasurement;
}

void cSfcModuleMeasProgram::startMeasurementDone()
{
    m_bMeasurementRunning = true;
    Actualize();
    readStatusRegister();
    m_ActualizeTimer->start();
}

void cSfcModuleMeasProgram::Actualize()
{
    if(m_bMeasurementRunning) {
        if(!m_pModule->getDemo()) {
            m_MsgNrCmdList[m_secInterface->readRegister(m_masterEcName, ECALCREG::MTCNTact)] = actualizeprogress;
        }
    }
}

void cSfcModuleMeasProgram::readStatusRegister()
{
    m_MsgNrCmdList[m_secInterface->readRegister(m_masterEcName, ECALCREG::STATUS)] = readstatus;
}

}
