#include "samplemodulemeasprogram.h"
#include "samplemoduleconfiguration.h"
#include "samplemoduleconfigdata.h"
#include "samplemodule.h"
#include "pllmeaschannel.h"
#include <errormessages.h>
#include <reply.h>
#include <proxy.h>
#include <timerfactoryqt.h>

namespace SAMPLEMODULE
{

cSampleModuleMeasProgram::cSampleModuleMeasProgram(cSampleModule* module, std::shared_ptr<cBaseModuleConfiguration> pConfiguration) :
    cBaseDspMeasProgram(pConfiguration), m_pModule(module)
{
    m_dspInterface = m_pModule->getServiceInterfaceFactory()->createDspInterfaceOther();

    m_ChannelList = getConfData()->m_ObsermaticConfPar.m_pllChannelList;

    m_IdentifyState.addTransition(this, &cSampleModuleMeasProgram::activationContinue, &m_dspserverConnectState);
    m_claimPGRMemState.addTransition(this, &cSampleModuleMeasProgram::activationContinue, &m_claimUSERMemState);
    m_claimUSERMemState.addTransition(this, &cSampleModuleMeasProgram::activationContinue, &m_var2DSPState);
    m_var2DSPState.addTransition(this, &cSampleModuleMeasProgram::activationContinue, &m_cmd2DSPState);
    m_cmd2DSPState.addTransition(this, &cSampleModuleMeasProgram::activationContinue, &m_activateDSPState);
    m_activateDSPState.addTransition(this, &cSampleModuleMeasProgram::activationContinue, &m_loadDSPDoneState);

    m_activationMachine.addState(&resourceManagerConnectState);
    m_activationMachine.addState(&m_IdentifyState);
    m_activationMachine.addState(&m_dspserverConnectState);
    m_activationMachine.addState(&m_claimPGRMemState);
    m_activationMachine.addState(&m_claimUSERMemState);
    m_activationMachine.addState(&m_var2DSPState);
    m_activationMachine.addState(&m_cmd2DSPState);
    m_activationMachine.addState(&m_activateDSPState);
    m_activationMachine.addState(&m_loadDSPDoneState);

    m_activationMachine.setInitialState(&resourceManagerConnectState);

    connect(&resourceManagerConnectState, &QState::entered, this, &cSampleModuleMeasProgram::resourceManagerConnect);
    connect(&m_IdentifyState, &QState::entered, this, &cSampleModuleMeasProgram::sendRMIdent);
    connect(&m_dspserverConnectState, &QState::entered, this, &cSampleModuleMeasProgram::dspserverConnect);
    connect(&m_claimPGRMemState, &QState::entered, this, &cSampleModuleMeasProgram::claimPGRMem);
    connect(&m_claimUSERMemState, &QState::entered, this, &cSampleModuleMeasProgram::claimUSERMem);
    connect(&m_var2DSPState, &QState::entered, this, &cSampleModuleMeasProgram::varList2DSP);
    connect(&m_cmd2DSPState, &QState::entered, this, &cSampleModuleMeasProgram::cmdList2DSP);
    connect(&m_activateDSPState, &QState::entered, this, &cSampleModuleMeasProgram::activateDSP);
    connect(&m_loadDSPDoneState, &QState::entered, this, &cSampleModuleMeasProgram::activateDSPdone);

    // setting up statemachine for unloading dsp and setting resources free
    m_deactivateDSPState.addTransition(this, &cSampleModuleMeasProgram::deactivationContinue, &m_freePGRMemState);
    m_freePGRMemState.addTransition(this, &cSampleModuleMeasProgram::deactivationContinue, &m_freeUSERMemState);
    m_freeUSERMemState.addTransition(this, &cSampleModuleMeasProgram::deactivationContinue, &m_unloadDSPDoneState);
    m_deactivationMachine.addState(&m_deactivateDSPState);
    m_deactivationMachine.addState(&m_freePGRMemState);
    m_deactivationMachine.addState(&m_freeUSERMemState);
    m_deactivationMachine.addState(&m_unloadDSPDoneState);

    m_deactivationMachine.setInitialState(&m_deactivateDSPState);

    connect(&m_deactivateDSPState, &QState::entered, this, &cSampleModuleMeasProgram::deactivateDSP);
    connect(&m_freePGRMemState, &QState::entered, this, &cSampleModuleMeasProgram::freePGRMem);
    connect(&m_freeUSERMemState, &QState::entered, this, &cSampleModuleMeasProgram::freeUSERMem);
    connect(&m_unloadDSPDoneState, &QState::entered, this, &cSampleModuleMeasProgram::deactivateDSPdone);

    // setting up statemachine for data acquisition
    m_dataAcquisitionState.addTransition(this, &cSampleModuleMeasProgram::dataAquisitionContinue, &m_dataAcquisitionDoneState);
    m_dataAcquisitionMachine.addState(&m_dataAcquisitionState);
    m_dataAcquisitionMachine.addState(&m_dataAcquisitionDoneState);
    m_dataAcquisitionMachine.setInitialState(&m_dataAcquisitionState);
    connect(&m_dataAcquisitionState, &QState::entered, this, &cSampleModuleMeasProgram::dataAcquisitionDSP);
    connect(&m_dataAcquisitionDoneState, &QState::entered, this, &cSampleModuleMeasProgram::dataReadDSP);

    if(m_pModule->getDemo()) {
        m_demoPeriodicTimer = TimerFactoryQt::createPeriodic(500);
        connect(m_demoPeriodicTimer.get(), &TimerTemplateQt::sigExpired,this, &cSampleModuleMeasProgram::handleDemoActualValues);
        m_demoPeriodicTimer->start();
    }
}


cSampleModuleMeasProgram::~cSampleModuleMeasProgram()
{
    Zera::Proxy::getInstance()->releaseConnection(m_dspClient.get());
}


void cSampleModuleMeasProgram::start()
{
}


void cSampleModuleMeasProgram::stop()
{
}


void cSampleModuleMeasProgram::generateInterface()
{
    // we don't have any interface for publication
}


void cSampleModuleMeasProgram::setDspVarList()
{

    // we fetch a handle for sampled data and other temporary values
    m_pTmpDataDsp = m_dspInterface->getMemHandle("TmpData");
    m_pTmpDataDsp->addVarItem( new cDspVar("MEASSIGNAL", m_nSamples, DSPDATA::vDspTemp));
    m_pTmpDataDsp->addVarItem( new cDspVar("TISTART",1, DSPDATA::vDspTemp, DSPDATA::dInt));
    m_pTmpDataDsp->addVarItem( new cDspVar("CHXRMS",m_ChannelList.count(), DSPDATA::vDspTemp));
    m_pTmpDataDsp->addVarItem( new cDspVar("FILTER",2 * m_ChannelList.count(),DSPDATA::vDspTemp));
    m_pTmpDataDsp->addVarItem( new cDspVar("N",1,DSPDATA::vDspTemp));

    // a handle for parameter
    m_pParameterDSP =  m_dspInterface->getMemHandle("Parameter");
    m_pParameterDSP->addVarItem( new cDspVar("TIPAR",1, DSPDATA::vDspParam, DSPDATA::dInt)); // integrationtime res = 1ms

    // and one for filtered actual values
    m_pActualValuesDSP = m_dspInterface->getMemHandle("ActualValues");
    m_pActualValuesDSP->addVarItem( new cDspVar("CHXRMSF",m_ChannelList.count(), DSPDATA::vDspResult));

    m_ModuleActualValues.resize(m_pActualValuesDSP->getSize()); // we provide a vector for generated actual values
    m_nDspMemUsed = m_pTmpDataDsp->getSize() + m_pParameterDSP->getSize() + m_pActualValuesDSP->getSize();
}


void cSampleModuleMeasProgram::deleteDspVarList()
{
    m_dspInterface->deleteMemHandle(m_pTmpDataDsp);
    m_dspInterface->deleteMemHandle(m_pParameterDSP);
    m_dspInterface->deleteMemHandle(m_pActualValuesDSP);
}


void cSampleModuleMeasProgram::setDspCmdList()
{
    QString s;

    m_dspInterface->addCycListItem( s = "STARTCHAIN(1,1,0x0101)"); // aktiv, prozessnr. (dummy),hauptkette 1 subkette 1 start
        m_dspInterface->addCycListItem( s = QString("CLEARN(%1,MEASSIGNAL)").arg(m_nSamples)); // clear meassignal
        m_dspInterface->addCycListItem( s = QString("CLEARN(%1,FILTER)").arg(2*m_ChannelList.count()+1)); // clear the whole filter incl. count
        m_dspInterface->addCycListItem( s = QString("SETVAL(TIPAR,%1)").arg(getConfData()->m_fMeasInterval*1000.0)); // initial ti time  /* todo variabel */
        m_dspInterface->addCycListItem( s = "GETSTIME(TISTART)"); // einmal ti start setzen
        m_dspInterface->addCycListItem( s = "DEACTIVATECHAIN(1,0x0101)"); // ende prozessnr., hauptkette 1 subkette 1
    m_dspInterface->addCycListItem( s = "STOPCHAIN(1,0x0101)"); // ende prozessnr., hauptkette 1 subkette 1

    // we compute or copy our wanted actual values
    for (int i = 0; i < m_ChannelList.count(); i++)
    {
        cPllMeasChannel* schn = m_pModule->getPllMeasChannel(m_ChannelList.at(i));
        m_dspInterface->addCycListItem( s = QString("COPYDATA(CH%1,0,MEASSIGNAL)").arg(schn->getDSPChannelNr())); // for each channel we work on
        m_dspInterface->addCycListItem( s = QString("RMS(MEASSIGNAL,CHXRMS+%1)").arg(i));
    }

    // and filter them
    m_dspInterface->addCycListItem( s = QString("AVERAGE1(%1,CHXRMS,FILTER)").arg(m_ChannelList.count())); // we add results to filter

    m_dspInterface->addCycListItem( s = "TESTTIMESKIPNEX(TISTART,TIPAR)");
    m_dspInterface->addCycListItem( s = "ACTIVATECHAIN(1,0x0102)");

    m_dspInterface->addCycListItem( s = "STARTCHAIN(0,1,0x0102)");
        m_dspInterface->addCycListItem( s = "GETSTIME(TISTART)"); // set new system time
        m_dspInterface->addCycListItem( s = QString("CMPAVERAGE1(%1,FILTER,CHXRMSF)").arg(m_ChannelList.count()));
        m_dspInterface->addCycListItem( s = QString("CLEARN(%1,FILTER)").arg(2*m_ChannelList.count()+1));
        m_dspInterface->addCycListItem( s = QString("DSPINTTRIGGER(0x0,0x%1)").arg(irqNr)); // send interrupt to module
        m_dspInterface->addCycListItem( s = "DEACTIVATECHAIN(1,0x0102)");
    m_dspInterface->addCycListItem( s = "STOPCHAIN(1,0x0102)"); // end processnr., mainchain 1 subchain 2
}


void cSampleModuleMeasProgram::deleteDspCmdList()
{
    m_dspInterface->clearCmdList();
}


void cSampleModuleMeasProgram::catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer)
{
    if (msgnr == 0) { // 0 was reserved for async. messages
        QString sintnr;
        sintnr = answer.toString().section(':', 1, 1);
        int service = sintnr.toInt();
        switch (service)
        {
        case irqNr:
            if (m_bActive && !m_dataAcquisitionMachine.isRunning()) // in case of deactivation in progress, no dataaquisition
                m_dataAcquisitionMachine.start();
            break;
        }
    }
    else {
        if (m_MsgNrCmdList.contains(msgnr)) {
            int cmd = m_MsgNrCmdList.take(msgnr);
            switch (cmd)
            {
            case sendrmidentsample:
                if (reply == ack) // we only continue if resource manager acknowledges
                    emit activationContinue();
                else {
                    emit errMsg((tr(rmidentErrMSG)));
                    emit activationError();
                }
                break;
            case claimpgrmem:
                if (reply == ack) // we only continue if resource manager acknowledges
                    emit activationContinue();
                else {
                    emit errMsg((tr(claimresourceErrMsg)));
                    emit activationError();
                }
                break;
            case claimusermem:
                if (reply == ack) // we only continue if resource manager acknowledges
                    emit activationContinue();
                else {
                    emit errMsg((tr(claimresourceErrMsg)));
                    emit activationError();
                }
                break;
            case varlist2dsp:
                if (reply == ack) // we only continue if resource manager acknowledges
                    emit activationContinue();
                else
                {
                    emit errMsg((tr(dspvarlistwriteErrMsg)));
                    emit activationError();
                }
                break;
            case cmdlist2dsp:
                if (reply == ack) // we only continue if resource manager acknowledges
                    emit activationContinue();
                else
                {
                    emit errMsg((tr(dspcmdlistwriteErrMsg)));
                    emit activationError();
                }
                break;
            case activatedsp:
                if (reply == ack) // we only continue if resource manager acknowledges
                    emit activationContinue();
                else
                {
                    emit errMsg((tr(dspactiveErrMsg)));
                    emit activationError();
                }
                break;

            case deactivatedsp:
                if (reply == ack) // we only continue if resource manager acknowledges
                    emit deactivationContinue();
                else
                {
                    emit errMsg((tr(dspdeactiveErrMsg)));
                    emit deactivationError();
                }
                break;
            case freepgrmem:
                if (reply == ack) // we only continue if resource manager acknowledges
                    emit deactivationContinue();
                else
                {
                    emit errMsg((tr(freeresourceErrMsg)));
                    emit deactivationError();
                }
                break;
            case freeusermem:
                if (reply == ack) // we only continue if resource manager acknowledges
                    emit deactivationContinue();
                else
                {
                    emit errMsg((tr(freeresourceErrMsg)));
                    emit deactivationError();
                }
                break;

            case dataaquistion:
                if (reply == ack)
                    emit dataAquisitionContinue();
                else
                {
                    m_dataAcquisitionMachine.stop();
                    // perhaps we emit some error here ?
                    {
                        emit errMsg((tr(dataaquisitionErrMsg)));
                        emit executionError();
                    }
                }
                break;
            }
        }
    }
}

cSampleModuleConfigData *cSampleModuleMeasProgram::getConfData()
{
    return qobject_cast<cSampleModuleConfiguration*>(m_pConfiguration.get())->getConfigurationData();
}


void cSampleModuleMeasProgram::setInterfaceActualValues(QVector<float>)
{
    // we have no interface ......
}

void cSampleModuleMeasProgram::handleDemoActualValues()
{
    QVector<float> demoValues;
    int channelCount = getConfData()->m_ObsermaticConfPar.m_npllChannelCount;

    for (int i = 0; i < channelCount; i++) {
        float val = (float)rand() / RAND_MAX ;
        demoValues.append(val);
    }
    m_ModuleActualValues = demoValues;
    Q_ASSERT(demoValues.size() == m_ModuleActualValues.size());
    emit actualValues(&m_ModuleActualValues);
}


void cSampleModuleMeasProgram::resourceManagerConnect()
{
    // first we try to get a connection to resource manager over proxy
    m_rmClient = Zera::Proxy::getInstance()->getConnectionSmart(m_pModule->getNetworkConfig()->m_rmServiceConnectionInfo.m_sIP,
                                                                m_pModule->getNetworkConfig()->m_rmServiceConnectionInfo.m_nPort,
                                                                m_pModule->getNetworkConfig()->m_tcpNetworkFactory);
    // and then we set connection resource manager interface's connection
    m_rmInterface.setClientSmart(m_rmClient); //
    resourceManagerConnectState.addTransition(m_rmClient.get(), &Zera::ProxyClient::connected, &m_IdentifyState);
    connect(&m_rmInterface, &Zera::cRMInterface::serverAnswer, this, &cSampleModuleMeasProgram::catchInterfaceAnswer);
    // todo insert timer for timeout and/or connect error conditions
    Zera::Proxy::getInstance()->startConnectionSmart(m_rmClient);
}


void cSampleModuleMeasProgram::sendRMIdent()
{
    m_MsgNrCmdList[m_rmInterface.rmIdent(QString("SampleModule%1").arg(m_pModule->getModuleNr()))] = sendrmidentsample;
}


void cSampleModuleMeasProgram::dspserverConnect()
{
    m_dspClient = Zera::Proxy::getInstance()->getConnectionSmart(m_pModule->getNetworkConfig()->m_dspServiceConnectionInfo.m_sIP,
                                                                 m_pModule->getNetworkConfig()->m_dspServiceConnectionInfo.m_nPort,
                                                                 m_pModule->getNetworkConfig()->m_tcpNetworkFactory);
    m_dspInterface->setClientSmart(m_dspClient);
    m_dspserverConnectState.addTransition(m_dspClient.get(), &Zera::ProxyClient::connected, &m_claimPGRMemState);
    connect(m_dspInterface.get(), &Zera::cDSPInterface::serverAnswer, this, &cSampleModuleMeasProgram::catchInterfaceAnswer);
    Zera::Proxy::getInstance()->startConnectionSmart(m_dspClient);
}


void cSampleModuleMeasProgram::claimPGRMem()
{
    m_nSamples = m_pModule->getPllMeasChannel(m_ChannelList.at(0))->getSampleRate(); // we first read the sample nr from first channel
    setDspVarList(); // first we set the var list for our dsp
    setDspCmdList(); // and the cmd list he has to work on

    m_MsgNrCmdList[m_rmInterface.setResource("DSP1", "PGRMEMC", m_dspInterface->cmdListCount())] = claimpgrmem;
}


void cSampleModuleMeasProgram::claimUSERMem()
{
   m_MsgNrCmdList[m_rmInterface.setResource("DSP1", "USERMEM", m_nDspMemUsed)] = claimusermem;
}


void cSampleModuleMeasProgram::varList2DSP()
{
    m_MsgNrCmdList[m_dspInterface->varList2Dsp()] = varlist2dsp;
}


void cSampleModuleMeasProgram::cmdList2DSP()
{
    m_MsgNrCmdList[m_dspInterface->cmdList2Dsp()] = cmdlist2dsp;
}


void cSampleModuleMeasProgram::activateDSP()
{
    m_MsgNrCmdList[m_dspInterface->activateInterface()] = activatedsp; // aktiviert die var- und cmd-listen im dsp
}


void cSampleModuleMeasProgram::activateDSPdone()
{
    m_bActive = true;
    emit activated();
}


void cSampleModuleMeasProgram::deactivateDSP()
{
    m_bActive = false;
    m_MsgNrCmdList[m_dspInterface->deactivateInterface()] = deactivatedsp; // wat wohl
}


void cSampleModuleMeasProgram::freePGRMem()
{
    // we always destroy the whole interface even in case of new configuration while running
    // so the list are gone anyway
    m_MsgNrCmdList[m_rmInterface.freeResource("DSP1", "PGRMEMC")] = freepgrmem;
}


void cSampleModuleMeasProgram::freeUSERMem()
{
    m_MsgNrCmdList[m_rmInterface.freeResource("DSP1", "USERMEM")] = freeusermem;
}


void cSampleModuleMeasProgram::deactivateDSPdone()
{
    disconnect(&m_rmInterface, 0, this, 0);
    disconnect(m_dspInterface.get(), 0, this, 0);
    emit deactivated();
}


void cSampleModuleMeasProgram::dataAcquisitionDSP()
{
    m_MsgNrCmdList[m_dspInterface->dataAcquisition(m_pActualValuesDSP)] = dataaquistion; // we start our data aquisition now
}


void cSampleModuleMeasProgram::dataReadDSP()
{
    if (m_bActive) {
        m_ModuleActualValues = m_pActualValuesDSP->getData();
        emit actualValues(&m_ModuleActualValues); // and send them
    }
}

}
