#include "adjustment.h"
#include "rangemodule.h"
#include "rangemeaschannel.h"
#include <errormessages.h>
#include <reply.h>
#include <proxy.h>
#include <vfmodulecomponent.h>
#include <rminterface.h>
#include <dspinterface.h>
#include <QSignalTransition>

namespace RANGEMODULE 
{

cAdjustManagement::cAdjustManagement(cRangeModule *module, cSocket* dspsocket, cSocket* pcbsocket, QStringList chnlist, QStringList subdclist, double interval)
    :m_pModule(module), m_pDSPSocket(dspsocket), m_pPCBSocket(pcbsocket), m_ChannelNameList(chnlist), m_subdcChannelNameList(subdclist), m_fAdjInterval(interval)
{
    m_pDSPInterFace = new Zera::cDSPInterface();
    m_pPCBInterface = new Zera::cPCBInterface();

    m_bAdjustTrigger = false;
    for (int i = 0; i < m_ChannelNameList.count(); i++) // we fetch all our real channels first
        m_ChannelList.append(m_pModule->getMeasChannel(m_ChannelNameList.at(i)));
    for (int i = 0; i < m_subdcChannelNameList.count(); i++) // we fetch all our real channels first
        m_subDCChannelList.append(m_pModule->getMeasChannel(m_subdcChannelNameList.at(i)));

    // and then set up our state machines
    m_readGainCorrState.addTransition(this, &cAdjustManagement::activationContinue, &m_readPhaseCorrState);
    m_readPhaseCorrState.addTransition(this, &cAdjustManagement::activationContinue, &m_readOffsetCorrState);
    m_readOffsetCorrState.addTransition(this, &cAdjustManagement::activationContinue, &m_setSubDCState);
    m_setSubDCState.addTransition(this, &cAdjustManagement::activationContinue, &m_activationDoneState);
    m_activationMachine.addState(&m_pcbserverConnectState);
    m_activationMachine.addState(&m_dspserverConnectState);
    m_activationMachine.addState(&m_readGainCorrState);
    m_activationMachine.addState(&m_readPhaseCorrState);
    m_activationMachine.addState(&m_readOffsetCorrState);
    m_activationMachine.addState(&m_setSubDCState);
    m_activationMachine.addState(&m_activationDoneState);
    m_activationMachine.setInitialState(&m_pcbserverConnectState);
    connect(&m_pcbserverConnectState, &QState::entered, this, &cAdjustManagement::pcbserverConnect);
    connect(&m_dspserverConnectState, &QState::entered, this, &cAdjustManagement::dspserverConnect);
    connect(&m_readGainCorrState, &QState::entered, this, &cAdjustManagement::readGainCorr);
    connect(&m_readPhaseCorrState, &QState::entered, this, &cAdjustManagement::readPhaseCorr);
    connect(&m_readOffsetCorrState, &QState::entered, this, &cAdjustManagement::readOffsetCorr);
    connect(&m_setSubDCState, &QState::entered, this, &cAdjustManagement::setSubDC);
    connect(&m_activationDoneState, &QState::entered, this, &cAdjustManagement::activationDone);

    m_deactivationInitState.addTransition(this, &cAdjustManagement::deactivationContinue, &m_deactivationDoneState);
    m_deactivationMachine.addState(&m_deactivationInitState);
    m_deactivationMachine.addState(&m_deactivationDoneState);
    m_deactivationMachine.setInitialState(&m_deactivationInitState);
    connect(&m_deactivationInitState, &QState::entered, this, &cAdjustManagement::deactivationInit);
    connect(&m_deactivationDoneState, &QState::entered, this, &cAdjustManagement::deactivationDone);

    m_writeGainCorrState.addTransition(this, &cAdjustManagement::activationContinue, &m_writePhaseCorrState);
    m_writePhaseCorrState.addTransition(this, &cAdjustManagement::activationContinue, &m_writeOffsetCorrState);
    m_writeOffsetCorrState.addTransition(this, &cAdjustManagement::activationContinue, &m_writeCorrDoneState);
    m_writeCorrectionDSPMachine.addState(&m_writeGainCorrState);
    m_writeCorrectionDSPMachine.addState(&m_writePhaseCorrState);
    m_writeCorrectionDSPMachine.addState(&m_writeOffsetCorrState);
    m_writeCorrectionDSPMachine.addState(&m_writeCorrDoneState);
    m_writeCorrectionDSPMachine.setInitialState(&m_writeGainCorrState);
    connect(&m_writeGainCorrState, &QState::entered, this, &cAdjustManagement::writeGainCorr);
    connect(&m_writePhaseCorrState, &QState::entered, this, &cAdjustManagement::writePhaseCorr);
    connect(&m_writeOffsetCorrState, &QState::entered, this, &cAdjustManagement::writeOffsetCorr);

    m_getGainCorr1State.addTransition(this, &cAdjustManagement::activationContinue, &m_getGainCorr2State);
    m_getGainCorr2State.addTransition(this, &cAdjustManagement::repeatStateMachine, &m_getGainCorr1State);
    m_getGainCorr2State.addTransition(this, &cAdjustManagement::activationContinue, &m_getPhaseCorr1State);
    m_getPhaseCorr1State.addTransition(this, &cAdjustManagement::activationContinue, &m_getPhaseCorr2State);
    m_getPhaseCorr2State.addTransition(this, &cAdjustManagement::repeatStateMachine, &m_getPhaseCorr1State);
    m_getPhaseCorr2State.addTransition(this, &cAdjustManagement::activationContinue, &m_getOffsetCorr1State);
    m_getOffsetCorr1State.addTransition(this, &cAdjustManagement::activationContinue, &m_getOffsetCorr2State);
    m_getOffsetCorr2State.addTransition(this, &cAdjustManagement::repeatStateMachine, &m_getOffsetCorr1State);
    m_getOffsetCorr2State.addTransition(this, &cAdjustManagement::activationContinue, &m_adjustDoneState);
    m_adjustMachine.addState(&m_getGainCorr1State);
    m_adjustMachine.addState(&m_getGainCorr2State);
    m_adjustMachine.addState(&m_getPhaseCorr1State);
    m_adjustMachine.addState(&m_getPhaseCorr2State);
    m_adjustMachine.addState(&m_getOffsetCorr1State);
    m_adjustMachine.addState(&m_getOffsetCorr2State);
    m_adjustMachine.addState(&m_adjustDoneState);
    m_adjustMachine.setInitialState(&m_getGainCorr1State);
    connect(&m_getGainCorr1State, &QState::entered, this, &cAdjustManagement::getGainCorr1);
    connect(&m_getGainCorr2State, &QState::entered, this, &cAdjustManagement::getGainCorr2);
    connect(&m_getPhaseCorr1State, &QState::entered, this, &cAdjustManagement::getPhaseCorr1);
    connect(&m_getPhaseCorr2State, &QState::entered, this, &cAdjustManagement::getPhaseCorr2);
    connect(&m_getOffsetCorr1State, &QState::entered, this, &cAdjustManagement::getOffsetCorr1);
    connect(&m_getOffsetCorr2State, &QState::entered, this, &cAdjustManagement::getOffsetCorr2);
    connect(&m_adjustDoneState, &QState::entered, this, &cAdjustManagement::getCorrDone);
}


cAdjustManagement::~cAdjustManagement()
{
    delete m_pDSPInterFace;
}


void cAdjustManagement::ActionHandler(QVector<float> *actualValues)
{
    m_nChannelIt = 0; // we start with first channel
    // in case measurement is faster than adjusting
    if (m_bActive && m_bAdjustTrigger && !m_adjustMachine.isRunning())
    {
        m_bAdjustTrigger = false;
        m_adjustMachine.start();
    }
}


void cAdjustManagement::generateInterface()
{
    m_pAdjustmentInfo = new VfModuleComponent(m_pModule->getEntityId(), m_pModule->m_pModuleValidator,
                                                 QString("INF_Adjusted"),
                                                 QString("Adjustment information"),
                                                 QVariant(0));

    m_pModule->veinModuleComponentList.append(m_pAdjustmentInfo);
}


void cAdjustManagement::pcbserverConnect()
{
    // we connect cmddone of our meas channels so we get informed if commands are finished
    for (int i = 0; i < m_ChannelList.count(); i++)
        connect(m_ChannelList.at(i), &cRangeMeasChannel::cmdDone, this, &cAdjustManagement::catchChannelReply);

    // we set up our pcb server connection
    m_pPCBClient = Zera::Proxy::getInstance()->getConnection(m_pPCBSocket->m_sIP, m_pPCBSocket->m_nPort);
    m_pPCBInterface->setClient(m_pPCBClient);
    m_pcbserverConnectState.addTransition(m_pPCBClient, &Zera::ProxyClient::connected, &m_dspserverConnectState);
    connect(m_pPCBInterface, &Zera::cPCBInterface::serverAnswer, this, &cAdjustManagement::catchInterfaceAnswer);
    Zera::Proxy::getInstance()->startConnection(m_pPCBClient);
}


void cAdjustManagement::dspserverConnect()
{
    // we set up our dsp server connection
    m_pDspClient = Zera::Proxy::getInstance()->getConnection(m_pDSPSocket->m_sIP, m_pDSPSocket->m_nPort);
    m_pDSPInterFace->setClient(m_pDspClient);
    m_dspserverConnectState.addTransition(m_pDspClient, &Zera::ProxyClient::connected, &m_readGainCorrState);
    connect(m_pDSPInterFace, &Zera::cDSPInterface::serverAnswer, this, &cAdjustManagement::catchInterfaceAnswer);
    Zera::Proxy::getInstance()->startConnection(m_pDspClient);
}


void cAdjustManagement::readGainCorr()
{
    // we fetch a handle for each gain-, phase, offset correction for
    // all possible channels because we do not know which channels become active

    m_pGainCorrectionDSP = m_pDSPInterFace->getMemHandle("GainCorrection");
    m_pGainCorrectionDSP->addVarItem( new cDspVar("GAINCORRECTION",32, DSPDATA::vDspIntVar));
    m_fGainCorr = m_pDSPInterFace->data(m_pGainCorrectionDSP, "GAINCORRECTION");

    m_pPhaseCorrectionDSP = m_pDSPInterFace->getMemHandle("PhaseCorrection");
    m_pPhaseCorrectionDSP->addVarItem( new cDspVar("PHASECORRECTION",32, DSPDATA::vDspIntVar));
    m_fPhaseCorr = m_pDSPInterFace->data(m_pPhaseCorrectionDSP, "PHASECORRECTION");

    m_pOffsetCorrectionDSP = m_pDSPInterFace->getMemHandle("OffsetCorrection");
    m_pOffsetCorrectionDSP->addVarItem( new cDspVar("OFFSETCORRECTION",32, DSPDATA::vDspIntVar));
    m_fOffsetCorr = m_pDSPInterFace->data(m_pOffsetCorrectionDSP, "OFFSETCORRECTION");

    m_MsgNrCmdList[m_pDSPInterFace->dspMemoryRead(m_pGainCorrectionDSP)] = readgaincorr;
}


void cAdjustManagement::readPhaseCorr()
{
    m_MsgNrCmdList[m_pDSPInterFace->dspMemoryRead(m_pPhaseCorrectionDSP)] = readphasecorr;
}


void cAdjustManagement::readOffsetCorr()
{
    m_MsgNrCmdList[m_pDSPInterFace->dspMemoryRead(m_pOffsetCorrectionDSP)] = readoffsetcorr;
}


void cAdjustManagement::setSubDC()
{
    quint32 subdc = 0;
    for (int i = 0; i < m_subDCChannelList.count(); i++) {
        subdc |= (1 << m_subDCChannelList.at(i)->getDSPChannelNr());
    }
    cDspMeasData* pSubDCMaskDSP = m_pDSPInterFace->getMemHandle("SubDC"); // here we can set if sub dc or not
    pSubDCMaskDSP->addVarItem( new cDspVar("SUBDC",1, DSPDATA::vDspIntVar, DSPDATA::dInt));
    m_pDSPInterFace->setVarData(pSubDCMaskDSP, QString("SUBDC:%1;").arg(subdc), DSPDATA::dInt);
    m_MsgNrCmdList[m_pDSPInterFace->dspMemoryWrite(pSubDCMaskDSP)] = subdcdsp;
}


void cAdjustManagement::activationDone()
{
    m_AdjustTimer.start(m_fAdjInterval*1000.0);
    connect(&m_AdjustTimer, &QTimer::timeout, this, &cAdjustManagement::adjustPrepare);
    m_bActive = true;
    emit activated();
}


void cAdjustManagement::deactivationInit()
{
    m_bActive = false;
    Zera::Proxy::getInstance()->releaseConnection(m_pDspClient);
    disconnect(m_pDSPInterFace, 0, this, 0);

    for (int i = 0; i < m_ChannelList.count(); i++)
        disconnect(m_ChannelList.at(i), 0, this, 0);

    m_pDSPInterFace->deleteMemHandle(m_pGainCorrectionDSP);
    m_pDSPInterFace->deleteMemHandle(m_pPhaseCorrectionDSP);
    m_pDSPInterFace->deleteMemHandle(m_pOffsetCorrectionDSP);

    emit deactivationContinue();
}


void cAdjustManagement::deactivationDone()
{
    emit deactivated();
}


void cAdjustManagement::writeGainCorr()
{
    if (m_bActive)
        m_MsgNrCmdList[m_pDSPInterFace->dspMemoryWrite(m_pGainCorrectionDSP)] = writegaincorr;
}


void cAdjustManagement::writePhaseCorr()
{
    if (m_bActive)
        m_MsgNrCmdList[m_pDSPInterFace->dspMemoryWrite(m_pPhaseCorrectionDSP)] = writephasecorr;
}


void cAdjustManagement::writeOffsetCorr()
{
    if (m_bActive)
        m_MsgNrCmdList[m_pDSPInterFace->dspMemoryWrite(m_pOffsetCorrectionDSP)] = writeoffsetcorr;
}


void cAdjustManagement::getGainCorr1()
{
    if (m_bActive){
        double actualValue=m_ChannelList.at(m_nChannelIt)->getRmsValue();
        double preScalingFact=m_ChannelList[m_nChannelIt]->getPreScaling();
        m_MsgNrCmdList[m_ChannelList.at(m_nChannelIt)->readGainCorrection(actualValue*preScalingFact)] = getgaincorr;
    }
}


void cAdjustManagement::getGainCorr2()
{
    cRangeMeasChannel *measChannel;
    float fCorr;

    if (m_bActive)
    {
        measChannel = m_ChannelList.at(m_nChannelIt);
        fCorr = measChannel->getGainCorrection();
        m_fGainCorr[measChannel->getDSPChannelNr()] = fCorr;
        m_nChannelIt++;
        if (m_nChannelIt < m_ChannelNameList.count())
            emit repeatStateMachine();
        else
        {
            m_nChannelIt = 0;
            emit activationContinue();
        }
    }
}


void cAdjustManagement::getPhaseCorr1()
{
    if (m_bActive){
        double frequency=m_ChannelList.at(m_nChannelIt)->getSignalFrequency();
        m_MsgNrCmdList[m_ChannelList.at(m_nChannelIt)->readPhaseCorrection(frequency)] = getphasecorr;
    }
}


void cAdjustManagement::getPhaseCorr2()
{
    cRangeMeasChannel *measChannel;
    float fCorr;

    if (m_bActive)
    {
        measChannel = m_ChannelList.at(m_nChannelIt);
        fCorr = measChannel->getPhaseCorrection();
        m_fPhaseCorr[measChannel->getDSPChannelNr()] = fCorr;
        m_nChannelIt++;
        if (m_nChannelIt < m_ChannelNameList.count())
            emit repeatStateMachine();
        else
        {
            m_nChannelIt = 0;
            emit activationContinue();
        }
    }
}


void cAdjustManagement::getOffsetCorr1()
{
    if (m_bActive){
        double actualValue=m_ChannelList.at(m_nChannelIt)->getRmsValue();
        double preScalingFact=m_ChannelList[m_nChannelIt]->getPreScaling();
        m_MsgNrCmdList[m_ChannelList.at(m_nChannelIt)->readOffsetCorrection(actualValue*preScalingFact)] = getoffsetcore;
    }
}


void cAdjustManagement::getOffsetCorr2()
{
    cRangeMeasChannel *measChannel;
    float fCorr;

    if (m_bActive)
    {
        measChannel = m_ChannelList.at(m_nChannelIt);
        fCorr = measChannel->getOffsetCorrection();
        double preScale=measChannel->getPreScaling();
        // Offset is an summand. It must be multiplied with the
        // scale factor.
        m_fOffsetCorr[measChannel->getDSPChannelNr()] = fCorr*preScale;

        m_nChannelIt++;
        if (m_nChannelIt < m_ChannelNameList.count())
            emit repeatStateMachine();
        else
            emit activationContinue();
    }
}


void cAdjustManagement::getCorrDone()
{
    m_nChannelIt = 0;
    m_writeCorrectionDSPMachine.start(); // we have all correction data and write it to dsp
}


void cAdjustManagement::catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer)
{
    bool ok;

    if (msgnr == 0) // 0 was reserved for async. messages
    {
        // that we will ignore
    }
    else
    {
        // because rangemodulemeasprogram, adjustment and rangeobsermatic share the same dsp interface
        if (m_MsgNrCmdList.contains(msgnr))
        {
            int cmd = m_MsgNrCmdList.take(msgnr);
            switch (cmd)
            {
            case readgaincorr:
                if (reply == ack)
                    emit activationContinue();
                else
                {
                    emit errMsg((tr(readdspgaincorrErrMsg)));
                    emit activationError();
                }
                break;
            case readphasecorr:
                if (reply == ack)
                    emit activationContinue();
                else
                {
                    emit errMsg((tr(readdspphasecorrErrMsg)));
                    emit activationError();
                }
                break;
            case readoffsetcorr:
                if (reply == ack)
                    emit activationContinue();
                else
                {
                    emit errMsg((tr(readdspoffsetcorrErrMsg)));
                    emit activationError();
                }
                break;

            case subdcdsp:
                if (reply == ack)
                    emit activationContinue();
                else
                {
                    emit errMsg((tr(writesubdcErrMsg)));
                    emit activationError();
                }
                break;


            case writegaincorr:
                if (reply == ack)
                    emit activationContinue();
                else
                {
                    // perhaps we emit some error here ?
                    emit errMsg((tr(writedspgaincorrErrMsg)));
                }
                break;

            case writephasecorr:
                if (reply == ack)
                    emit activationContinue();
                else
                {
                    // perhaps we emit some error here ?
                    emit errMsg((tr(writedspphasecorrErrMsg)));
                }
                break;

            case writeoffsetcorr:
                if (reply == ack)
                    emit activationContinue();
                else
                {
                    // perhaps we emit some error here ?
                    emit errMsg((tr(writedspoffsetcorrErrMsg)));
                }
                break;

            }
        }
    }
}

void cAdjustManagement::catchChannelReply(quint32 msgnr)
{
    int cmd = m_MsgNrCmdList.take(msgnr);
    switch (cmd)
    {
    case getgaincorr:
    case getphasecorr:
    case getoffsetcore:
        emit activationContinue();
        break;
    }
}

void cAdjustManagement::adjustPrepare()
{
    m_bAdjustTrigger = true;
}

}








