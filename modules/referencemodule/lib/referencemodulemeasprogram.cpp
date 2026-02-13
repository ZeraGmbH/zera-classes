#include "referencemodulemeasprogram.h"
#include "referencemodule.h"
#include "referencemeaschannel.h"
#include "referencemoduleconfiguration.h"
#include <errormessages.h>
#include <proxy.h>
#include <proxyclient.h>
#include <reply.h>
#include <timerfactoryqt.h>

namespace REFERENCEMODULE
{

cReferenceModuleMeasProgram::cReferenceModuleMeasProgram(cReferenceModule* module, std::shared_ptr<BaseModuleConfiguration> pConfiguration) :
    cBaseDspMeasProgram(pConfiguration, module->getVeinModuleName()),
    m_pModule(module)
{
    m_ChannelList = getConfData()->m_referenceChannelList;
    m_dspInterface = m_pModule->getServiceInterfaceFactory()->createDspInterfaceRefProg(
        m_pModule->getEntityId(),
        m_ChannelList);

    m_IdentifyState.addTransition(this, &cReferenceModuleMeasProgram::activationContinue, &m_dspserverConnectState);
    m_var2DSPState.addTransition(this, &cReferenceModuleMeasProgram::activationContinue, &m_cmd2DSPState);
    m_cmd2DSPState.addTransition(this, &cReferenceModuleMeasProgram::activationContinue, &m_activateDSPState);
    m_activateDSPState.addTransition(this, &cReferenceModuleMeasProgram::activationContinue, &m_loadDSPDoneState);

    m_activationMachine.addState(&m_resourceManagerConnectState);
    m_activationMachine.addState(&m_IdentifyState);
    m_activationMachine.addState(&m_dspserverConnectState);
    m_activationMachine.addState(&m_var2DSPState);
    m_activationMachine.addState(&m_cmd2DSPState);
    m_activationMachine.addState(&m_activateDSPState);
    m_activationMachine.addState(&m_loadDSPDoneState);

    m_activationMachine.setInitialState(&m_resourceManagerConnectState);

    connect(&m_resourceManagerConnectState, &QState::entered, this, &cReferenceModuleMeasProgram::resourceManagerConnect);
    connect(&m_IdentifyState, &QState::entered, this, &cReferenceModuleMeasProgram::sendRMIdent);
    connect(&m_dspserverConnectState, &QState::entered, this, &cReferenceModuleMeasProgram::dspserverConnect);
    connect(&m_var2DSPState, &QState::entered, this, &cReferenceModuleMeasProgram::varList2DSP);
    connect(&m_cmd2DSPState, &QState::entered, this, &cReferenceModuleMeasProgram::cmdList2DSP);
    connect(&m_activateDSPState, &QState::entered, this, &cReferenceModuleMeasProgram::activateDSP);
    connect(&m_loadDSPDoneState, &QState::entered, this, &cReferenceModuleMeasProgram::activateDSPdone);

    m_deactivationMachine.addState(&m_unloadDSPDoneState);

    m_deactivationMachine.setInitialState(&m_unloadDSPDoneState);

    connect(&m_unloadDSPDoneState, &QState::entered, this, &cReferenceModuleMeasProgram::deactivateDSPdone);

    // setting up statemachine for data acquisition
    m_dataAcquisitionState.addTransition(this, &cReferenceModuleMeasProgram::dataAquisitionContinue, &m_dataAcquisitionDoneState);
    m_dataAcquisitionMachine.addState(&m_dataAcquisitionState);
    m_dataAcquisitionMachine.addState(&m_dataAcquisitionDoneState);
    m_dataAcquisitionMachine.setInitialState(&m_dataAcquisitionState);
    connect(&m_dataAcquisitionState, &QState::entered, this, &cReferenceModuleMeasProgram::dataAcquisitionDSP);
    connect(&m_dataAcquisitionDoneState, &QState::entered, this, &cReferenceModuleMeasProgram::dataReadDSP);

    if(m_pModule->getDemo()) {
        m_demoPeriodicTimer = TimerFactoryQt::createPeriodic(500);
        connect(m_demoPeriodicTimer.get(), &TimerTemplateQt::sigExpired,this, &cReferenceModuleMeasProgram::handleDemoActualValues);
        m_demoPeriodicTimer->start();
    }
}

void cReferenceModuleMeasProgram::start()
{
}

void cReferenceModuleMeasProgram::stop()
{
}


void cReferenceModuleMeasProgram::generateVeinInterface()
{
    // we have no interface
}


void cReferenceModuleMeasProgram::setDspVarList()
{
    int samples = m_pModule->getSharedChannelRangeObserver()->getSamplesPerPeriod();
    // we fetch a handle for sampled data and other temporary values
    m_pTmpDataDsp = m_dspInterface->getMemHandle("TmpData");
    m_pTmpDataDsp->addDspVar("MEASSIGNAL", samples, DSPDATA::vDspTemp);
    m_pTmpDataDsp->addDspVar("TISTART",1, DSPDATA::vDspTemp, DSPDATA::dInt);
    m_pTmpDataDsp->addDspVar("VALXDFT",m_ChannelList.count()*2, DSPDATA::vDspTemp);
    m_pTmpDataDsp->addDspVar("FILTER",2*2*m_ChannelList.count(),DSPDATA::vDspTemp);
    m_pTmpDataDsp->addDspVar("N",1,DSPDATA::vDspTemp);

    // a handle for parameter
    m_pParameterDSP =  m_dspInterface->getMemHandle("Parameter");
    m_pParameterDSP->addDspVar("TIPAR",1, DSPDATA::vDspParam, DSPDATA::dInt); // integrationtime res = 1ms

    // and one for filtered actual values
    m_pActualValuesDSP = m_dspInterface->getMemHandle("ActualValues");
    m_pActualValuesDSP->addDspVar("VALXDFTF",2*m_ChannelList.count(), DSPDATA::vDspResult);

    m_ModuleActualValues.resize(m_pActualValuesDSP->getSize()); // we provide a vector for generated actual values
    m_nDspMemUsed = m_pTmpDataDsp->getSize() + m_pParameterDSP->getSize() + m_pActualValuesDSP->getSize();
}

void cReferenceModuleMeasProgram::setDspCmdList()
{
    int samples = m_pModule->getSharedChannelRangeObserver()->getSamplesPerPeriod();
    m_dspInterface->addCycListItem("STARTCHAIN(1,1,0x0101)"); // aktiv, prozessnr. (dummy),hauptkette 1 subkette 1 start
        m_dspInterface->addCycListItem(QString("CLEARN(%1,MEASSIGNAL)").arg(samples) ); // clear meassignal
        m_dspInterface->addCycListItem(QString("CLEARN(%1,FILTER)").arg(2*2*m_ChannelList.count()+1) ); // clear the whole filter incl. count
        m_dspInterface->addCycListItem(QString("SETVAL(TIPAR,%1)").arg(getConfData()->m_fMeasInterval*1000.0)); // initial ti time  /* todo variabel */
        m_dspInterface->addCycListItem("GETSTIME(TISTART)"); // einmal ti start setzen
        m_dspInterface->addCycListItem("DEACTIVATECHAIN(1,0x0101)"); // ende prozessnr., hauptkette 1 subkette 1
    m_dspInterface->addCycListItem("STOPCHAIN(1,0x0101)"); // ende prozessnr., hauptkette 1 subkette 1

    // we compute or copy our wanted actual values
    for (int i = 0; i < m_ChannelList.count(); i++) {
        cReferenceMeasChannel* mchn = m_pModule->getMeasChannel(m_ChannelList.at(i));
        m_dspInterface->addCycListItem(QString("COPYDATA(CH%1,0,MEASSIGNAL)").arg(mchn->getDSPChannelNr())); // for each channel we work on
        m_dspInterface->addCycListItem(QString("DFT(0,MEASSIGNAL,VALXDFT+%1)").arg(2*i));
    }

    // and filter them
    m_dspInterface->addCycListItem(QString("AVERAGE1(%1,VALXDFT,FILTER)").arg(2*m_ChannelList.count())); // we add results to filter

    m_dspInterface->addCycListItem("TESTTIMESKIPNEX(TISTART,TIPAR)");
    m_dspInterface->addCycListItem("ACTIVATECHAIN(1,0x0102)");

    m_dspInterface->addCycListItem("STARTCHAIN(0,1,0x0102)");
        m_dspInterface->addCycListItem("GETSTIME(TISTART)" ); // set new system time
        m_dspInterface->addCycListItem(QString("CMPAVERAGE1(%1,FILTER,VALXDFTF)").arg(2*m_ChannelList.count()) );
        m_dspInterface->addCycListItem(QString("CLEARN(%1,FILTER)").arg(2*2*m_ChannelList.count()+1) );
        m_dspInterface->addCycListItem(QString("DSPINTTRIGGER(0x0,0x%1)").arg(irqNr) ); // send interrupt to module
        m_dspInterface->addCycListItem("DEACTIVATECHAIN(1,0x0102)" );
    m_dspInterface->addCycListItem("STOPCHAIN(1,0x0102)"); // end processnr., mainchain 1 subchain 2
}


void cReferenceModuleMeasProgram::deleteDspCmdList()
{
    m_dspInterface->clearCmdList();
}


void cReferenceModuleMeasProgram::catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer)
{
    bool ok;

    if (msgnr == 0) // 0 was reserved for async. messages
    {
        QString sintnr;
        sintnr = answer.toString().section(':', 1, 1);
        int service = sintnr.toInt(&ok);
        switch (service)
        {
        case irqNr:
            // we got an interrupt from our cmd chain and have to fetch our actual values
            if (m_bActive && !m_dataAcquisitionMachine.isRunning()) // in case of deactivation in progress, no dataaquisition
                m_dataAcquisitionMachine.start();

            break;
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

            case varlist2dsp:
                if (reply == ack)
                    emit activationContinue();
                else
                    notifyError(dspvarlistwriteErrMsg);
                break;
            case cmdlist2dsp:
                if (reply == ack)
                    emit activationContinue();
                else
                    notifyError(dspcmdlistwriteErrMsg);
                break;
            case activatedsp:
                if (reply == ack)
                    emit activationContinue();
                else
                    notifyError(dspactiveErrMsg);
                break;

            case deactivatedsp:
                if (reply == ack)
                    emit deactivationContinue();
                else
                    notifyError(dspdeactiveErrMsg);
                break;

            case dataaquistion:
                if (reply == ack)
                    emit dataAquisitionContinue();
                else {
                    m_dataAcquisitionMachine.stop();
                    notifyError(dataaquisitionErrMsg);
                }
                break;
            }
        }
    }
}

cReferenceModuleConfigData *cReferenceModuleMeasProgram::getConfData()
{
    return qobject_cast<cReferenceModuleConfiguration*>(m_pConfiguration.get())->getConfigurationData();
}

void cReferenceModuleMeasProgram::resourceManagerConnect()
{
    // first we try to get a connection to resource manager over proxy
    m_rmClient = Zera::Proxy::getInstance()->getConnectionSmart(m_pModule->getNetworkConfig()->m_rmServiceConnectionInfo,
                                                                m_pModule->getNetworkConfig()->m_tcpNetworkFactory);
    // and then we set connection resource manager interface's connection
    m_rmInterface.setClientSmart(m_rmClient); //
    m_resourceManagerConnectState.addTransition(m_rmClient.get(), &Zera::ProxyClient::connected, &m_IdentifyState);
    connect(&m_rmInterface, &AbstractServerInterface::serverAnswer, this, &cReferenceModuleMeasProgram::catchInterfaceAnswer);
    // todo insert timer for timeout and/or connect error conditions
    Zera::Proxy::getInstance()->startConnectionSmart(m_rmClient);
}

void cReferenceModuleMeasProgram::sendRMIdent()
{
    m_MsgNrCmdList[m_rmInterface.rmIdent(QString("ReferenceModule%1").arg(m_pModule->getModuleNr()))] = sendrmident;
}

void cReferenceModuleMeasProgram::dspserverConnect()
{
    m_dspClient = Zera::Proxy::getInstance()->getConnectionSmart(m_pModule->getNetworkConfig()->m_dspServiceConnectionInfo,
                                                                 m_pModule->getNetworkConfig()->m_tcpNetworkFactory);
    m_dspInterface->setClientSmart(m_dspClient);
    m_dspserverConnectState.addTransition(m_dspClient.get(), &Zera::ProxyClient::connected, &m_var2DSPState);
    connect(m_dspInterface.get(), &AbstractServerInterface::serverAnswer, this, &cReferenceModuleMeasProgram::catchInterfaceAnswer);
    Zera::Proxy::getInstance()->startConnectionSmart(m_dspClient);
}

void cReferenceModuleMeasProgram::varList2DSP()
{
    setDspVarList(); // first we set the var list for our dsp
    setDspCmdList(); // and the cmd list he has to work on
    m_MsgNrCmdList[m_dspInterface->varList2Dsp()] = varlist2dsp;
}

void cReferenceModuleMeasProgram::cmdList2DSP()
{
    m_MsgNrCmdList[m_dspInterface->cmdList2Dsp()] = cmdlist2dsp;
}

void cReferenceModuleMeasProgram::activateDSP()
{
    m_MsgNrCmdList[m_dspInterface->activateInterface()] = activatedsp; // aktiviert die var- und cmd-listen im dsp
}

void cReferenceModuleMeasProgram::activateDSPdone()
{
    m_bActive = true;
    emit activated();
}

void cReferenceModuleMeasProgram::deactivateDSPdone()
{
    disconnect(&m_rmInterface, 0, this, 0);
    disconnect(m_dspInterface.get(), 0, this, 0);
    emit deactivated();
}

void cReferenceModuleMeasProgram::dataAcquisitionDSP()
{
    if (m_bActive)
        m_MsgNrCmdList[m_dspInterface->dataAcquisition(m_pActualValuesDSP)] = dataaquistion; // we start our data aquisition now
}

void cReferenceModuleMeasProgram::dataReadDSP()
{
    if (m_bActive) {
        m_ModuleActualValues = m_pActualValuesDSP->getData();
        emit actualValues(&m_ModuleActualValues); // and send them
    }
}

void cReferenceModuleMeasProgram::handleDemoActualValues()
{
    QVector<float> demoValues;

    for (int i = 0; i < m_ChannelList.count(); i++) {
        float val = (float)rand() / RAND_MAX ;
        demoValues.append(val);
    }
    m_ModuleActualValues = demoValues;
    //Q_ASSERT(demoValues.size() == m_ModuleActualValues.size());
    emit actualValues(&m_ModuleActualValues);

}

}
