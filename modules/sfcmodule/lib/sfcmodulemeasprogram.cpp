#include "sfcmodulemeasprogram.h"
#include "sfcmodule.h"
#include "sfcmoduleconfiguration.h"
#include <errormessages.h>
#include <intvalidator.h>
#include <reply.h>
#include <scpi.h>
#include <proxy.h>
#include <proxyclient.h>

namespace SFCMODULE
{

cSfcModuleMeasProgram::cSfcModuleMeasProgram(cSfcModule *module, std::shared_ptr<BaseModuleConfiguration> pConfiguration)
    : cBaseMeasProgram(pConfiguration, module->getVeinModuleName()), m_pModule(module)
{
    m_pSECInterface = std::make_unique<Zera::cSECInterface>();
    m_pcbInterface = std::make_shared<Zera::cPCBInterface>();

    m_IdentifyState.addTransition(this, &cSfcModuleMeasProgram::activationContinue, &m_testSEC1ResourceState);
    m_testSEC1ResourceState.addTransition(this, &cSfcModuleMeasProgram::activationContinue, &m_setECResourceState); // test presence of sec1 resource
    m_setECResourceState.addTransition(this, &cSfcModuleMeasProgram::activationContinue, &m_readResourcesState); // claim 2 ecalculator units
    m_readResourcesState.addTransition(this, &cSfcModuleMeasProgram::activationContinue, &m_readResourceState); // init read resources
    m_readResourceState.addTransition(this, &cSfcModuleMeasProgram::activationLoop, &m_readResourceState); // read their resources into list
    m_readResourceState.addTransition(this, &cSfcModuleMeasProgram::activationContinue, &m_testSecInputsState); // go on if done
    m_testSecInputsState.addTransition(this, &cSfcModuleMeasProgram::activationContinue, &m_ecalcServerConnectState); // test all configured Inputs
    //m_ecalcServerConnectState.addTransition(this, &cSfcModuleMeasProgram::activationContinue, &m_fetchECalcUnitsState); // connect to ecalc server
    //transition from this state to m_fetch....is done in ecalcServerConnect
    m_fetchECalcUnitsState.addTransition(this, &cSfcModuleMeasProgram::activationContinue, &m_pcbServerConnectState);

    //m_pcbServerConnectState.addTransition(this, &cSfcModuleMeasProgram::activationContinue, &m_pcbServerConnectState);
    //transition from this state to m_readREFInputsState....is done in pcbServerConnect
    m_readREFInputsState.addTransition(this, &cSfcModuleMeasProgram::activationContinue, &m_readREFInputAliasState);
    m_readREFInputAliasState.addTransition(this, &cSfcModuleMeasProgram::activationContinue, &m_readREFInputDoneState);
    m_readREFInputDoneState.addTransition(this, &cSfcModuleMeasProgram::activationLoop, &m_readREFInputAliasState);
    m_readREFInputDoneState.addTransition(this, &cSfcModuleMeasProgram::activationContinue, &m_readDUTInputsState);
    m_readDUTInputsState.addTransition(this, &cSfcModuleMeasProgram::activationContinue, &m_readDUTInputAliasState);
    m_readDUTInputAliasState.addTransition(this, &cSfcModuleMeasProgram::activationContinue, &m_readDUTInputDoneState);
    m_readDUTInputDoneState.addTransition(this, &cSfcModuleMeasProgram::activationLoop, &m_readDUTInputAliasState);
    if(m_pModule->getDemo())
        m_readDUTInputDoneState.addTransition(this, &cSfcModuleMeasProgram::activationContinue, &m_activationDoneState);
    else
        m_readDUTInputDoneState.addTransition(this, &cSfcModuleMeasProgram::activationContinue, &m_setpcbREFConstantNotifierState);

    m_setpcbREFConstantNotifierState.addTransition(this, &cSfcModuleMeasProgram::activationContinue, &m_setsecINTNotifierState);
    m_setsecINTNotifierState.addTransition(this, &cSfcModuleMeasProgram::activationContinue, &m_activationDoneState);

    m_activationMachine.addState(&resourceManagerConnectState);
    m_activationMachine.addState(&m_IdentifyState);
    m_activationMachine.addState(&m_testSEC1ResourceState);
    m_activationMachine.addState(&m_setECResourceState);
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

    connect(&resourceManagerConnectState, &QState::entered, this, &cSfcModuleMeasProgram::resourceManagerConnect);
    connect(&m_IdentifyState, &QState::entered, this, &cSfcModuleMeasProgram::sendRMIdent);
    connect(&m_testSEC1ResourceState, &QState::entered, this, &cSfcModuleMeasProgram::testSEC1Resource);
    connect(&m_setECResourceState, &QState::entered, this, &cSfcModuleMeasProgram::setECResource);
    connect(&m_readResourcesState, &QState::entered, this, &cSfcModuleMeasProgram::readResources);
    connect(&m_readResourceState, &QState::entered, this, &cSfcModuleMeasProgram::readResource);
    connect(&m_testSecInputsState, &QState::entered, this, &cSfcModuleMeasProgram::testSecInputs);
    connect(&m_ecalcServerConnectState, &QState::entered, this, &cSfcModuleMeasProgram::ecalcServerConnect);
    connect(&m_fetchECalcUnitsState, &QState::entered, this, &cSfcModuleMeasProgram::fetchECalcUnits);
    connect(&m_pcbServerConnectState, &QState::entered, this, &cSfcModuleMeasProgram::pcbServerConnect);
    connect(&m_readREFInputsState, &QState::entered, this, &cSfcModuleMeasProgram::readREFInputs);
    connect(&m_readREFInputAliasState, &QState::entered, this, &cSfcModuleMeasProgram::readREFInputAlias);
    connect(&m_readREFInputDoneState, &QState::entered, this, &cSfcModuleMeasProgram::readREFInputDone);
    connect(&m_readDUTInputsState, &QState::entered, this, &cSfcModuleMeasProgram::readDUTInputs);
    connect(&m_readDUTInputAliasState, &QState::entered, this, &cSfcModuleMeasProgram::readDUTInputAlias);
    connect(&m_readDUTInputDoneState, &QState::entered, this, &cSfcModuleMeasProgram::readDUTInputDone);
    connect(&m_setpcbREFConstantNotifierState, &QState::entered, this, &cSfcModuleMeasProgram::setpcbREFConstantNotifier);
    connect(&m_setsecINTNotifierState, &QState::entered, this, &cSfcModuleMeasProgram::setsecINTNotifier);
    connect(&m_activationDoneState, &QState::entered, this, &cSfcModuleMeasProgram::activationDone);


    // setting up statemachine for interrupt handling (Interrupt is thrown on measuremnt finished)
    m_readIntRegisterState.addTransition(this, &cSfcModuleMeasProgram::interruptContinue, &m_readMTCountactState);
    m_readMTCountactState.addTransition(this, &cSfcModuleMeasProgram::interruptContinue, &m_calcResultAndResetIntState);
    m_calcResultAndResetIntState.addTransition(this, &cSfcModuleMeasProgram::interruptContinue, &m_FinalState);

    m_InterrupthandlingStateMachine.addState(&m_readIntRegisterState);
    m_InterrupthandlingStateMachine.addState(&m_readMTCountactState);
    m_InterrupthandlingStateMachine.addState(&m_calcResultAndResetIntState);
    m_InterrupthandlingStateMachine.addState(&m_FinalState);

    m_InterrupthandlingStateMachine.setInitialState(&m_readIntRegisterState);

    connect(&m_readIntRegisterState, &QState::entered, this, &cSfcModuleMeasProgram::readIntRegister);
    connect(&m_readMTCountactState, &QState::entered, this, &cSfcModuleMeasProgram::readMTCountact);
    connect(&m_calcResultAndResetIntState, &QState::entered, this, &cSfcModuleMeasProgram::setECResultAndResetInt);
    connect(&m_FinalState, &QState::entered, this, &cSfcModuleMeasProgram::checkForRestart);

    // setting up statemachine to free the occupied resources
    m_stopECalculatorState.addTransition(this, &cSfcModuleMeasProgram::deactivationContinue, &m_freeECalculatorState);
    m_freeECalculatorState.addTransition(this, &cSfcModuleMeasProgram::deactivationContinue, &m_freeECResource);
    m_freeECResource.addTransition(this, &cSfcModuleMeasProgram::deactivationContinue, &m_deactivationDoneState);

    m_deactivationMachine.addState(&m_stopECalculatorState);
    m_deactivationMachine.addState(&m_freeECalculatorState);
    m_deactivationMachine.addState(&m_freeECResource);
    m_deactivationMachine.addState(&m_deactivationDoneState);

    m_deactivationMachine.setInitialState(&m_stopECalculatorState);

    connect(&m_stopECalculatorState, &QState::entered, this, &cSfcModuleMeasProgram::stopECCalculator);
    connect(&m_freeECalculatorState, &QState::entered, this, &cSfcModuleMeasProgram::freeECalculator);
    connect(&m_freeECResource, &QState::entered, this, &cSfcModuleMeasProgram::freeECResource);
    connect(&m_deactivationDoneState, &QState::entered, this, &cSfcModuleMeasProgram::deactivationDone);

    m_resourceTypeList.addTypesFromConfig(getConfData()->m_dutInpList);
}

cSfcModuleMeasProgram::~cSfcModuleMeasProgram()
{
    Zera::Proxy::getInstance()->releaseConnection(m_pSECClient);
    Zera::Proxy::getInstance()->releaseConnection(m_pPCBClient);
}

void cSfcModuleMeasProgram::start()
{
}

void cSfcModuleMeasProgram::stop()
{
}

cSfcModuleConfigData *cSfcModuleMeasProgram::getConfData()
{
    return qobject_cast<cSfcModuleConfiguration *>(m_pConfiguration.get())->getConfigurationData();
}

void cSfcModuleMeasProgram::handleSECInterrupt()
{
    if (!m_InterrupthandlingStateMachine.isRunning())
        m_InterrupthandlingStateMachine.start();
}

void cSfcModuleMeasProgram::resourceManagerConnect()
{
    // first we try to get a connection to resource manager over proxy
    m_rmClient = Zera::Proxy::getInstance()->getConnectionSmart(m_pModule->getNetworkConfig()->m_rmServiceConnectionInfo,
                                                                m_pModule->getNetworkConfig()->m_tcpNetworkFactory);
    // and then we set connection resource manager interface's connection
    m_rmInterface.setClientSmart(m_rmClient);
    resourceManagerConnectState.addTransition(m_rmClient.get(), &Zera::ProxyClient::connected, &m_IdentifyState);
    connect(&m_rmInterface, &AbstractServerInterface::serverAnswer, this, &cSfcModuleMeasProgram::catchInterfaceAnswer);
    // todo insert timer for timeout and/or connect error conditions
    Zera::Proxy::getInstance()->startConnectionSmart(m_rmClient);
}

void cSfcModuleMeasProgram::sendRMIdent()
{
    m_MsgNrCmdList[m_rmInterface.rmIdent(QString("SfcModule%1").arg(m_pModule->getModuleNr()))] = sendrmident;
}

void cSfcModuleMeasProgram::testSEC1Resource()
{
    m_MsgNrCmdList[m_rmInterface.getResourceTypes()] = testsec1resource;
}

void cSfcModuleMeasProgram::setECResource()
{
    m_MsgNrCmdList[m_rmInterface.setResource("SEC1", "ECALCULATOR", 1)] = setecresource;
}

void cSfcModuleMeasProgram::readResources()
{
    m_nIt = 0; // we want to read all resources from resourcetypelist
    emit activationContinue();
}

void cSfcModuleMeasProgram::readResource()
{
    QString resourcetype = m_resourceTypeList.getResourceTypeList().at(m_nIt);
    m_MsgNrCmdList[m_rmInterface.getResources(resourcetype)] = readresource;
}

void cSfcModuleMeasProgram::testSecInputs()
{
    const auto dutInplist = getConfData()->m_dutInpList;
    QStringList resourceTypeList = m_resourceTypeList.getResourceTypeList();
    qint32 dutInputCountLeftToCheck = dutInplist.count();
    for (int dutInputNo = 0; dutInputNo < dutInplist.count(); dutInputNo++) {
        QString dutInputName = dutInplist[dutInputNo];
        for (int resourceTypeNo = 0; resourceTypeNo < resourceTypeList.count(); resourceTypeNo++) {
            QString resourcelist = m_ResourceHash[resourceTypeList[resourceTypeNo]];
            if (resourcelist.contains(dutInputName)) {
                dutInputCountLeftToCheck--;
                m_dutInputDictionary.addReferenceInput(dutInputName, resourceTypeList[resourceTypeNo]);
                break;
            }
        }
    }

    if (dutInputCountLeftToCheck == 0) // we found all our configured Inputs
        emit activationContinue(); // so lets go on
    else
        notifyError(resourceErrMsg);
}

void cSfcModuleMeasProgram::ecalcServerConnect()
{
    // we try to get a connection to ecalc server over proxy
    m_pSECClient = Zera::Proxy::getInstance()->getConnection(m_pModule->getNetworkConfig()->m_secServiceConnectionInfo,
                                                             m_pModule->getNetworkConfig()->m_tcpNetworkFactory);
    // and then we set ecalcalculator interface's connection
    m_pSECInterface->setClient(m_pSECClient); //
    m_ecalcServerConnectState.addTransition(m_pSECClient, &Zera::ProxyClient::connected, &m_fetchECalcUnitsState);
    connect(m_pSECInterface.get(), &AbstractServerInterface::serverAnswer, this, &cSfcModuleMeasProgram::catchInterfaceAnswer);
    // todo insert timer for timeout and/or connect error conditions
    Zera::Proxy::getInstance()->startConnection(m_pSECClient);
}

void cSfcModuleMeasProgram::fetchECalcUnits()
{
    m_MsgNrCmdList[m_pSECInterface->setECalcUnit(2)] = fetchecalcunits; // we need 2 ecalc units to cascade
}

void cSfcModuleMeasProgram::pcbServerConnect()
{
    // we try to get a connection to ecalc server over proxy
    m_pPCBClient = Zera::Proxy::getInstance()->getConnection(m_pModule->getNetworkConfig()->m_pcbServiceConnectionInfo,
                                                             m_pModule->getNetworkConfig()->m_tcpNetworkFactory);
    // and then we set ecalcalculator interface's connection
    m_pcbInterface->setClient(m_pPCBClient); //
    m_pcbServerConnectState.addTransition(m_pPCBClient, &Zera::ProxyClient::connected, &m_readREFInputsState);
    connect(m_pcbInterface.get(), &AbstractServerInterface::serverAnswer, this, &cSfcModuleMeasProgram::catchInterfaceAnswer);
    // todo insert timer for timeout and/or connect error conditions
    Zera::Proxy::getInstance()->startConnection(m_pPCBClient);
}

void cSfcModuleMeasProgram::readREFInputs()
{
    emit activationContinue();
}

void cSfcModuleMeasProgram::readREFInputAlias()
{
    emit activationContinue();
}

void cSfcModuleMeasProgram::readREFInputDone()
{
    emit activationContinue();
}

void cSfcModuleMeasProgram::readDUTInputs()
{
    m_sItList = m_dutInputDictionary.getInputNameList();
    emit activationContinue();
}

void cSfcModuleMeasProgram::readDUTInputAlias()
{
    m_sIt = m_sItList.takeFirst();
    m_MsgNrCmdList[m_pcbInterface->resourceAliasQuery(m_dutInputDictionary.getResource(m_sIt), m_sIt)] = readdutInputalias;
}

void cSfcModuleMeasProgram::readDUTInputDone()
{
    if (m_sItList.isEmpty())
        emit activationContinue();
    else
        emit activationLoop();
}

void cSfcModuleMeasProgram::setpcbREFConstantNotifier()
{
    emit activationContinue();
}

void cSfcModuleMeasProgram::setsecINTNotifier()
{
    m_MsgNrCmdList[m_pSECInterface->registerNotifier(QString("ECAL:%1:R%2?").arg(m_masterErrCalcName).arg(ECALCREG::INTREG))] = setsecintnotifier;
}

void cSfcModuleMeasProgram::activationDone()
{
    connect(m_pStartStopPar, &VfModuleParameter::sigValueChanged, this, &cSfcModuleMeasProgram::onStartStopChanged);
    m_bActive = true;
    emit activated();
}

void cSfcModuleMeasProgram::catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer)
{
    bool ok;

    if (msgnr == 0) // 0 was reserved for async. messages
    {
        QString sintnr;
        sintnr = answer.toString().section(':', 1, 1);
        int service = sintnr.toInt(&ok);
        switch (service)
        {
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
            {
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
            }

            case fetchecalcunits:
            {
                QStringList sl;
                sl = answer.toString().split(';');
                if ((reply == ack) && (sl.length() >= 2)) {
                    m_masterErrCalcName = sl.at(0);
                    m_slaveErrCalcName = sl.at(1);
                    emit activationContinue();
                }
                else
                    notifyError(fetchsececalcunitErrMsg);
                break;
            }

            case readdutInputalias:
            {
                if (reply == ack) {
                    m_dutInputDictionary.setAlias(m_sIt, answer.toString());
                    emit activationContinue();
                }
                else
                    notifyError(readaliasErrMsg);
                break;
            }
            case setsecintnotifier:
                if (reply == ack)
                    emit activationContinue();
                else
                    notifyError(registerpcbnotifierErrMsg);
                break;
            case readintregister:
                if (reply == ack)
                {
                    m_nIntReg = answer.toInt(&ok) & 7;
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
            case stopmeas:
                if (reply == ack) {
                    emit deactivationContinue();
                }
                else
                    notifyError(stopmeasErrMsg);
                break;
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
            }
        }
    }
}

void cSfcModuleMeasProgram::generateVeinInterface()
{
    QString key;
    m_pStartStopPar = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->m_pModuleValidator,
                                         key = QString("PAR_StartStop"),
                                         QString("Start/stop measurement (start=1, stop=0)"),
                                         QVariant((int)0));
    m_pStartStopPar->setValidator(new cIntValidator(0,1,1));
    m_pStartStopPar->setScpiInfo("CALCULATE", "START",
                                 SCPI::isCmdwP | SCPI::isQuery,
                                 m_pStartStopPar->getName());
    m_pModule->m_veinModuleParameterMap[key] = m_pStartStopPar;

    m_pFlankCountAct = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->m_pModuleValidator,
                                             key = QString("ACT_FlankCount"),
                                             QString("Measurement of flank count"),
                                             QVariant((int)0));
    m_pFlankCountAct->setScpiInfo("CALCULATE", "FLANKCOUNT",
                                  SCPI::isQuery,
                                  m_pFlankCountAct->getName());
    m_pModule->m_veinModuleParameterMap[key] = m_pFlankCountAct;

    m_pLedStateAct = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->m_pModuleValidator,
                                             key = QString("ACT_LedState"),
                                             QString("Current state of scanning head LED (on=1, off=0)"),
                                             QVariant((int)0));
    m_pLedStateAct->setScpiInfo("CALCULATE", "LEDSTATUS",
                                SCPI::isQuery,
                                m_pLedStateAct->getName());
    m_pModule->m_veinModuleParameterMap[key] = m_pLedStateAct;

    m_pLedInitialStateAct = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->m_pModuleValidator,
                                           key = QString("ACT_InitialLedState"),
                                           QString("Initial state of scanning head LED (on=1, off=0)"),
                                           QVariant((int)0));
    m_pLedInitialStateAct->setScpiInfo("CALCULATE", "LEDSTATUS",
                                       SCPI::isQuery,
                                       m_pLedInitialStateAct->getName());
    m_pModule->m_veinModuleParameterMap[key] = m_pLedInitialStateAct;
}

void cSfcModuleMeasProgram::deactivateMeasDone()
{
    emit deactivated();
}

void cSfcModuleMeasProgram::onStartStopChanged(QVariant newValue)
{
    qInfo("kick off ec flank count thingy");
}

void cSfcModuleMeasProgram::readIntRegister()
{
    m_MsgNrCmdList[m_pSECInterface->readRegister(m_masterErrCalcName, ECALCREG::INTREG)] = readintregister;
}

void cSfcModuleMeasProgram::resetIntRegister()
{
    m_MsgNrCmdList[m_pSECInterface->intAck(m_masterErrCalcName, 0xF)] = resetintregister; // we reset all here
}

void cSfcModuleMeasProgram::readMTCountact()
{
    interruptContinue();
}

void cSfcModuleMeasProgram::setECResult()
{
    interruptContinue();
}

void cSfcModuleMeasProgram::setRating()
{
    interruptContinue();
}

void cSfcModuleMeasProgram::setECResultAndResetInt()
{
    interruptContinue();
}

void cSfcModuleMeasProgram::checkForRestart()
{
    interruptContinue();
}

void cSfcModuleMeasProgram::stopECCalculator()
{
    m_MsgNrCmdList[m_pSECInterface->stop(m_masterErrCalcName)] = stopmeas;

}

void cSfcModuleMeasProgram::freeECalculator()
{
    m_bActive = false;
    m_MsgNrCmdList[m_pSECInterface->freeECalcUnits()] = freeecalcunits;
}

void cSfcModuleMeasProgram::freeECResource()
{
    m_MsgNrCmdList[m_rmInterface.freeResource("SEC1", "ECALCULATOR")] = freeecresource;
}

void cSfcModuleMeasProgram::deactivationDone()
{
    disconnect(&m_rmInterface, 0, this, 0);
    disconnect(m_pSECInterface.get(), 0, this, 0);
    disconnect(m_pcbInterface.get(), 0, this, 0);

    emit deactivateMeasDone();
}

}
