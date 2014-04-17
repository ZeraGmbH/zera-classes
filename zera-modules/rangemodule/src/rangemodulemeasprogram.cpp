#include <QCoreApplication>
#include <QString>
#include <QStateMachine>
#include <rminterface.h>
#include <dspinterface.h>
#include <proxy.h>
#include <proxyclient.h>
#include <veinpeer.h>
#include <veinentity.h>

#include "rangemodule.h"
#include "rangemeaschannel.h"
#include "rangemodulemeasprogram.h"


cRangeModuleMeasProgram::cRangeModuleMeasProgram(cRangeModule* module, Zera::Proxy::cProxy* proxy, VeinPeer* peer, Zera::Server::cDSPInterface* iface, cSocket* rmsocket, QStringList chnlist, float interval)
    :cBaseMeasProgram(proxy, peer, iface, rmsocket, chnlist), m_pModule(module), m_fInterval(interval)
{
    m_bRanging = false;
    m_bIgnore = false;
    app = QCoreApplication::instance();
    m_pRMInterface = new Zera::Server::cRMInterface();

    generateInterface(); // here we set up our visible entities

    m_IdentifyState.addTransition(this, SIGNAL(activationContinue()), &m_claimPGRMemState);
    m_claimPGRMemState.addTransition(this, SIGNAL(activationContinue()), &m_claimUSERMemState);
    m_claimUSERMemState.addTransition(this, SIGNAL(activationContinue()), &m_var2DSPState);
    m_var2DSPState.addTransition(this, SIGNAL(activationContinue()), &m_cmd2DSPState);
    m_cmd2DSPState.addTransition(this, SIGNAL(activationContinue()), &m_activateDSPState);
    m_activateDSPState.addTransition(this, SIGNAL(activationContinue()), &m_loadDSPDoneState);
    m_loadDSPMachine.addState(&m_wait4ConnectionState);
    m_loadDSPMachine.addState(&m_IdentifyState);
    m_loadDSPMachine.addState(&m_claimPGRMemState);
    m_loadDSPMachine.addState(&m_claimUSERMemState);
    m_loadDSPMachine.addState(&m_var2DSPState);
    m_loadDSPMachine.addState(&m_cmd2DSPState);
    m_loadDSPMachine.addState(&m_activateDSPState);
    m_loadDSPMachine.addState(&m_loadDSPDoneState);

    m_loadDSPMachine.setInitialState(&m_wait4ConnectionState);

    connect(&m_IdentifyState, SIGNAL(entered()), SLOT(sendRMIdent()));
    connect(&m_claimPGRMemState, SIGNAL(entered()), SLOT(claimPGRMem()));
    connect(&m_claimUSERMemState, SIGNAL(entered()), SLOT(claimUSERMem()));
    connect(&m_var2DSPState, SIGNAL(entered()), SLOT(varList2DSP()));
    connect(&m_cmd2DSPState, SIGNAL(entered()), SLOT(cmdList2DSP()));
    connect(&m_activateDSPState, SIGNAL(entered()), SLOT(activateDSP()));
    connect(&m_loadDSPDoneState, SIGNAL(entered()), SLOT(activateDSPdone()));

    // setting up statemachine for unloading dsp and setting resources free
    m_deactivateDSPState.addTransition(this, SIGNAL(activationContinue()), &m_freePGRMemState);
    m_freePGRMemState.addTransition(this, SIGNAL(activationContinue()), &m_freeUSERMemState);
    m_freeUSERMemState.addTransition(this, SIGNAL(activationContinue()), &m_unloadDSPDoneState);
    m_unloadDSPMachine.addState(&m_deactivateDSPState);
    m_unloadDSPMachine.addState(&m_freePGRMemState);
    m_unloadDSPMachine.addState(&m_freeUSERMemState);
    m_unloadDSPMachine.addState(&m_unloadDSPDoneState);

    m_unloadDSPMachine.setInitialState(&m_deactivateDSPState);

    connect(&m_deactivateDSPState, SIGNAL(entered()), SLOT(deactivateDSP()));
    connect(&m_freePGRMemState, SIGNAL(entered()), SLOT(freePGRMem()));
    connect(&m_freeUSERMemState, SIGNAL(entered()), SLOT(freeUSERMem()));
    connect(&m_unloadDSPDoneState, SIGNAL(entered()), SLOT(deactivateDSPdone()));

    // setting up statemachine for data acquisition
    m_dataAcquisitionState.addTransition(this, SIGNAL(activationContinue()), &m_dataAcquisitionDoneState);
    m_dataAcquisitionMachine.addState(&m_dataAcquisitionState);
    m_dataAcquisitionMachine.addState(&m_dataAcquisitionDoneState);
    m_dataAcquisitionMachine.setInitialState(&m_dataAcquisitionState);
    connect(&m_dataAcquisitionState, SIGNAL(entered()), SLOT(dataAcquisitionDSP()));
    connect(&m_dataAcquisitionDoneState, SIGNAL(entered()), SLOT(dataReadDSP()));
}


cRangeModuleMeasProgram::~cRangeModuleMeasProgram()
{
    deleteInterface();
    deactivate();
    // app->exec(); // sync !!!!!!
    delete m_pRMInterface;
}


void cRangeModuleMeasProgram::activate()
{
    setDspVarList(); // first we set the var list
    setDspCmdList(); // and the cmd list

    // we have to instantiate a working resource manager interface
    // so first we try to get a connection to resource manager over proxy
    Zera::Proxy::cProxyClient* client = m_pProxy->getConnection(m_pRMSocket->m_sIP, m_pRMSocket->m_nPort);
    m_wait4ConnectionState.addTransition(client, SIGNAL(connected()), &m_IdentifyState);
    // and then we set connection resource manager interface's connection
    m_pRMInterface->setClient(client); //
    // todo insert timer for timeout

    connect(m_pRMInterface, SIGNAL(serverAnswer(quint32, quint8, QVariant)), this, SLOT(catchInterfaceAnswer(quint32, quint8, QVariant)));
    connect(m_pDSPIFace, SIGNAL(serverAnswer(quint32, quint8, QVariant)), this, SLOT(catchInterfaceAnswer(quint32, quint8, QVariant)));
    m_loadDSPMachine.start(); // then we start the statemachine for activating the dsp and claiming the resources
}


void cRangeModuleMeasProgram::deactivate()
{
    deleteDspVarList();
    deleteDspCmdList();

    m_unloadDSPMachine.start(); // unload dsp program and free resources
}


void cRangeModuleMeasProgram::start()
{
    connect(this, SIGNAL(actualValues(QVector<float>*)), this, SLOT(setInterfaceActualValues(QVector<float>*)));
}


void cRangeModuleMeasProgram::stop()
{
    disconnect(this, SIGNAL(actualValues(QVector<float>*)), this, SLOT(setInterfaceActualValues(QVector<float>*)));
}


void cRangeModuleMeasProgram::syncRanging(QVariant sync)
{
    m_bRanging = (sync != 0); // we are ranging from the moment sync becomes 1
    m_bIgnore = (sync == 0); // when ranging has finished we ignore 1 more actual values
}


void cRangeModuleMeasProgram::generateInterface()
{
    VeinEntity* p_entity;
    for (int i = 0; i < m_ChannelList.count(); i++)
    {
        p_entity = m_pPeer->dataAdd(QString("ACT_Peak%1").arg(i+1));
        p_entity->modifiersAdd(VeinEntity::MOD_READONLY);
        p_entity->modifiersAdd(VeinEntity::MOD_NOECHO);
        p_entity->setValue(QVariant((double) 0.0), m_pPeer);
        m_EntityList.append(p_entity);
    }

    p_entity = m_pPeer->dataAdd("ACT_Frequency");
    p_entity->modifiersAdd(VeinEntity::MOD_READONLY);
    p_entity->modifiersAdd(VeinEntity::MOD_NOECHO);
    p_entity->setValue(QVariant((double) 0.0), m_pPeer);
    m_EntityList.append(p_entity);
}


void cRangeModuleMeasProgram::deleteInterface()
{
    for (int i = 0; i < m_EntityList.count(); i++)
        m_pPeer->dataRemove(m_EntityList.at(i));
}


void cRangeModuleMeasProgram::setDspVarList()
{
    // we fetch a handle for sampled data and other temporary values
    m_pTmpDataDsp = m_pDSPIFace->getMemHandle("TmpData");
    m_pTmpDataDsp->addVarItem( new cDspVar("MEASSIGNAL", 504/* !!! später variable*/, DSPDATA::vDspTemp));
    m_pTmpDataDsp->addVarItem( new cDspVar("TISTART",1, DSPDATA::vDspTemp, DSPDATA::dInt));
    m_pTmpDataDsp->addVarItem( new cDspVar("CHXPEAK",m_ChannelList.count(), DSPDATA::vDspTemp));
    m_pTmpDataDsp->addVarItem( new cDspVar("CHXRMS",m_ChannelList.count(), DSPDATA::vDspTemp));
    m_pTmpDataDsp->addVarItem( new cDspVar("FREQ", 1, DSPDATA::vDspTemp));
    m_pTmpDataDsp->addVarItem( new cDspVar("FILTER",2*(2*m_ChannelList.count()+1),DSPDATA::vDspTemp));
    m_pTmpDataDsp->addVarItem( new cDspVar("N",1,DSPDATA::vDspTemp));

    // a handle for parameter
    m_pParameterDSP =  m_pDSPIFace->getMemHandle("Parameter");
    m_pParameterDSP->addVarItem( new cDspVar("TIPAR",1, DSPDATA::vDspParam, DSPDATA::dInt)); // integrationtime res = 1ms

    // and one for filtered actual values
    m_pActualValuesDSP = m_pDSPIFace->getMemHandle("ActualValues");
    m_pActualValuesDSP->addVarItem( new cDspVar("CHXPEAKF",m_ChannelList.count(), DSPDATA::vDspResult));
    m_pActualValuesDSP->addVarItem( new cDspVar("CHXRMSF",m_ChannelList.count(), DSPDATA::vDspResult));
    m_pActualValuesDSP->addVarItem( new cDspVar("FREQF", 1, DSPDATA::vDspResult));

    m_ModuleActualValues.resize(m_pActualValuesDSP->getSize()); // we provide a vector for generated actual values
    m_nDspMemUsed = m_pTmpDataDsp->getSize() + m_pParameterDSP->getSize() + m_pActualValuesDSP->getSize();
}


void cRangeModuleMeasProgram::deleteDspVarList()
{
    m_pDSPIFace->deleteMemHandle(m_pTmpDataDsp);
    m_pDSPIFace->deleteMemHandle(m_pParameterDSP);
    m_pDSPIFace->deleteMemHandle(m_pActualValuesDSP);
}


void cRangeModuleMeasProgram::setDspCmdList()
{
    QString s;

    m_pDSPIFace->addCycListItem( s = "STARTCHAIN(1,1,0x0101)"); // aktiv, prozessnr. (dummy),hauptkette 1 subkette 1 start
        m_pDSPIFace->addCycListItem( s = QString("CLEARN(%1,MEASSIGNAL)").arg(504) ); // clear meassignal
        m_pDSPIFace->addCycListItem( s = QString("CLEARN(%1,FILTER)").arg(2*(2*m_ChannelList.count()+1)+1) ); // clear the whole filter incl. count
        m_pDSPIFace->addCycListItem( s = QString("SETVAL(TIPAR,%1)").arg(m_fInterval*1000.0)); // initial ti time  /* todo variabel */
        m_pDSPIFace->addCycListItem( s = "GETSTIME(TISTART)"); // einmal ti start setzen
        m_pDSPIFace->addCycListItem( s = "DEACTIVATECHAIN(1,0x0101)"); // ende prozessnr., hauptkette 1 subkette 1
    m_pDSPIFace->addCycListItem( s = "STOPCHAIN(1,0x0101)"); // ende prozessnr., hauptkette 1 subkette 1

    // we compute or copy our wanted actual values
    for (int i = 0; i < m_ChannelList.count(); i++)
    {
        cRangeMeasChannel* mchn = m_pModule->getMeasChannel(m_ChannelList.at(i));
        m_pDSPIFace->addCycListItem( s = QString("COPYDATA(CH%1,0,MEASSIGNAL)").arg(mchn->getDSPChannelNr())); // for each channel we work on
        m_pDSPIFace->addCycListItem( s = QString("SETPEAK(MEASSIGNAL,CHXPEAK+%1)").arg(i));
        m_pDSPIFace->addCycListItem( s = QString("RMS(MEASSIGNAL,CHXRMS+%1)").arg(i));
    }
    m_pDSPIFace->addCycListItem( s = "COPYDU(1,FREQENCY,FREQ)");

    // and filter them
    m_pDSPIFace->addCycListItem( s = QString("AVERAGE1(%1,CHXPEAK,FILTER)").arg(2*m_ChannelList.count()+1)); // we add results to filter

    m_pDSPIFace->addCycListItem( s = "TESTTIMESKIPNEX(TISTART,TIPAR)");
    m_pDSPIFace->addCycListItem( s = "ACTIVATECHAIN(1,0x0102)");

    m_pDSPIFace->addCycListItem( s = "STARTCHAIN(0,1,0x0102)");
        m_pDSPIFace->addCycListItem( s = "GETSTIME(TISTART)"); // set new system time
        m_pDSPIFace->addCycListItem( s = QString("CMPAVERAGE1(%1,FILTER,CHXPEAKF)").arg(2*m_ChannelList.count()+1));
        m_pDSPIFace->addCycListItem( s = QString("CLEARN(%1,FILTER)").arg(2*(2*m_ChannelList.count()+1)+1) );
        m_pDSPIFace->addCycListItem( s = "DSPINTTRIGGER(0x0,0x0001)"); // send interrupt to module
        m_pDSPIFace->addCycListItem( s = "DEACTIVATECHAIN(1,0x0102)");
    m_pDSPIFace->addCycListItem( s = "STOPCHAIN(1,0x0102)"); // end processnr., mainchain 1 subchain 2
}


void cRangeModuleMeasProgram::deleteDspCmdList()
{
    m_pDSPIFace->clearCmdList();
}


void cRangeModuleMeasProgram::catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer)
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
        case 1:
            // we got an interrupt from our cmd chain and have to fetch our actual values
            // but we synchronize on ranging process
            if (!m_bRanging)
            {
                if (!m_bIgnore)
                    m_dataAcquisitionMachine.start();
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
            case MEASPROGRAM::sendrmident:
            case MEASPROGRAM::claimpgrmem:
            case MEASPROGRAM::claimusermem:
            case MEASPROGRAM::varlist2dsp:
            case MEASPROGRAM::cmdlist2dsp:
            case MEASPROGRAM::activatedsp:
                if (reply == ack) // we only continue if resource manager acknowledges
                    emit activationContinue();
                else
                    emit activationError();
                break;

            case MEASPROGRAM::deactivatedsp:
            case MEASPROGRAM::freepgrmem:
            case MEASPROGRAM::freeusermem:
                if (reply == ack) // we only continue if resource manager acknowledges
                    emit activationContinue();
                else
                    emit deactivationError();
                break;

            case MEASPROGRAM::dataaquistion:
                if (reply == ack)
                    emit activationContinue();
                else
                {
                    m_dataAcquisitionMachine.stop();
                    // perhaps we emit some error here ?
                }
                break;
            }
        }
    }
}


void cRangeModuleMeasProgram::setInterfaceActualValues(QVector<float> *actualValues)
{
    int i;
    for (i = 0; i < m_EntityList.count()-1; i++) // we set n peak values first
        m_EntityList.at(i)->setValue((*actualValues)[i], m_pPeer);
    m_EntityList.at(i)->setValue((*actualValues)[2*i], m_pPeer);
}


void cRangeModuleMeasProgram::sendRMIdent()
{
    m_MsgNrCmdList[m_pRMInterface->rmIdent("RangeModule")] = MEASPROGRAM::sendrmident;
}


void cRangeModuleMeasProgram::claimPGRMem()
{
  m_MsgNrCmdList[m_pRMInterface->setResource("DSP1", "PGRMEMC", m_pDSPIFace->cmdListCount())] = MEASPROGRAM::claimpgrmem;
}


void cRangeModuleMeasProgram::claimUSERMem()
{
   m_MsgNrCmdList[m_pRMInterface->setResource("DSP1", "USERMEM", m_nDspMemUsed)] = MEASPROGRAM::claimusermem;
}


void cRangeModuleMeasProgram::varList2DSP()
{
    m_MsgNrCmdList[m_pDSPIFace->varList2Dsp()] = MEASPROGRAM::varlist2dsp;
}


void cRangeModuleMeasProgram::cmdList2DSP()
{
    m_MsgNrCmdList[m_pDSPIFace->cmdList2Dsp()] = MEASPROGRAM::cmdlist2dsp;
}


void cRangeModuleMeasProgram::activateDSP()
{
    m_MsgNrCmdList[m_pDSPIFace->activateInterface()] = MEASPROGRAM::activatedsp; // aktiviert die var- und cmd-listen im dsp
}


void cRangeModuleMeasProgram::activateDSPdone()
{
    emit activated();
}


void cRangeModuleMeasProgram::deactivateDSP()
{
    m_MsgNrCmdList[m_pDSPIFace->deactivateInterface()] = MEASPROGRAM::deactivatedsp; // wat wohl
}


void cRangeModuleMeasProgram::freePGRMem()
{
    m_MsgNrCmdList[m_pRMInterface->freeResource("DSP1", "PGRMEMC")] = MEASPROGRAM::freepgrmem;
}


void cRangeModuleMeasProgram::freeUSERMem()
{
    m_MsgNrCmdList[m_pRMInterface->freeResource("DSP1", "USERMEM")] = MEASPROGRAM::freeusermem;
}


void cRangeModuleMeasProgram::deactivateDSPdone()
{
    disconnect(m_pRMInterface, 0, this, 0);
    disconnect(m_pDSPIFace, 0, this, 0);
    emit deactivated();
}


void cRangeModuleMeasProgram::dataAcquisitionDSP()
{
    m_MsgNrCmdList[m_pDSPIFace->dataAcquisition(m_pActualValuesDSP)] = MEASPROGRAM::dataaquistion; // we start our data aquisition now
}


void cRangeModuleMeasProgram::dataReadDSP()
{
    m_pDSPIFace->setData(m_pActualValuesDSP, m_ModuleActualValues);
    emit actualValues(&m_ModuleActualValues); // and send them
}







