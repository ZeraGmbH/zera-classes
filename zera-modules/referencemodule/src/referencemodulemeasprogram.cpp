#include "referencemodulemeasprogram.h"
#include "referencemodule.h"
#include "referencemeaschannel.h"
#include "referencemoduleconfiguration.h"
#include "debug.h"
#include <errormessages.h>
#include <proxy.h>
#include <proxyclient.h>
#include <reply.h>

namespace REFERENCEMODULE
{

cReferenceModuleMeasProgram::cReferenceModuleMeasProgram(cReferenceModule* module, Zera::Proxy::cProxy* proxy, std::shared_ptr<cBaseModuleConfiguration> pConfiguration)
    :cBaseDspMeasProgram(proxy, pConfiguration), m_pModule(module)
{
    // we have to instantiate a working resource manager and dspserver interface
    m_pDSPInterFace = new Zera::Server::cDSPInterface();

    m_ChannelList = getConfData()->m_referenceChannelList;

    m_IdentifyState.addTransition(this, SIGNAL(activationContinue()), &m_dspserverConnectState);
    m_claimPGRMemState.addTransition(this, SIGNAL(activationContinue()), &m_claimUSERMemState);
    m_claimUSERMemState.addTransition(this, SIGNAL(activationContinue()), &m_var2DSPState);
    m_var2DSPState.addTransition(this, SIGNAL(activationContinue()), &m_cmd2DSPState);
    m_cmd2DSPState.addTransition(this, SIGNAL(activationContinue()), &m_activateDSPState);
    m_activateDSPState.addTransition(this, SIGNAL(activationContinue()), &m_loadDSPDoneState);

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

    connect(&resourceManagerConnectState, SIGNAL(entered()), SLOT(resourceManagerConnect()));
    connect(&m_IdentifyState, SIGNAL(entered()), SLOT(sendRMIdent()));
    connect(&m_dspserverConnectState, SIGNAL(entered()), SLOT(dspserverConnect()));
    connect(&m_claimPGRMemState, SIGNAL(entered()), SLOT(claimPGRMem()));
    connect(&m_claimUSERMemState, SIGNAL(entered()), SLOT(claimUSERMem()));
    connect(&m_var2DSPState, SIGNAL(entered()), SLOT(varList2DSP()));
    connect(&m_cmd2DSPState, SIGNAL(entered()), SLOT(cmdList2DSP()));
    connect(&m_activateDSPState, SIGNAL(entered()), SLOT(activateDSP()));
    connect(&m_loadDSPDoneState, SIGNAL(entered()), SLOT(activateDSPdone()));

    // setting up statemachine for unloading dsp and setting resources free
    m_deactivateDSPState.addTransition(this, SIGNAL(deactivationContinue()), &m_freePGRMemState);
    m_freePGRMemState.addTransition(this, SIGNAL(deactivationContinue()), &m_freeUSERMemState);
    m_freeUSERMemState.addTransition(this, SIGNAL(deactivationContinue()), &m_unloadDSPDoneState);
    m_deactivationMachine.addState(&m_deactivateDSPState);
    m_deactivationMachine.addState(&m_freePGRMemState);
    m_deactivationMachine.addState(&m_freeUSERMemState);
    m_deactivationMachine.addState(&m_unloadDSPDoneState);

    m_deactivationMachine.setInitialState(&m_deactivateDSPState);

    connect(&m_deactivateDSPState, SIGNAL(entered()), SLOT(deactivateDSP()));
    connect(&m_freePGRMemState, SIGNAL(entered()), SLOT(freePGRMem()));
    connect(&m_freeUSERMemState, SIGNAL(entered()), SLOT(freeUSERMem()));
    connect(&m_unloadDSPDoneState, SIGNAL(entered()), SLOT(deactivateDSPdone()));

    // setting up statemachine for data acquisition
    m_dataAcquisitionState.addTransition(this, SIGNAL(dataAquisitionContinue()), &m_dataAcquisitionDoneState);
    m_dataAcquisitionMachine.addState(&m_dataAcquisitionState);
    m_dataAcquisitionMachine.addState(&m_dataAcquisitionDoneState);
    m_dataAcquisitionMachine.setInitialState(&m_dataAcquisitionState);
    connect(&m_dataAcquisitionState, SIGNAL(entered()), SLOT(dataAcquisitionDSP()));
    connect(&m_dataAcquisitionDoneState, SIGNAL(entered()), SLOT(dataReadDSP()));
}


cReferenceModuleMeasProgram::~cReferenceModuleMeasProgram()
{
    delete m_pDSPInterFace;
}


void cReferenceModuleMeasProgram::start()
{
    // connect(this, SIGNAL(actualValues(QVector<float>*)), this, SLOT(setInterfaceActualValues(QVector<float>*)));
}


void cReferenceModuleMeasProgram::stop()
{
    // disconnect(this, SIGNAL(actualValues(QVector<float>*)), this, SLOT(setInterfaceActualValues(QVector<float>*)));
}


void cReferenceModuleMeasProgram::generateInterface()
{
    // we have no interface
}


void cReferenceModuleMeasProgram::setDspVarList()
{

    // we fetch a handle for sampled data and other temporary values
    m_pTmpDataDsp = m_pDSPInterFace->getMemHandle("TmpData");
    m_pTmpDataDsp->addVarItem( new cDspVar("MEASSIGNAL", m_nSamples, DSPDATA::vDspTemp));
    m_pTmpDataDsp->addVarItem( new cDspVar("TISTART",1, DSPDATA::vDspTemp, DSPDATA::dInt));
    m_pTmpDataDsp->addVarItem( new cDspVar("VALXDFT",m_ChannelList.count()*2, DSPDATA::vDspTemp));
    m_pTmpDataDsp->addVarItem( new cDspVar("FILTER",2*2*m_ChannelList.count(),DSPDATA::vDspTemp));
    m_pTmpDataDsp->addVarItem( new cDspVar("N",1,DSPDATA::vDspTemp));

    // a handle for parameter
    m_pParameterDSP =  m_pDSPInterFace->getMemHandle("Parameter");
    m_pParameterDSP->addVarItem( new cDspVar("TIPAR",1, DSPDATA::vDspParam, DSPDATA::dInt)); // integrationtime res = 1ms

    // and one for filtered actual values
    m_pActualValuesDSP = m_pDSPInterFace->getMemHandle("ActualValues");
    m_pActualValuesDSP->addVarItem( new cDspVar("VALXDFTF",2*m_ChannelList.count(), DSPDATA::vDspResult));

    m_ModuleActualValues.resize(m_pActualValuesDSP->getSize()); // we provide a vector for generated actual values
    m_nDspMemUsed = m_pTmpDataDsp->getSize() + m_pParameterDSP->getSize() + m_pActualValuesDSP->getSize();
}


void cReferenceModuleMeasProgram::deleteDspVarList()
{
    m_pDSPInterFace->deleteMemHandle(m_pTmpDataDsp);
    m_pDSPInterFace->deleteMemHandle(m_pParameterDSP);
    m_pDSPInterFace->deleteMemHandle(m_pActualValuesDSP);
}


void cReferenceModuleMeasProgram::setDspCmdList()
{
    QString s;

    m_pDSPInterFace->addCycListItem( s = "STARTCHAIN(1,1,0x0101)"); // aktiv, prozessnr. (dummy),hauptkette 1 subkette 1 start
        m_pDSPInterFace->addCycListItem( s = QString("CLEARN(%1,MEASSIGNAL)").arg(m_nSamples) ); // clear meassignal
        m_pDSPInterFace->addCycListItem( s = QString("CLEARN(%1,FILTER)").arg(2*2*m_ChannelList.count()+1) ); // clear the whole filter incl. count
        m_pDSPInterFace->addCycListItem( s = QString("SETVAL(TIPAR,%1)").arg(getConfData()->m_fMeasInterval*1000.0)); // initial ti time  /* todo variabel */
        m_pDSPInterFace->addCycListItem( s = "GETSTIME(TISTART)"); // einmal ti start setzen
        m_pDSPInterFace->addCycListItem( s = "DEACTIVATECHAIN(1,0x0101)"); // ende prozessnr., hauptkette 1 subkette 1
    m_pDSPInterFace->addCycListItem( s = "STOPCHAIN(1,0x0101)"); // ende prozessnr., hauptkette 1 subkette 1

    // we compute or copy our wanted actual values
    for (int i = 0; i < m_ChannelList.count(); i++)
    {
        cReferenceMeasChannel* mchn = m_pModule->getMeasChannel(m_ChannelList.at(i));
        m_pDSPInterFace->addCycListItem( s = QString("COPYDATA(CH%1,0,MEASSIGNAL)").arg(mchn->getDSPChannelNr())); // for each channel we work on
        m_pDSPInterFace->addCycListItem( s = QString("DFT(0,MEASSIGNAL,VALXDFT+%1)").arg(2*i));
    }

    // and filter them
    m_pDSPInterFace->addCycListItem( s = QString("AVERAGE1(%1,VALXDFT,FILTER)").arg(2*m_ChannelList.count())); // we add results to filter

    m_pDSPInterFace->addCycListItem( s = "TESTTIMESKIPNEX(TISTART,TIPAR)");
    m_pDSPInterFace->addCycListItem( s = "ACTIVATECHAIN(1,0x0102)");

    m_pDSPInterFace->addCycListItem( s = "STARTCHAIN(0,1,0x0102)");
        m_pDSPInterFace->addCycListItem( s = "GETSTIME(TISTART)" ); // set new system time
        m_pDSPInterFace->addCycListItem( s = QString("CMPAVERAGE1(%1,FILTER,VALXDFTF)").arg(2*m_ChannelList.count()) );
        m_pDSPInterFace->addCycListItem( s = QString("CLEARN(%1,FILTER)").arg(2*2*m_ChannelList.count()+1) );
        m_pDSPInterFace->addCycListItem( s = QString("DSPINTTRIGGER(0x0,0x%1)").arg(irqNr) ); // send interrupt to module
        m_pDSPInterFace->addCycListItem( s = "DEACTIVATECHAIN(1,0x0102)" );
    m_pDSPInterFace->addCycListItem( s = "STOPCHAIN(1,0x0102)"); // end processnr., mainchain 1 subchain 2
}


void cReferenceModuleMeasProgram::deleteDspCmdList()
{
    m_pDSPInterFace->clearCmdList();
}


void cReferenceModuleMeasProgram::catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer)
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

            case claimpgrmem:
                if (reply == ack) // we only continue if resource manager acknowledges
                    emit activationContinue();
                else
                {
                    emit errMsg((tr(claimresourceErrMsg)));
#ifdef DEBUG
                    qDebug() << claimresourceErrMsg;
#endif
                    emit activationError();
                }
                break;

            case claimusermem:
                if (reply == ack) // we only continue if resource manager acknowledges
                    emit activationContinue();
                else
                {
                    emit errMsg((tr(claimresourceErrMsg)));
#ifdef DEBUG
                    qDebug() << claimresourceErrMsg;
#endif
                    emit activationError();
                }
                break;
            case varlist2dsp:
                if (reply == ack) // we only continue if resource manager acknowledges
                    emit activationContinue();
                else
                {
                    emit errMsg((tr(dspvarlistwriteErrMsg)));
#ifdef DEBUG
                    qDebug() << dspvarlistwriteErrMsg;
#endif
                    emit activationError();
                }
                break;
            case cmdlist2dsp:
                if (reply == ack) // we only continue if resource manager acknowledges
                    emit activationContinue();
                else
                {
                    emit errMsg((tr(dspcmdlistwriteErrMsg)));
#ifdef DEBUG
                    qDebug() << dspcmdlistwriteErrMsg;
#endif
                    emit activationError();
                }
                break;
            case activatedsp:
                if (reply == ack) // we only continue if resource manager acknowledges
                    emit activationContinue();
                else
                {
                    emit errMsg((tr(dspactiveErrMsg)));
#ifdef DEBUG
                    qDebug() << dspactiveErrMsg;
#endif
                    emit activationError();
                }
                break;

            case deactivatedsp:
                if (reply == ack) // we only continue if resource manager acknowledges
                    emit deactivationContinue();
                else
                {
                    emit errMsg((tr(dspdeactiveErrMsg)));
#ifdef DEBUG
                    qDebug() << dspdeactiveErrMsg;
#endif
                    emit deactivationError();
                }
                break;
            case freepgrmem:
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
            case freeusermem:
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

            case dataaquistion:
                if (reply == ack)
                    emit dataAquisitionContinue();
                else
                {
                    m_dataAcquisitionMachine.stop();
                    // perhaps we emit some error here ?
                    {
                        emit errMsg((tr(dataaquisitionErrMsg)));
#ifdef DEBUG
                        qDebug() << dataaquisitionErrMsg;
#endif
                        emit executionError();
                    }
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
    m_rmClient = m_pProxy->getConnectionSmart(getConfData()->m_RMSocket.m_sIP, getConfData()->m_RMSocket.m_nPort);
    // and then we set connection resource manager interface's connection
    m_rmInterface.setClientSmart(m_rmClient); //
    resourceManagerConnectState.addTransition(m_rmClient.get(), SIGNAL(connected()), &m_IdentifyState);
    connect(&m_rmInterface, SIGNAL(serverAnswer(quint32, quint8, QVariant)), this, SLOT(catchInterfaceAnswer(quint32, quint8, QVariant)));
    // todo insert timer for timeout and/or connect error conditions
    m_pProxy->startConnectionSmart(m_rmClient);
}


void cReferenceModuleMeasProgram::sendRMIdent()
{
    m_MsgNrCmdList[m_rmInterface.rmIdent(QString("ReferenceModule%1").arg(m_pModule->getModuleNr()))] = sendrmident;
}


void cReferenceModuleMeasProgram::dspserverConnect()
{
    m_pDspClient = m_pProxy->getConnection(getConfData()->m_DSPServerSocket.m_sIP, getConfData()->m_DSPServerSocket.m_nPort);
    m_pDSPInterFace->setClient(m_pDspClient);
    m_dspserverConnectState.addTransition(m_pDspClient, SIGNAL(connected()), &m_claimPGRMemState);
    connect(m_pDSPInterFace, SIGNAL(serverAnswer(quint32, quint8, QVariant)), this, SLOT(catchInterfaceAnswer(quint32, quint8, QVariant)));
    m_pProxy->startConnection(m_pDspClient);
}

void cReferenceModuleMeasProgram::claimPGRMem()
{
    m_nSamples = m_pModule->getMeasChannel(m_ChannelList.at(0))->getSampleRate(); // we first read the sample nr from first channel
    setDspVarList(); // first we set the var list for our dsp
    setDspCmdList(); // and the cmd list he has to work on

    m_MsgNrCmdList[m_rmInterface.setResource("DSP1", "PGRMEMC", m_pDSPInterFace->cmdListCount())] = claimpgrmem;
}


void cReferenceModuleMeasProgram::claimUSERMem()
{
   m_MsgNrCmdList[m_rmInterface.setResource("DSP1", "USERMEM", m_nDspMemUsed)] = claimusermem;
}


void cReferenceModuleMeasProgram::varList2DSP()
{
    m_MsgNrCmdList[m_pDSPInterFace->varList2Dsp()] = varlist2dsp;
}


void cReferenceModuleMeasProgram::cmdList2DSP()
{
    m_MsgNrCmdList[m_pDSPInterFace->cmdList2Dsp()] = cmdlist2dsp;
}


void cReferenceModuleMeasProgram::activateDSP()
{
    m_MsgNrCmdList[m_pDSPInterFace->activateInterface()] = activatedsp; // aktiviert die var- und cmd-listen im dsp
}


void cReferenceModuleMeasProgram::activateDSPdone()
{
    m_bActive = true;
    emit activated();
}


void cReferenceModuleMeasProgram::deactivateDSP()
{
    m_bActive = false;
    m_MsgNrCmdList[m_pDSPInterFace->deactivateInterface()] = deactivatedsp; // wat wohl
}


void cReferenceModuleMeasProgram::freePGRMem()
{
    m_pProxy->releaseConnection(m_pDspClient);
    deleteDspVarList();
    deleteDspCmdList();

    m_MsgNrCmdList[m_rmInterface.freeResource("DSP1", "PGRMEMC")] = freepgrmem;
}


void cReferenceModuleMeasProgram::freeUSERMem()
{
    m_MsgNrCmdList[m_rmInterface.freeResource("DSP1", "USERMEM")] = freeusermem;
}


void cReferenceModuleMeasProgram::deactivateDSPdone()
{
    disconnect(&m_rmInterface, 0, this, 0);
    disconnect(m_pDSPInterFace, 0, this, 0);
    emit deactivated();
}


void cReferenceModuleMeasProgram::dataAcquisitionDSP()
{
    m_MsgNrCmdList[m_pDSPInterFace->dataAcquisition(m_pActualValuesDSP)] = dataaquistion; // we start our data aquisition now
}


void cReferenceModuleMeasProgram::dataReadDSP()
{
    if (m_bActive)
    {
        m_pDSPInterFace->getData(m_pActualValuesDSP, m_ModuleActualValues);
        emit actualValues(&m_ModuleActualValues); // and send them

    }
}

}





