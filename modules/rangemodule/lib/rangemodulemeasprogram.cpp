#include "rangemodulemeasprogram.h"
#include "rangemodule.h"
#include "rangemeaschannel.h"
#include "rangemoduleconfiguration.h"
#include "rangemoduleconfigdata.h"
#include <errormessages.h>
#include <reply.h>
#include <proxy.h>
#include <timerfactoryqt.h>

namespace RANGEMODULE
{

cRangeModuleMeasProgram::cRangeModuleMeasProgram(cRangeModule* module, std::shared_ptr<cBaseModuleConfiguration> pConfiguration) :
    cBaseDspMeasProgram(pConfiguration),
    m_pModule(module)
{
    // we have to instantiate a working resource manager and dspserver interface
    m_pDSPInterFace = new Zera::cDSPInterface();

    m_bRanging = false;
    m_bIgnore = false;
    m_ChannelList = getConfData()->m_senseChannelList;

    m_IdentifyState.addTransition(this, &cRangeModuleMeasProgram::activationContinue, &m_dspserverConnectState);
    m_claimPGRMemState.addTransition(this, &cRangeModuleMeasProgram::activationContinue, &m_claimUSERMemState);
    m_claimUSERMemState.addTransition(this, &cRangeModuleMeasProgram::activationContinue, &m_var2DSPState);
    m_var2DSPState.addTransition(this, &cRangeModuleMeasProgram::activationContinue, &m_cmd2DSPState);
    m_cmd2DSPState.addTransition(this, &cRangeModuleMeasProgram::activationContinue, &m_activateDSPState);
    m_activateDSPState.addTransition(this, &cRangeModuleMeasProgram::activationContinue, &m_loadDSPDoneState);

    m_activationMachine.addState(&resourceManagerConnectState);
    m_activationMachine.addState(&m_IdentifyState);
    m_activationMachine.addState(&m_dspserverConnectState);
    m_activationMachine.addState(&m_claimPGRMemState);
    m_activationMachine.addState(&m_claimUSERMemState);
    m_activationMachine.addState(&m_var2DSPState);
    m_activationMachine.addState(&m_cmd2DSPState);
    m_activationMachine.addState(&m_activateDSPState);
    m_activationMachine.addState(&m_loadDSPDoneState);

    connect(&resourceManagerConnectState, &QState::entered, this, &cRangeModuleMeasProgram::resourceManagerConnect);
    connect(&m_IdentifyState, &QState::entered, this, &cRangeModuleMeasProgram::sendRMIdent);
    connect(&m_dspserverConnectState, &QState::entered, this, &cRangeModuleMeasProgram::dspserverConnect);
    connect(&m_claimPGRMemState, &QState::entered, this, &cRangeModuleMeasProgram::claimPGRMem);
    connect(&m_claimUSERMemState, &QState::entered, this, &cRangeModuleMeasProgram::claimUSERMem);
    connect(&m_var2DSPState, &QState::entered, this, &cRangeModuleMeasProgram::varList2DSP);
    connect(&m_cmd2DSPState, &QState::entered, this, &cRangeModuleMeasProgram::cmdList2DSP);
    connect(&m_activateDSPState, &QState::entered, this, &cRangeModuleMeasProgram::activateDSP);
    connect(&m_loadDSPDoneState, &QState::entered, this, &cRangeModuleMeasProgram::activateDSPdone);

    // setting up statemachine for unloading dsp and setting resources free
    m_deactivateDSPState.addTransition(this, &cRangeModuleMeasProgram::deactivationContinue, &m_freePGRMemState);
    m_freePGRMemState.addTransition(this, &cRangeModuleMeasProgram::deactivationContinue, &m_freeUSERMemState);
    m_freeUSERMemState.addTransition(this, &cRangeModuleMeasProgram::deactivationContinue, &m_unloadDSPDoneState);
    m_deactivationMachine.addState(&m_deactivateDSPState);
    m_deactivationMachine.addState(&m_freePGRMemState);
    m_deactivationMachine.addState(&m_freeUSERMemState);
    m_deactivationMachine.addState(&m_unloadDSPDoneState);

    connect(&m_deactivateDSPState, &QState::entered, this, &cRangeModuleMeasProgram::deactivateDSP);
    connect(&m_freePGRMemState, &QState::entered, this, &cRangeModuleMeasProgram::freePGRMem);
    connect(&m_freeUSERMemState, &QState::entered, this, &cRangeModuleMeasProgram::freeUSERMem);
    connect(&m_unloadDSPDoneState, &QState::entered, this, &cRangeModuleMeasProgram::deactivateDSPdone);

    if(!m_pModule->getDemo()) {
        m_activationMachine.setInitialState(&resourceManagerConnectState);
        m_deactivationMachine.setInitialState(&m_deactivateDSPState);
    }
    else {
        m_activationMachine.setInitialState(&m_loadDSPDoneState);
        m_deactivationMachine.setInitialState(&m_unloadDSPDoneState);
    }

    // setting up statemachine for data acquisition
    m_dataAcquisitionState.addTransition(this, &cRangeModuleMeasProgram::dataAquisitionContinue, &m_dataAcquisitionDoneState);
    m_dataAcquisitionMachine.addState(&m_dataAcquisitionState);
    m_dataAcquisitionMachine.addState(&m_dataAcquisitionDoneState);
    m_dataAcquisitionMachine.setInitialState(&m_dataAcquisitionState);
    connect(&m_dataAcquisitionState, &QState::entered, this, &cRangeModuleMeasProgram::dataAcquisitionDSP);
    connect(&m_dataAcquisitionDoneState, &QState::entered, this, &cRangeModuleMeasProgram::dataReadDSP);

    connect(&m_dspWatchdogTimer, &QTimer::timeout, this, &cRangeModuleMeasProgram::onDspWatchdogTimeout);
    if(m_pModule->getDemo()) {
        // Demo timer for dummy actual values
        m_demoPeriodicTimer = TimerFactoryQt::createPeriodic(500);
        connect(m_demoPeriodicTimer.get(), &TimerTemplateQt::sigExpired, this, &cRangeModuleMeasProgram::handleDemoPeriodicTimer);
    }
}


cRangeModuleMeasProgram::~cRangeModuleMeasProgram()
{
    delete m_pDSPInterFace;
}


void cRangeModuleMeasProgram::start()
{
    disconnect(this, &cRangeModuleMeasProgram::actualValues, this, 0);
    connect(this, &cRangeModuleMeasProgram::actualValues, this, &cRangeModuleMeasProgram::setInterfaceActualValues);
    if(m_pModule->getDemo())
        m_demoPeriodicTimer->start();
}


void cRangeModuleMeasProgram::stop()
{
    if(m_pModule->getDemo())
        m_demoPeriodicTimer->stop();
}


void cRangeModuleMeasProgram::syncRanging(QVariant sync)
{
    m_bRanging = (sync != 0); // we are ranging from the moment sync becomes 1
    m_bIgnore = (sync == 0); // when ranging has finished we ignore 1 more actual values
}


void cRangeModuleMeasProgram::generateInterface()
{
    VfModuleActvalue *pActvalue;

    for (int i = 0; i < m_ChannelList.count(); i++) {
        pActvalue = new VfModuleActvalue(m_pModule->getEntityId(), m_pModule->m_pModuleValidator,
                                            QString("ACT_Channel%1Peak").arg(i+1),
                                            QString("Actual peak value"),
                                            QVariant(0.0) );
        m_veinActValueList.append(pActvalue); // we add the component for our measurement
        m_pModule->veinModuleActvalueList.append(pActvalue); // and for the modules interface
    }

    pActvalue = new VfModuleActvalue(m_pModule->getEntityId(), m_pModule->m_pModuleValidator,
                                        QString("ACT_Frequency"),
                                        QString("Actual frequency"),
                                        QVariant(0.0) );

    pActvalue->setUnit("Hz");
    pActvalue->setSCPIInfo(new cSCPIInfo("MEASURE","F", "8", "ACT_Frequency", "0", "Hz"));

    m_veinActValueList.append(pActvalue); // we add the component for our measurement
    m_pModule->veinModuleActvalueList.append(pActvalue); // and for the modules interface

    for (int i = 0; i < m_ChannelList.count(); i++) {
        pActvalue = new VfModuleActvalue(m_pModule->getEntityId(), m_pModule->m_pModuleValidator,
                                            QString("ACT_Channel%1Rms").arg(i+1),
                                            QString("Actual RMS value"),
                                            QVariant(0.0) );
        m_veinRmsValueList.append(pActvalue); // we add the component for our measurement
        m_pModule->veinModuleActvalueList.append(pActvalue); // and for the modules interface
    }


    m_pMeasureSignal = new VfModuleComponent(m_pModule->getEntityId(), m_pModule->m_pModuleValidator,
                                                QString("SIG_Measuring"),
                                                QString("Signal indicating measurement activity"),
                                                QVariant(0) );
    m_pModule->veinModuleComponentList.append(m_pMeasureSignal); // and for the modules interface
}


void cRangeModuleMeasProgram::setDspVarList()
{

    // we fetch a handle for sampled data and other temporary values
    m_pTmpDataDsp = m_pDSPInterFace->getMemHandle("TmpData");
    m_pTmpDataDsp->addVarItem( new cDspVar("MEASSIGNAL", m_nSamples, DSPDATA::vDspTemp));
    m_pTmpDataDsp->addVarItem( new cDspVar("MAXRESET", 32, DSPDATA::vDspTemp));
    m_pTmpDataDsp->addVarItem( new cDspVar("TISTART",1, DSPDATA::vDspTemp, DSPDATA::dInt));
    m_pTmpDataDsp->addVarItem( new cDspVar("CHXPEAK",m_ChannelList.count(), DSPDATA::vDspTemp));
    m_pTmpDataDsp->addVarItem( new cDspVar("CHXRMS",m_ChannelList.count(), DSPDATA::vDspTemp));
    m_pTmpDataDsp->addVarItem( new cDspVar("FREQ", 1, DSPDATA::vDspTemp));
    m_pTmpDataDsp->addVarItem( new cDspVar("FILTER",2*(2*m_ChannelList.count()+1),DSPDATA::vDspTemp)); // filter workspace for scaled peak, rms and freq
    m_pTmpDataDsp->addVarItem( new cDspVar("N",1,DSPDATA::vDspTemp));

    // a handle for parameter
    m_pParameterDSP =  m_pDSPInterFace->getMemHandle("Parameter");
    m_pParameterDSP->addVarItem( new cDspVar("TIPAR",1, DSPDATA::vDspParam, DSPDATA::dInt)); // integrationtime res = 1ms

    // and one for filtered actual values
    m_pActualValuesDSP = m_pDSPInterFace->getMemHandle("ActualValues");
    m_pActualValuesDSP->addVarItem( new cDspVar("CHXPEAKF",m_ChannelList.count(), DSPDATA::vDspResult)); // only copied values from channels maximum from dsp workspace
    m_pActualValuesDSP->addVarItem( new cDspVar("CHXRMSF",m_ChannelList.count(), DSPDATA::vDspResult));
    m_pActualValuesDSP->addVarItem( new cDspVar("FREQF", 1, DSPDATA::vDspResult));
    m_pActualValuesDSP->addVarItem( new cDspVar("CHXRAWPEAK",m_ChannelList.count(), DSPDATA::vDspResult));

    m_ModuleActualValues.resize(m_pActualValuesDSP->getSize()); // we provide a vector for generated actual values
    m_nDspMemUsed = m_pTmpDataDsp->getSize() + m_pParameterDSP->getSize() + m_pActualValuesDSP->getSize();
}


void cRangeModuleMeasProgram::deleteDspVarList()
{
    m_pDSPInterFace->deleteMemHandle(m_pTmpDataDsp);
    m_pDSPInterFace->deleteMemHandle(m_pParameterDSP);
    m_pDSPInterFace->deleteMemHandle(m_pActualValuesDSP);
}


void cRangeModuleMeasProgram::setDspCmdList()
{
    QString s;

    m_pDSPInterFace->addCycListItem( s = "STARTCHAIN(1,1,0x0101)"); // aktiv, prozessnr. (dummy),hauptkette 1 subkette 1 start
        m_pDSPInterFace->addCycListItem( s = QString("CLEARN(%1,MEASSIGNAL)").arg(m_nSamples) ); // clear meassignal
        m_pDSPInterFace->addCycListItem( s = QString("CLEARN(%1,FILTER)").arg(2*(2*m_ChannelList.count()+1)+1) ); // clear the whole filter incl. count
        m_pDSPInterFace->addCycListItem( s = QString("SETVAL(TIPAR,%1)").arg(getConfData()->m_fMeasInterval*1000.0)); // initial ti time  /* todo variabel */
        m_pDSPInterFace->addCycListItem( s = "GETSTIME(TISTART)"); // einmal ti start setzen
        m_pDSPInterFace->addCycListItem( s = QString("CLKMODE(1)")); // clk mode auf 48bit einstellen
        m_pDSPInterFace->addCycListItem( s = "DEACTIVATECHAIN(1,0x0101)"); // ende prozessnr., hauptkette 1 subkette 1
    m_pDSPInterFace->addCycListItem( s = "STOPCHAIN(1,0x0101)"); // ende prozessnr., hauptkette 1 subkette 1

    // we compute or copy our wanted actual values
    for (int i = 0; i < m_ChannelList.count(); i++)
    {
        cRangeMeasChannel* mchn = m_pModule->getMeasChannel(m_ChannelList.at(i));
        m_pDSPInterFace->addCycListItem( s = QString("COPYDATA(CH%1,0,MEASSIGNAL)").arg(mchn->getDSPChannelNr())); // for each channel we work on
        m_pDSPInterFace->addCycListItem( s = QString("SETPEAK(MEASSIGNAL,CHXPEAK+%1)").arg(i)); // here we have signal with dc regardless subdc is configured
        //m_pDSPInterFace->addCycListItem( s = QString("COPYDATA(CH%1,0,MEASSIGNAL)").arg(chnnr)); // for each channel we work on
        m_pDSPInterFace->addCycListItem( s = QString("RMS(MEASSIGNAL,CHXRMS+%1)").arg(i)); // with or without dc depending on subdc .... see config file
    }
    m_pDSPInterFace->addCycListItem( s = "COPYDU(1,FREQENCY,FREQ)");

    // and filter them
    m_pDSPInterFace->addCycListItem( s = QString("AVERAGE1(%1,CHXPEAK,FILTER)").arg(2*m_ChannelList.count()+1)); // we add results to filter
    m_pDSPInterFace->addCycListItem( s = "TESTTIMESKIPNEX(TISTART,TIPAR)");
    m_pDSPInterFace->addCycListItem( s = "ACTIVATECHAIN(1,0x0102)");

    m_pDSPInterFace->addCycListItem( s = "STARTCHAIN(0,1,0x0102)");
        m_pDSPInterFace->addCycListItem( s = "GETSTIME(TISTART)"); // set new system time
        // The following is so assember-ish: We copy CHXPEAKF, CHXRMSF and FREQF here in one line!!!
        m_pDSPInterFace->addCycListItem( s = QString("CMPAVERAGE1(%1,FILTER,CHXPEAKF)").arg(2*m_ChannelList.count()+1));
        m_pDSPInterFace->addCycListItem( s = QString("CLEARN(%1,FILTER)").arg(2*(2*m_ChannelList.count()+1)+1) );

        m_pDSPInterFace->addCycListItem( s = QString("COPYDU(32,MAXIMUMSAMPLE,MAXRESET)")); // all raw adc maximum samples to userspace

        for (int i = 0; i < m_ChannelList.count(); i++)
        {
            quint8 chnnr;

            cRangeMeasChannel* mchn = m_pModule->getMeasChannel(m_ChannelList.at(i));
            chnnr = mchn->getDSPChannelNr();
            m_pDSPInterFace->addCycListItem( s = QString("COPYDU(1,MAXIMUMSAMPLE+%1,CHXRAWPEAK+%2)").arg(chnnr).arg(i)); // raw adc value maximum
            m_pDSPInterFace->addCycListItem( s = QString("SETVAL(MAXRESET+%1,0.0)").arg(chnnr)); // raw adc value maximum

        }
        m_pDSPInterFace->addCycListItem( s = QString("COPYUD(32,MAXRESET,MAXIMUMSAMPLE)")); // reset dspworkspace maximum samples

        m_pDSPInterFace->addCycListItem( s = QString("DSPINTTRIGGER(0x0,0x%1)").arg(irqNr)); // send interrupt to module
        m_pDSPInterFace->addCycListItem( s = "DEACTIVATECHAIN(1,0x0102)");
    m_pDSPInterFace->addCycListItem( s = "STOPCHAIN(1,0x0102)"); // end processnr., mainchain 1 subchain 2
}


void cRangeModuleMeasProgram::deleteDspCmdList()
{
    m_pDSPInterFace->clearCmdList();
}


void cRangeModuleMeasProgram::catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer)
{
    bool ok;

    if (msgnr == 0) // 0 was reserved for async. messages
    {
        restartDspWachdog();
        QString sintnr;
        sintnr = answer.toString().section(':', 1, 1);
        int service = sintnr.toInt(&ok);
        switch (service)
        {
        case irqNr:
            // we got an interrupt from our cmd chain and have to fetch our actual values
            // but we synchronize on ranging process
            if (!m_bRanging)
            {
                if (!m_bIgnore)
                {
                    if (m_bActive && !m_dataAcquisitionMachine.isRunning()) // in case of deactivation in progress, no dataaquisition
                        m_dataAcquisitionMachine.start();
                }
                else
                    m_bIgnore = false;
            }
            break;
        }
    }
    else
    {
        // because rangemodulemeasprogram, justifynorm share the same dsp interface
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
                break;
            case claimpgrmem:
                if (reply == ack) // we only continue if resource manager acknowledges
                    emit activationContinue();
                else
                {
                    emit errMsg((tr(claimresourceErrMsg)));
                    emit activationError();
                }
                break;
            case claimusermem:
                if (reply == ack) // we only continue if resource manager acknowledges
                    emit activationContinue();
                else
                {
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

cRangeModuleConfigData *cRangeModuleMeasProgram::getConfData()
{
    return qobject_cast<cRangeModuleConfiguration*>(m_pConfiguration.get())->getConfigurationData();
}


void cRangeModuleMeasProgram::setActualValuesNames()
{
    cRangeMeasChannel* mchn;

    for (int i = 0; i < m_ChannelList.count(); i++)
    {
        mchn = m_pModule->getMeasChannel(m_ChannelList.at(i));
        m_veinActValueList.at(i)->setChannelName(mchn->getAlias());
        m_veinActValueList.at(i)->setUnit(mchn->getUnit());
    }
}


void cRangeModuleMeasProgram::setSCPIMeasInfo()
{
    cRangeMeasChannel* mchn;
    cSCPIInfo* pSCPIInfo;

    for (int i = 0; i < m_ChannelList.count(); i++)
    {
        mchn = m_pModule->getMeasChannel(m_ChannelList.at(i));
        pSCPIInfo = new cSCPIInfo("MEASURE", mchn->getAlias(), "8", m_veinActValueList.at(i)->getName(), "0", m_veinActValueList.at(i)->getUnit());
        m_veinActValueList.at(i)->setSCPIInfo(pSCPIInfo);
    }
}

void cRangeModuleMeasProgram::restartDspWachdog()
{
    m_dspWatchdogTimer.stop();
    m_dspWatchdogTimer.setSingleShot(true);
    m_dspWatchdogTimer.start(3000);
}

void cRangeModuleMeasProgram::setInterfaceActualValues(QVector<float> *actualValues)
{
    if (m_bActive) {// maybe we are deactivating !!!!
        int i;
        for (i = 0; i < m_veinActValueList.count()-1; i++) // we set n peak values first
            m_veinActValueList.at(i)->setValue(QVariant((*actualValues)[i]));
        m_veinActValueList.at(i)->setValue(QVariant((*actualValues)[2*i]));

        int rmsOffsetInActual = m_ChannelList.count();
        for(int rmsNo=0; rmsNo<m_ChannelList.count(); rmsNo++)
            m_veinRmsValueList.at(rmsNo)->setValue(QVariant((*actualValues)[rmsNo+rmsOffsetInActual]));
    }
}

void cRangeModuleMeasProgram::resourceManagerConnect()
{
    // first we try to get a connection to resource manager over proxy
    m_rmClient = Zera::Proxy::getInstance()->getConnectionSmart(getConfData()->m_RMSocket.m_sIP, getConfData()->m_RMSocket.m_nPort);
    // and then we set connection resource manager interface's connection
    m_rmInterface.setClientSmart(m_rmClient); //
    resourceManagerConnectState.addTransition(m_rmClient.get(), &Zera::ProxyClient::connected, &m_IdentifyState);
    connect(&m_rmInterface, &Zera::cRMInterface::serverAnswer, this, &cRangeModuleMeasProgram::catchInterfaceAnswer);
    // todo insert timer for timeout and/or connect error conditions
    Zera::Proxy::getInstance()->startConnectionSmart(m_rmClient);
}


void cRangeModuleMeasProgram::sendRMIdent()
{
    m_MsgNrCmdList[m_rmInterface.rmIdent(QString("RangeModule%1").arg(m_pModule->getModuleNr()))] = sendrmident;
}


void cRangeModuleMeasProgram::dspserverConnect()
{
    m_pDspClient = Zera::Proxy::getInstance()->getConnection(getConfData()->m_DSPServerSocket.m_sIP, getConfData()->m_DSPServerSocket.m_nPort);
    m_pDSPInterFace->setClient(m_pDspClient);
    m_dspserverConnectState.addTransition(m_pDspClient, &Zera::ProxyClient::connected, &m_claimPGRMemState);
    connect(m_pDSPInterFace, &Zera::cDSPInterface::serverAnswer, this, &cRangeModuleMeasProgram::catchInterfaceAnswer);
    Zera::Proxy::getInstance()->startConnection(m_pDspClient);
}

void cRangeModuleMeasProgram::claimPGRMem()
{
    m_nSamples = m_pModule->getMeasChannel(m_ChannelList.at(0))->getSampleRate(); // we first read the sample nr from first channel
    setDspVarList(); // first we set the var list for our dsp
    setDspCmdList(); // and the cmd list he has to work on

    m_MsgNrCmdList[m_rmInterface.setResource("DSP1", "PGRMEMC", m_pDSPInterFace->cmdListCount())] = claimpgrmem;
}


void cRangeModuleMeasProgram::claimUSERMem()
{
   m_MsgNrCmdList[m_rmInterface.setResource("DSP1", "USERMEM", m_nDspMemUsed)] = claimusermem;
}


void cRangeModuleMeasProgram::varList2DSP()
{
    m_MsgNrCmdList[m_pDSPInterFace->varList2Dsp()] = varlist2dsp;
}


void cRangeModuleMeasProgram::cmdList2DSP()
{
    m_MsgNrCmdList[m_pDSPInterFace->cmdList2Dsp()] = cmdlist2dsp;
}


void cRangeModuleMeasProgram::activateDSP()
{
    m_MsgNrCmdList[m_pDSPInterFace->activateInterface()] = activatedsp; // aktiviert die var- und cmd-listen im dsp
}


void cRangeModuleMeasProgram::activateDSPdone()
{
    m_bActive = true;
    setActualValuesNames();
    setSCPIMeasInfo();
    m_pMeasureSignal->setValue(QVariant(1));
    if(!m_pModule->getDemo())
        restartDspWachdog();
    emit activated();
}


void cRangeModuleMeasProgram::deactivateDSP()
{
    m_bActive = false;
    m_MsgNrCmdList[m_pDSPInterFace->deactivateInterface()] = deactivatedsp; // wat wohl
}


void cRangeModuleMeasProgram::freePGRMem()
{
    Zera::Proxy::getInstance()->releaseConnection(m_pDspClient);
    deleteDspVarList();
    deleteDspCmdList();

    m_MsgNrCmdList[m_rmInterface.freeResource("DSP1", "PGRMEMC")] = freepgrmem;
}


void cRangeModuleMeasProgram::freeUSERMem()
{
    m_MsgNrCmdList[m_rmInterface.freeResource("DSP1", "USERMEM")] = freeusermem;
}


void cRangeModuleMeasProgram::deactivateDSPdone()
{
    disconnect(&m_rmInterface, 0, this, 0);
    disconnect(m_pDSPInterFace, 0, this, 0);
    emit deactivated();
}


void cRangeModuleMeasProgram::dataAcquisitionDSP()
{
    m_pMeasureSignal->setValue(QVariant(0));
    m_MsgNrCmdList[m_pDSPInterFace->dataAcquisition(m_pActualValuesDSP)] = dataaquistion; // we start our data aquisition now
}


void cRangeModuleMeasProgram::dataReadDSP()
{
    if (m_bActive) {
        m_ModuleActualValues = m_pActualValuesDSP->getData();
        emit actualValues(&m_ModuleActualValues); // and send them
        m_pMeasureSignal->setValue(QVariant(1)); // signal measuring
    }
}


bool cRangeModuleMeasProgram::demoChannelIsVoltage(int channel)
{
    bool isVoltage;
    // a bit of a hack: We expect MT310s2 channel order
    switch(channel) {
    case 0:
    case 1:
    case 2:
    case 6:
        isVoltage = true;
        break;
    default:
        isVoltage = false;
        break;
    }
    return isVoltage;
}

QVector<float> cRangeModuleMeasProgram::demoChannelRms()
{
    double voltageBase = 230.0;
    double currentBase= 10.0;
    QVector<float> randomChannelRMS;
    int channelCount = m_ChannelList.count();
    randomChannelRMS.resize(channelCount);
    for (int channel=0; channel<channelCount; ++channel) {
        bool isVoltage = demoChannelIsVoltage(channel);
        double baseRMS = isVoltage ? voltageBase : currentBase;
        double randPlusMinusOne = 2.0 * (double)rand() / RAND_MAX - 1.0;
        double randOffset = 0.02 * randPlusMinusOne;
        double randRMS = (1+randOffset) * baseRMS;
        randomChannelRMS[channel] = randRMS;
    }
    return randomChannelRMS;
}

double cRangeModuleMeasProgram::demoFrequency()
{
    double freqBase= 50.0;
    double randPlusMinusOne = 2.0 * (double)rand() / RAND_MAX - 1.0;
    double randOffset = 0.02 * randPlusMinusOne;
    double randRMS = (1+randOffset) * freqBase;
    return randRMS;
}

void cRangeModuleMeasProgram::handleDemoPeriodicTimer()
{
    QVector<float> randomChannelRMS = demoChannelRms();
    // values - see cRangeModule::setPeakRmsAndFrequencyValues:
    m_ModuleActualValues.clear();
    int channelCount = m_ChannelList.count();
    // peak
    for (int channel=0; channel<channelCount; ++channel) {
        double randPeak = randomChannelRMS[channel]*sqrt2;
        m_ModuleActualValues.append(randPeak);
        m_veinActValueList.at(channel)->setValue(QVariant(randPeak)); // this should go??
    }
    // RMS
    for (int channel=0; channel<channelCount; ++channel) {
        double randPeak = randomChannelRMS[channel];
        m_ModuleActualValues.append(randPeak);
        m_veinRmsValueList.at(channel)->setValue(QVariant(randPeak)); // this should go??
    }
    // frequency
    m_ModuleActualValues.append(demoFrequency());
    // peak DC (no DC for now)
    for (int channel=0; channel<channelCount; ++channel) {
        double randPeak = randomChannelRMS[channel]*sqrt2;
        m_ModuleActualValues.append(randPeak);
    }
    emit actualValues(&m_ModuleActualValues);
}

void cRangeModuleMeasProgram::onDspWatchdogTimeout()
{
    qCritical("Rangemodule: DSP is stuck!");
    restartDspWachdog();
}

}





