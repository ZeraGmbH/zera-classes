#include <QSignalTransition>
#include <dspinterface.h>
#include <proxy.h>
#include <proxyclient.h>

#include "debug.h"
#include "errormessages.h"
#include "adjustment.h"
#include "rangemodule.h"
#include "rangemeaschannel.h"

namespace RANGEMODULE 
{

cAdjustManagement::cAdjustManagement(cRangeModule *module,  Zera::Proxy::cProxy* proxy, VeinPeer* peer,  cSocket* dspsocket, QStringList chnlist, double interval)
    :m_pModule(module), m_pProxy(proxy), m_pPeer(peer), m_pDSPSocket(dspsocket), m_ChannelNameList(chnlist), m_fAdjInterval(interval)
{
    m_pDSPInterFace = new Zera::Server::cDSPInterface();

    m_bAdjustTrigger = false;
    for (int i = 0; i < m_ChannelNameList.count(); i++) // we fetch all our real channels first
        m_ChannelList.append(m_pModule->getMeasChannel(m_ChannelNameList.at(i)));

    // and then set up our state machines
    m_readGainCorrState.addTransition(this, SIGNAL(activationContinue()), &m_readPhaseCorrState);
    m_readPhaseCorrState.addTransition(this, SIGNAL(activationContinue()), &m_readOffsetCorrState);
    m_readOffsetCorrState.addTransition(this, SIGNAL(activationContinue()), &m_readCorrDoneState);
    m_activationMachine.addState(&m_dspserverConnectState);
    m_activationMachine.addState(&m_readGainCorrState);
    m_activationMachine.addState(&m_readPhaseCorrState);
    m_activationMachine.addState(&m_readOffsetCorrState);
    m_activationMachine.addState(&m_readCorrDoneState);
    m_activationMachine.setInitialState(&m_dspserverConnectState);
    connect(&m_dspserverConnectState, SIGNAL(entered()), SLOT(dspserverConnect()));
    connect(&m_readGainCorrState, SIGNAL(entered()), SLOT(readGainCorr()));
    connect(&m_readPhaseCorrState, SIGNAL(entered()), SLOT(readPhaseCorr()));
    connect(&m_readOffsetCorrState, SIGNAL(entered()), SLOT(readOffsetCorr()));
    connect(&m_readCorrDoneState, SIGNAL(entered()), SLOT(readCorrDone()));

    m_deactivationInitState.addTransition(this, SIGNAL(deactivationContinue()), &m_deactivationDoneState);
    m_deactivationMachine.addState(&m_deactivationInitState);
    m_deactivationMachine.addState(&m_deactivationDoneState);
    m_deactivationMachine.setInitialState(&m_deactivationInitState);
    connect(&m_deactivationInitState, SIGNAL(entered()), SLOT(deactivationInit()));
    connect(&m_deactivationDoneState, SIGNAL(entered()), SLOT(deactivationDone()));

    m_writeGainCorrState.addTransition(this, SIGNAL(activationContinue()), &m_writePhaseCorrState);
    m_writePhaseCorrState.addTransition(this, SIGNAL(activationContinue()), &m_writeOffsetCorrState);
    m_writeOffsetCorrState.addTransition(this, SIGNAL(activationContinue()), &m_writeCorrDoneState);
    m_writeCorrectionDSPMachine.addState(&m_writeGainCorrState);
    m_writeCorrectionDSPMachine.addState(&m_writePhaseCorrState);
    m_writeCorrectionDSPMachine.addState(&m_writeOffsetCorrState);
    m_writeCorrectionDSPMachine.addState(&m_writeCorrDoneState);
    m_writeCorrectionDSPMachine.setInitialState(&m_writeGainCorrState);
    connect(&m_writeGainCorrState, SIGNAL(entered()), SLOT(writeGainCorr()));
    connect(&m_writePhaseCorrState, SIGNAL(entered()), SLOT(writePhaseCorr()));
    connect(&m_writeOffsetCorrState, SIGNAL(entered()), SLOT(writeOffsetCorr()));

    m_getGainCorr1State.addTransition(this, SIGNAL(activationContinue()), &m_getGainCorr2State);
    m_getGainCorr2State.addTransition(this, SIGNAL(repeatStateMachine()), &m_getGainCorr1State);
    m_getGainCorr2State.addTransition(this, SIGNAL(activationContinue()), &m_getPhaseCorr1State);
    m_getPhaseCorr1State.addTransition(this, SIGNAL(activationContinue()), &m_getPhaseCorr2State);
    m_getPhaseCorr2State.addTransition(this, SIGNAL(repeatStateMachine()), &m_getPhaseCorr1State);
    m_getPhaseCorr2State.addTransition(this, SIGNAL(activationContinue()), &m_getOffsetCorr1State);
    m_getOffsetCorr1State.addTransition(this, SIGNAL(activationContinue()), &m_getOffsetCorr2State);
    m_getOffsetCorr2State.addTransition(this, SIGNAL(repeatStateMachine()), &m_getOffsetCorr1State);
    m_getOffsetCorr2State.addTransition(this, SIGNAL(activationContinue()), &m_adjustDoneState);
    m_adjustMachine.addState(&m_getGainCorr1State);
    m_adjustMachine.addState(&m_getGainCorr2State);
    m_adjustMachine.addState(&m_getPhaseCorr1State);
    m_adjustMachine.addState(&m_getPhaseCorr2State);
    m_adjustMachine.addState(&m_getOffsetCorr1State);
    m_adjustMachine.addState(&m_getOffsetCorr2State);
    m_adjustMachine.addState(&m_adjustDoneState);
    m_adjustMachine.setInitialState(&m_getGainCorr1State);
    connect(&m_getGainCorr1State, SIGNAL(entered()), SLOT(getGainCorr1()));
    connect(&m_getGainCorr2State, SIGNAL(entered()), SLOT(getGainCorr2()));
    connect(&m_getPhaseCorr1State, SIGNAL(entered()), SLOT(getPhaseCorr1()));
    connect(&m_getPhaseCorr2State, SIGNAL(entered()), SLOT(getPhaseCorr2()));
    connect(&m_getOffsetCorr1State, SIGNAL(entered()), SLOT(getOffsetCorr1()));
    connect(&m_getOffsetCorr2State, SIGNAL(entered()), SLOT(getOffsetCorr2()));
    connect(&m_adjustDoneState, SIGNAL(entered()), SLOT(getCorrDone()));
}


cAdjustManagement::~cAdjustManagement()
{
    delete m_pDSPInterFace;
}


void cAdjustManagement::ActionHandler(QVector<float> *actualValues)
{
    m_ActualValues = *actualValues;
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
    // at the moment no interface
}


void cAdjustManagement::deleteInterface()
{
    // at the moment no interface
}


void cAdjustManagement::dspserverConnect()
{
    // we connect cmddone of our meas channels so we get informed if commands are finished
    for (int i = 0; i < m_ChannelList.count(); i++)
        connect(m_ChannelList.at(i), SIGNAL(cmdDone(quint32)), SLOT(catchChannelReply(quint32)));

    // we set up our dsp server connection
    m_pDspClient = m_pProxy->getConnection(m_pDSPSocket->m_sIP, m_pDSPSocket->m_nPort);
    m_pDSPInterFace->setClient(m_pDspClient);
    m_dspserverConnectState.addTransition(m_pDspClient, SIGNAL(connected()), &m_readGainCorrState);
    connect(m_pDSPInterFace, SIGNAL(serverAnswer(quint32, quint8, QVariant)), this, SLOT(catchInterfaceAnswer(quint32, quint8, QVariant)));
    m_pProxy->startConnection(m_pDspClient);
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


void cAdjustManagement::readCorrDone()
{
    m_AdjustTimer.start(m_fAdjInterval*1000.0);
    connect(&m_AdjustTimer, SIGNAL(timeout()), this, SLOT(adjustPrepare()));
    m_bActive = true;
    emit activated();
}


void cAdjustManagement::deactivationInit()
{
    m_bActive = false;
    m_pProxy->releaseConnection(m_pDspClient);
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
    m_MsgNrCmdList[m_pDSPInterFace->dspMemoryWrite(m_pGainCorrectionDSP)] = writegaincorr;
}


void cAdjustManagement::writePhaseCorr()
{
    m_MsgNrCmdList[m_pDSPInterFace->dspMemoryWrite(m_pPhaseCorrectionDSP)] = writephasecorr;
}


void cAdjustManagement::writeOffsetCorr()
{
    m_MsgNrCmdList[m_pDSPInterFace->dspMemoryWrite(m_pOffsetCorrectionDSP)] = writeoffsetcorr;
}


void cAdjustManagement::getGainCorr1()
{
    // qDebug() << "Adjustmentstatemachine";
    m_MsgNrCmdList[m_ChannelList.at(m_nChannelIt)->readGainCorrection(m_ActualValues[m_nChannelIt+m_ChannelNameList.count()])] = getgaincorr;
}


void cAdjustManagement::getGainCorr2()
{
    cRangeMeasChannel *measChannel;
    float fCorr;

    measChannel = m_ChannelList.at(m_nChannelIt);
    fCorr = measChannel->getGainCorrection();
    m_fGainCorr[measChannel->getDSPChannelNr()] = fCorr;
    //qDebug() << QString("GainCorr%1: %2").arg(m_nChannelIt).arg(fCorr);
    m_nChannelIt++;
    if (m_nChannelIt < m_ChannelNameList.count())
        emit repeatStateMachine();
    else
    {
        m_nChannelIt = 0;
        emit activationContinue();
    }
}


void cAdjustManagement::getPhaseCorr1()
{
    m_MsgNrCmdList[m_ChannelList.at(m_nChannelIt)->readPhaseCorrection(m_ActualValues[2 * m_ChannelNameList.count()])] = getphasecorr;
}


void cAdjustManagement::getPhaseCorr2()
{
    cRangeMeasChannel *measChannel;
    float fCorr;

    measChannel = m_ChannelList.at(m_nChannelIt);
    fCorr = measChannel->getPhaseCorrection();
    m_fPhaseCorr[measChannel->getDSPChannelNr()] = fCorr;
    // qDebug() << QString("PhaseCorr%1: %2").arg(m_nChannelIt).arg(fCorr);
    m_nChannelIt++;
    if (m_nChannelIt < m_ChannelNameList.count())
        emit repeatStateMachine();
    else
    {
        m_nChannelIt = 0;
        emit activationContinue();
    }
}


void cAdjustManagement::getOffsetCorr1()
{
    m_MsgNrCmdList[m_ChannelList.at(m_nChannelIt)->readOffsetCorrection(m_ActualValues[m_nChannelIt+m_ChannelNameList.count()])] = getoffsetcore;
}


void cAdjustManagement::getOffsetCorr2()
{
    cRangeMeasChannel *measChannel;
    float fCorr;

    measChannel = m_ChannelList.at(m_nChannelIt);
    fCorr = measChannel->getOffsetCorrection();
    m_fOffsetCorr[measChannel->getDSPChannelNr()] = fCorr;
    // qDebug() << QString("OffsetCorr%1: %2").arg(m_nChannelIt).arg(fCorr);

    m_nChannelIt++;
    if (m_nChannelIt < m_ChannelNameList.count())
        emit repeatStateMachine();
    else
        emit activationContinue();
}


void cAdjustManagement::getCorrDone()
{
    m_nChannelIt = 0;
    m_writeCorrectionDSPMachine.start(); // we have all correction data and write it to dsp
}


void cAdjustManagement::catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant)
{
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
#ifdef DEBUG
                    qDebug() << readdspgaincorrErrMsg;
#endif
                    emit activationError();
                }
                break;
            case readphasecorr:
                if (reply == ack)
                    emit activationContinue();
                else
                {
                    emit errMsg((tr(readdspphasecorrErrMsg)));
#ifdef DEBUG
                    qDebug() << readdspphasecorrErrMsg;
#endif
                    emit activationError();
                }
                break;
            case readoffsetcorr:
                if (reply == ack)
                    emit activationContinue();
                else
                {
                    emit errMsg((tr(readdspoffsetcorrErrMsg)));
#ifdef DEBUG
                    qDebug() << readdspoffsetcorrErrMsg;
#endif
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
#ifdef DEBUG
                    qDebug() << writedspgaincorrErrMsg;
#endif
                }
                break;

            case writephasecorr:
                if (reply == ack)
                    emit activationContinue();
                else
                {
                    // perhaps we emit some error here ?
                    emit errMsg((tr(writedspphasecorrErrMsg)));
#ifdef DEBUG
                    qDebug() << writedspphasecorrErrMsg;
#endif
                }
                break;

            case writeoffsetcorr:
                if (reply == ack)
                    emit activationContinue();
                else
                {
                    // perhaps we emit some error here ?
                    emit errMsg((tr(writedspoffsetcorrErrMsg)));
#ifdef DEBUG
                    qDebug() << writedspoffsetcorrErrMsg;
#endif
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








