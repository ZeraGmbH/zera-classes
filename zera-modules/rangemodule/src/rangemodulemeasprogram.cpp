#include <QString>
#include <QStateMachine>
#include <dspinterface.h>
#include <veinpeer.h>
#include <veinentity.h>

#include "rangemodule.h"
#include "rangemeaschannel.h"
#include "rangemodulemeasprogram.h"


cRangeModuleMeasProgram::cRangeModuleMeasProgram(cRangeModule* module, VeinPeer* peer, Zera::Server::cDSPInterface* iface, cSocket* rmsocket, QStringList chnlist)
    :cBaseMeasProgram(peer, iface, rmsocket, chnlist), m_pModule(module)
{
    generateInterface();

    m_var2DSPState.addTransition(this, SIGNAL(activationContinue()), &m_cmd2DSPState);
    m_cmd2DSPState.addTransition(this, SIGNAL(activationContinue()), &m_activateDSPState);
    m_activateDSPState.addTransition(this, SIGNAL(activationContinue()), &m_loadDSPDoneState);
    m_loadDSPMachine.addState(&m_var2DSPState);
    m_loadDSPMachine.addState(&m_cmd2DSPState);
    m_loadDSPMachine.addState(&m_activateDSPState);
    m_loadDSPMachine.addState(&m_loadDSPDoneState);
    m_loadDSPMachine.setInitialState(&m_var2DSPState);
    connect(&m_var2DSPState, SIGNAL(entered()), SLOT(varList2DSP()));
    connect(&m_cmd2DSPState, SIGNAL(entered()), SLOT(cmdList2DSP()));
    connect(&m_activateDSPState, SIGNAL(entered()), SLOT(activateDSP()));
    connect(&m_loadDSPDoneState, SIGNAL(entered()), SLOT(activateDSPdone()));

    m_deactivateDSPState.addTransition(this, SIGNAL(activationContinue()), &m_unloadDSPDoneState);
    m_unloadDSPMachine.addState(&m_deactivateDSPState);
    m_unloadDSPMachine.addState(&m_unloadDSPDoneState);
    m_unloadDSPMachine.setInitialState(&m_deactivateDSPState);
    connect(&m_deactivateDSPState, SIGNAL(entered()), SLOT(deactivateDSP()));
    connect(&m_unloadDSPDoneState, SIGNAL(entered()), SLOT(deactivateDSPdone()));

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
}


void cRangeModuleMeasProgram::activate()
{
    setDspVarList(); // first we set the var list
    setDspCmdList(); // and the cmd list

    // wir müssen die benötigten ressourcen noch anmelden

    connect(m_pDSPIFace, SIGNAL(serverAnswer(quint32, QVariant)), this, SLOT(catchInterfaceAnswer(quint32,QVariant)));
    m_loadDSPMachine.start(); // then we start the statemachine for activating the dsp
}


void cRangeModuleMeasProgram::deactivate()
{
    deleteDspVarList();
    deleteDspCmdList();
    m_unloadDSPMachine.start();
}


void cRangeModuleMeasProgram::start()
{
    connect(this, SIGNAL(actualValues(QVector<double>*)), this, SLOT(setInterfaceActualValues(QVector<double>*)));
}


void cRangeModuleMeasProgram::stop()
{
    disconnect(this, SIGNAL(actualValues(QVector<double>*)), this, SLOT(setInterfaceActualValues(QVector<double>*)));
}


void cRangeModuleMeasProgram::generateInterface()
{
    VeinEntity* p_entity;
    for (int i = 0; i < m_ChannelList.count(); i++)
    {
        p_entity = m_pPeer->dataAdd(QString("Peak%1").arg(i+1));
        p_entity->modifiersAdd(VeinEntity::MOD_READONLY);
        p_entity->modifiersAdd(VeinEntity::MOD_NOECHO);
        p_entity->setValue(QVariant((double) 0.0));
        m_EntityList.append(p_entity);
    }

    p_entity = m_pPeer->dataAdd("Frequency");
    p_entity->modifiersAdd(VeinEntity::MOD_READONLY);
    p_entity->modifiersAdd(VeinEntity::MOD_NOECHO);
    p_entity->setValue(QVariant((double) 0.0));
    m_EntityList.append(p_entity);
}


void cRangeModuleMeasProgram::deleteInterface()
{
    for (int i = 0; i < m_EntityList.count(); i++)
        m_pPeer->dataRemove(m_EntityList.at(i));
}


void cRangeModuleMeasProgram::setDspVarList()
{
    // we fetch a handle for sampled data
    m_pTmpDataDsp = m_pDSPIFace->getMemHandle("TmpData", DSPDATA::sLocal);
    m_pTmpDataDsp->addVarItem( new cDspVar("MEASSIGNAL", 504/* !!! später variable*/, DSPDATA::vDspTemp));
    m_pTmpDataDsp->addVarItem( new cDspVar("TISTART",1, DSPDATA::vDspTemp));

    // a handle for parameter
    m_pParameterDSP =  m_pDSPIFace->getMemHandle("Parameter", DSPDATA::sLocal);
    m_pParameterDSP->addVarItem( new cDspVar("TIPar",1, DSPDATA::vDspParam)); // integrationtime res = 1ms

    // and one for actual values
    m_pActualValuesDSP = m_pDSPIFace->getMemHandle("ActualValues", DSPDATA::sLocal);
    m_pActualValuesDSP->addVarItem( new cDspVar("CHXPEAK",m_ChannelList.count(), DSPDATA::vDspResult));
    m_pActualValuesDSP->addVarItem( new cDspVar("CHXRMS",m_ChannelList.count(), DSPDATA::vDspResult));
    m_pActualValuesDSP->addVarItem( new cDspVar("Freq", 1, DSPDATA::vDspResult));

    m_pModuleActualValues.resize(m_pActualValuesDSP->getDataCount()); // we provide a vector for generated actual values
}


void cRangeModuleMeasProgram::deleteDspVarList()
{
    m_pDSPIFace->deleteDSPMemHandle(m_pTmpDataDsp);
    m_pDSPIFace->deleteDSPMemHandle(m_pParameterDSP);
    m_pDSPIFace->deleteDSPMemHandle(m_pActualValuesDSP);
}


void cRangeModuleMeasProgram::setDspCmdList()
{
    QString s;

    m_pDSPIFace->addCycListItem( s = "STARTCHAIN(1,1,0x0101)"); // aktiv, prozessnr. (dummy),hauptkette 1 subkette 1 start
        m_pDSPIFace->addCycListItem( s = QString("CLEARN(%1,MESSSIGNAL0)").arg(504) ); // clear meassignal
        m_pDSPIFace->addCycListItem( s = "SETVAL(TIPar,1000 /* !!! variabel */)"); // initial ti time
        m_pDSPIFace->addCycListItem( s = "GETSTIME(TISTART)"); // einmal ti start setzen
        m_pDSPIFace->addCycListItem( s = "DEACTIVATECHAIN(1,0x0101)"); // ende prozessnr., hauptkette 1 subkette 1
    m_pDSPIFace->addCycListItem( s = "STOPCHAIN(1,0x0101)"); // ende prozessnr., hauptkette 1 subkette 1

    for (int i = 0; i < m_ChannelList.count(); i++)
    {
        cRangeMeasChannel* mchn = m_pModule->getMeasChannel(m_ChannelList.at(i));
        m_pDSPIFace->addCycListItem( s = QString("COPYDATA(CH%1,0,MEASSIGNAL").arg(mchn->getDSPChannelNr())); // for each channel we work on
        m_pDSPIFace->addCycListItem( s = QString("SETPEAK(MEASSIGNAL,CHXPEAK+%1").arg(i));
        m_pDSPIFace->addCycListItem( s = QString("RMS(MEASSIGNAL,CHXRMS+%1").arg(i));
    }

    m_pDSPIFace->addCycListItem( s = "COPYDU(1,FREQENCY,FREQ)");

    m_pDSPIFace->addCycListItem( s = "TESTTIMESKIPNEX(TISTART,TIPAR)");
    m_pDSPIFace->addCycListItem( s = "ACTIVATECHAIN(1,0x0102)");

    m_pDSPIFace->addCycListItem( s = "STARTCHAIN(0,1,0x0102)");
        m_pDSPIFace->addCycListItem( s = "GETSTIME(TISTART)"); // set new system time
        m_pDSPIFace->addCycListItem( s = "DSPINTTRIGGER(0x0,0x0001)"); // send interrupt to module
        m_pDSPIFace->addCycListItem( s = "DEACTIVATECHAIN(1,0x0102)");
    m_pDSPIFace->addCycListItem( s = "STOPCHAIN(1,0x0102)"); // end processnr., mainchain 1 subchain 2
}


void cRangeModuleMeasProgram::deleteDspCmdList()
{
    m_pDSPIFace->clearCmdList();
}


void cRangeModuleMeasProgram::catchInterfaceAnswer(quint32 msgnr, QVariant answer)
{
    bool ok;

    if (msgnr == 0) // 0 was reserved for async. messages
    {
        QString sintnr;
        sintnr = answer.toString().section(':', 1, 1);
        int service = sintnr.toInt(&ok);
        switch (service)
        {
        case 1:
            // we got an interrupt from our cmd chain and have to fetch our actual values
            m_dataAcquisitionMachine.start();
            break;
        }
    }
    else
    {
        int cmd = m_MsgNrCmdList.take(msgnr);
        switch (cmd)
        {
        case varlist2dsp:
            if (answer.toInt(&ok) == ack)
                emit activationContinue();
            else
                emit activationError();
            break;
        case cmdlist2dsp:
            if (answer.toInt(&ok) == ack)
                emit activationContinue();
            else
                emit activationError();
            break;
        case activatedsp:
            if (answer.toInt(&ok) == ack)
                emit activationContinue();
            else
                emit activationError();
            break;
        case dataaquistion:
            if (answer.toInt(&ok) == ack)
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


void cRangeModuleMeasProgram::setInterfaceActualValues(QVector<double> *actualValues)
{
    for (int i = 0; i < m_EntityList.count(); i++)
        m_EntityList.at(i)->setValue(QVariant((*actualValues)[i]));
}


void cRangeModuleMeasProgram::varList2DSP()
{
    m_MsgNrCmdList[m_pDSPIFace->varList2Dsp()] = varlist2dsp;
}


void cRangeModuleMeasProgram::cmdList2DSP()
{
    m_MsgNrCmdList[m_pDSPIFace->cmdLists2Dsp()] = cmdlist2dsp;
}


void cRangeModuleMeasProgram::activateDSP()
{
    m_MsgNrCmdList[m_pDSPIFace->activateInterface()] = activatedsp; // aktiviert die var- und cmd-listen im dsp
}


void cRangeModuleMeasProgram::activateDSPdone()
{
    emit activated();
}


void cRangeModuleMeasProgram::deactivateDSP()
{
    m_pDSPIFace->deactivateInterface();
}


void cRangeModuleMeasProgram::deactivateDSPdone()
{
    emit deactivated();
}


void cRangeModuleMeasProgram::dataAcquisitionDSP()
{
    m_MsgNrCmdList[m_pDSPIFace->dataAcquisition(m_pActualValuesDSP)] = dataaquistion; // we start our data aquisition now
}


void cRangeModuleMeasProgram::dataReadDSP()
{
    float *source = m_pDSPIFace->data(m_pActualValuesDSP);
    double *dest = m_pModuleActualValues.data();

    // read values from dsp interface
    for (int i = 0; i < m_pModuleActualValues.count(); i++, dest++, source++)
        *dest = *source;

    emit actualValues(&m_pModuleActualValues); // and send them
}







