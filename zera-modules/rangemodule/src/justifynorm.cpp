#include <QSignalTransition>
#include <dspinterface.h>

#include "justifynorm.h"
#include "rangemodule.h"
#include "rangemeaschannel.h"


cJustifyNormManagement::cJustifyNormManagement(cRangeModule *module, VeinPeer* peer, Zera::Server::cDSPInterface* iface, QStringList chnlist)
    :m_pRangemodule(module), m_pPeer(peer), m_pDSPIFace(iface), m_ChannelNameList(chnlist)
{
    generateInterface();

    for (int i = 0; i < m_ChannelNameList.count(); i++) // we fetch all our real channels first
        m_ChannelList.append(m_pRangemodule->getMeasChannel(m_ChannelNameList.at(i)));

    m_readGainCorrState.addTransition(this, SIGNAL(activationContinue()), &m_readPhaseCorrState);
    m_readPhaseCorrState.addTransition(this, SIGNAL(activationContinue()), &m_readOffsetCorrState);
    m_readOffsetCorrState.addTransition(this, SIGNAL(activationContinue()), &m_readCorrDoneState);
    m_readCorrectionDSPMachine.addState(&m_readGainCorrState);
    m_readCorrectionDSPMachine.addState(&m_readPhaseCorrState);
    m_readCorrectionDSPMachine.addState(&m_readOffsetCorrState);
    m_readCorrectionDSPMachine.addState(&m_readCorrDoneState);
    m_readCorrectionDSPMachine.setInitialState(&m_readGainCorrState);
    connect(&m_readGainCorrState, SIGNAL(entered()), SLOT(readGainCorr()));
    connect(&m_readPhaseCorrState, SIGNAL(entered()), SLOT(readPhaseCorr()));
    connect(&m_readOffsetCorrState, SIGNAL(entered()), SLOT(readOffsetCorr()));
    connect(&m_readCorrDoneState, SIGNAL(entered()), SLOT(readCorrDone()));

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
    m_getPhaseCorr2State.addTransition(this, SIGNAL(repeatStateMachine()), &m_getPhaseCorr1State);
    m_getPhaseCorr2State.addTransition(this, SIGNAL(activationContinue()), &m_getOffsetCorr1State);
    m_getOffsetCorr1State.addTransition(this, SIGNAL(activationContinue()), &m_getOffsetCorr2State);
    m_getOffsetCorr2State.addTransition(this, SIGNAL(repeatStateMachine()), &m_getOffsetCorr1State);
    m_getPhaseCorr2State.addTransition(this, SIGNAL(activationContinue()), &m_adjNormDoneState);
    m_adjNormMachine.addState(&m_getGainCorr1State);
    m_adjNormMachine.addState(&m_getGainCorr2State);
    m_adjNormMachine.addState(&m_getPhaseCorr1State);
    m_adjNormMachine.addState(&m_getPhaseCorr2State);
    m_adjNormMachine.addState(&m_getOffsetCorr1State);
    m_adjNormMachine.addState(&m_getOffsetCorr2State);
    m_adjNormMachine.addState(&m_adjNormDoneState);
    m_adjNormMachine.setInitialState(&m_getGainCorr1State);
    connect(&m_getGainCorr1State, SIGNAL(entered()), SLOT(getGainCorr1()));
    connect(&m_getGainCorr2State, SIGNAL(entered()), SLOT(getGainCorr2()));
    connect(&m_getPhaseCorr1State, SIGNAL(entered()), SLOT(getPhaseCorr1()));
    connect(&m_getPhaseCorr2State, SIGNAL(entered()), SLOT(getPhaseCorr2()));
    connect(&m_getOffsetCorr1State, SIGNAL(entered()), SLOT(getOffsetCorr1()));
    connect(&m_getOffsetCorr2State, SIGNAL(entered()), SLOT(getOffsetCorr2()));
    connect(&m_adjNormDoneState, SIGNAL(entered()), SLOT(getCorrDone()));
}


cJustifyNormManagement::~cJustifyNormManagement()
{
    deleteInterface();
}


void cJustifyNormManagement::activate()
{
    // we fetch a handle for each gain-, phase, offset correction for
    // all possible channels because we do not know which channels become active


    for (int i = 0; i < m_ChannelList.count(); i++)
        connect(m_ChannelList.at(i), SIGNAL(cmdDone(quint32)), SLOT(catchChannelReply(quint32)));

    connect(m_pDSPIFace, SIGNAL(serverAnswer(quint32, QVariant)), SLOT(catchInterfaceAnswer(quint32,QVariant)));

    m_pGainCorrectionDSP = m_pDSPIFace->getMemHandle("GainCorrection", DSPDATA::sGlobal);
    m_pGainCorrectionDSP->addVarItem( new cDspVar("GAINCORRECTION",32, DSPDATA::vDspIntVar));
    m_pPhaseCorrectionDSP = m_pDSPIFace->getMemHandle("PhaseCorrection", DSPDATA::sGlobal);
    m_pPhaseCorrectionDSP->addVarItem( new cDspVar("PHASECORRECTION",32, DSPDATA::vDspIntVar));
    m_pOffsetCorrectionDSP = m_pDSPIFace->getMemHandle("GainCorrection", DSPDATA::sGlobal);
    m_pOffsetCorrectionDSP->addVarItem( new cDspVar("OFFSETCORRECTION",32, DSPDATA::vDspIntVar));

    m_readCorrectionDSPMachine.start(); // we read all correction data once
}


void cJustifyNormManagement::deactivate()
{
    m_pDSPIFace->deleteDSPMemHandle(m_pGainCorrectionDSP);
    m_pDSPIFace->deleteDSPMemHandle(m_pPhaseCorrectionDSP);
    m_pDSPIFace->deleteDSPMemHandle(m_pOffsetCorrectionDSP);
}


void cJustifyNormManagement::ActionHandler(QVector<double> *actualValues)
{
    m_ActualValues = *actualValues;
    m_nChannelIt = 0; // we start with first channel
    m_adjNormMachine.start();
}


void cJustifyNormManagement::generateInterface()
{
    // at the moment no interface
}


void cJustifyNormManagement::deleteInterface()
{
    // at the moment no interface
}


void cJustifyNormManagement::readGainCorr()
{
    m_MsgNrCmdList[m_pDSPIFace->dspMemoryRead(m_pGainCorrectionDSP)] = readgaincorr;
}


void cJustifyNormManagement::readPhaseCorr()
{
    m_MsgNrCmdList[m_pDSPIFace->dspMemoryRead(m_pPhaseCorrectionDSP)] = readphasecorr;
}


void cJustifyNormManagement::readOffsetCorr()
{
    m_MsgNrCmdList[m_pDSPIFace->dspMemoryRead(m_pOffsetCorrectionDSP)] = readoffsetcorr;
}


void cJustifyNormManagement::readCorrDone()
{
    emit activated();
}


void cJustifyNormManagement::writeGainCorr()
{
    m_MsgNrCmdList[m_pDSPIFace->dspMemoryWrite(m_pGainCorrectionDSP, DSPDATA::dFloat)] = writegaincorr;
}


void cJustifyNormManagement::writePhaseCorr()
{
    m_MsgNrCmdList[m_pDSPIFace->dspMemoryWrite(m_pPhaseCorrectionDSP, DSPDATA::dFloat)] = writephasecorr;
}


void cJustifyNormManagement::writeOffsetCorr()
{
    m_MsgNrCmdList[m_pDSPIFace->dspMemoryWrite(m_pOffsetCorrectionDSP, DSPDATA::dFloat)] = writeoffsetcorr;
}


void cJustifyNormManagement::getGainCorr1()
{
    m_MsgNrCmdList[m_ChannelList.at(m_nChannelIt)->readGainCorrection(m_ActualValues[m_nChannelIt+m_ChannelNameList.count()])] = getgaincorr;
}


void cJustifyNormManagement::getGainCorr2()
{
    cRangeMeasChannel *measChannel;
    double fCorrNorm;
    float* corr;

    measChannel = m_ChannelList.at(m_nChannelIt);
    fCorrNorm = measChannel->getGainCorrection() * measChannel->getUrValue() / measChannel->getRejection();
    corr = m_pGainCorrectionDSP->data();
    corr[measChannel->getDSPChannelNr()] = fCorrNorm;

    m_nChannelIt++;
    if (m_nChannelIt < m_ChannelNameList.count())
        emit repeatStateMachine();
    else
    {
        m_nChannelIt = 0;
        emit activationContinue();
    }
}


void cJustifyNormManagement::getPhaseCorr1()
{
    m_MsgNrCmdList[m_ChannelList.at(m_nChannelIt)->readPhaseCorrection(m_ActualValues[2 * m_ChannelNameList.count()])] = getphasecorr;
}


void cJustifyNormManagement::getPhaseCorr2()
{
    cRangeMeasChannel *measChannel;
    double fCorr;
    float* corr;

    measChannel = m_ChannelList.at(m_nChannelIt);
    fCorr = measChannel->getPhaseCorrection();
    corr = m_pPhaseCorrectionDSP->data();
    corr[measChannel->getDSPChannelNr()] = fCorr;

    m_nChannelIt++;
    if (m_nChannelIt < m_ChannelNameList.count())
        emit repeatStateMachine();
    else
    {
        m_nChannelIt = 0;
        emit activationContinue();
    }
}


void cJustifyNormManagement::getOffsetCorr1()
{
    m_MsgNrCmdList[m_ChannelList.at(m_nChannelIt)->readOffsetCorrection(m_ActualValues[m_nChannelIt+m_ChannelNameList.count()])] = getoffsetcore;
}


void cJustifyNormManagement::getOffsetCorr2()
{
    cRangeMeasChannel *measChannel;
    double fCorr;
    float* corr;

    measChannel = m_ChannelList.at(m_nChannelIt);
    fCorr = measChannel->getOffsetCorrection();
    corr = m_pOffsetCorrectionDSP->data();
    corr[measChannel->getDSPChannelNr()] = fCorr;

    m_nChannelIt++;
    if (m_nChannelIt < m_ChannelNameList.count())
        emit repeatStateMachine();
    else
        emit activationContinue();
}


void cJustifyNormManagement::getCorrDone()
{
    m_nChannelIt = 0;
    m_writeCorrectionDSPMachine.start(); // we have all correction data and write it to dsp
}


void cJustifyNormManagement::catchInterfaceAnswer(quint32 msgnr, QVariant answer)
{
    bool ok;

    if (msgnr == 0) // 0 was reserved for async. messages
    {
        // that we will ignore
    }
    else
    {
        int cmd = m_MsgNrCmdList.take(msgnr);
        switch (cmd)
        {
        case readgaincorr:
        case readphasecorr:
        case readoffsetcorr:
            if (answer.toInt(&ok) == ack)
                emit activationContinue();
            else
                emit activationError();
            break;

        case writegaincorr:
        case writephasecorr:
        case writeoffsetcorr:
            if (answer.toInt(&ok) == ack)
                emit activationContinue();
            else
                break; // perhaps we emit some error here ?
            break;

        }
    }
}

void cJustifyNormManagement::catchChannelReply(quint32 msgnr)
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










