#include <QSignalTransition>
#include <dspinterface.h>
#include <pcbinterface.h>
#include <proxy.h>
#include <proxyclient.h>
#include <math.h>

#include "debug.h"
#include "errormessages.h"
#include "referenceadjustment.h"
#include "referencemoduleconfigdata.h"
#include "referencemodule.h"
#include "referencemeaschannel.h"

namespace REFERENCEMODULE
{

cReferenceAdjustment::cReferenceAdjustment(cReferenceModule* module, Zera::Proxy::cProxy* proxy, cReferenceModuleConfigData* confData)
    :m_pModule(module), m_pProxy(proxy), m_pConfigData(confData)
{
    m_pDSPInterFace = new Zera::Server::cDSPInterface();
    m_pPCBInterface = new Zera::Server::cPCBInterface();

    for (int i = 0; i < m_pConfigData->m_referenceChannelList.count(); i++) // we fetch all our real channels first
        m_ChannelList.append(m_pModule->getMeasChannel(m_pConfigData->m_referenceChannelList.at(i)));

    // and then set up our state machines
    m_set0VRangeState.addTransition(this, SIGNAL(activationContinue()), &m_dspserverConnectState);
    m_activationMachine.addState(&m_pcbserverConnectState);
    m_activationMachine.addState(&m_set0VRangeState);
    m_activationMachine.addState(&m_dspserverConnectState);
    m_activationMachine.addState(&m_activationDoneState);
    m_activationMachine.setInitialState(&m_pcbserverConnectState);
    connect(&m_pcbserverConnectState, SIGNAL(entered()), SLOT(pcbserverConnect()));
    connect(&m_set0VRangeState, SIGNAL(entered()), SLOT(set0VRange()));
    connect(&m_dspserverConnectState, SIGNAL(entered()), SLOT(dspserverConnect()));
    connect(&m_activationDoneState, SIGNAL(entered()), SLOT(activationDone()));

    m_deactivationInitState.addTransition(this, SIGNAL(deactivationContinue()), &m_deactivationDoneState);
    m_deactivationMachine.addState(&m_deactivationInitState);
    m_deactivationMachine.addState(&m_deactivationDoneState);
    m_deactivationMachine.setInitialState(&m_deactivationInitState);
    connect(&m_deactivationInitState, SIGNAL(entered()), SLOT(deactivationInit()));
    connect(&m_deactivationDoneState, SIGNAL(entered()), SLOT(deactivationDone()));

    m_readGainCorrState.addTransition(this, SIGNAL(adjustContinue()), &m_readOffset2CorrState);
    m_readOffset2CorrState.addTransition(this, SIGNAL(adjustContinue()), &m_writeOffsetAdjustmentState);
    m_writeOffsetAdjustmentState.addTransition(this, SIGNAL(adjustContinue()), &m_set10VRangeState);
    m_set10VRangeState.addTransition(this, SIGNAL(adjustContinue()), &m_referenceAdjustDoneState);
    m_referenceAdjustMachine.addState(&m_readGainCorrState);
    m_referenceAdjustMachine.addState(&m_readOffset2CorrState);
    m_referenceAdjustMachine.addState(&m_writeOffsetAdjustmentState);
    m_referenceAdjustMachine.addState(&m_set10VRangeState);
    m_referenceAdjustMachine.addState(&m_referenceAdjustDoneState);
    m_referenceAdjustMachine.setInitialState(&m_readGainCorrState);
    connect(&m_readGainCorrState, SIGNAL(entered()), SLOT(readGainCorr()));
    connect(&m_readOffset2CorrState, SIGNAL(entered()), SLOT(readOffset2Corr()));
    connect(&m_writeOffsetAdjustmentState, SIGNAL(entered()), SLOT(writeOffsetAdjustment()));
    connect(&m_set10VRangeState, SIGNAL(entered()), SLOT(set10VRange()));
    connect(&m_referenceAdjustDoneState, SIGNAL(entered()), SLOT(referenceAdjustDone()));
}


cReferenceAdjustment::~cReferenceAdjustment()
{
    delete m_pDSPInterFace;
    delete m_pPCBInterface;
}


void cReferenceAdjustment::ActionHandler(QVector<float> *actualValues)
{
    m_ActualValues = *actualValues;

#ifdef DEBUG
    qDebug() << "Reference Module Actual Values received";
    QString s;
    for (int i = 0; i < m_ChannelList.count(); i++)
    {
        double dc = sqrt((m_ActualValues.at(i*2) * m_ActualValues.at(i*2)) + (m_ActualValues.at(i*2+1) * m_ActualValues.at(i*2+1))) / 2.0;

        s = s + QString("DC(%1)=%2;").arg(m_ChannelList.at(i)->getAlias()).arg(dc);
    }

    qDebug() << s;
#endif

    if (m_nIgnoreCount > 0)
    {
        m_nIgnoreCount--;
#ifdef DEBUG
        qDebug() << "Ignored";
#endif
    }
    else
        // m_bactive in case measurement is faster than adjusting
        // adjustMachine.isRunning in caae actual values come faster than adjustment process duration
        if (m_bActive && !m_bAdjustmentDone && !m_referenceAdjustMachine.isRunning())
            m_referenceAdjustMachine.start();
}


void cReferenceAdjustment::generateInterface()
{
    // at the moment no interface
}


void cReferenceAdjustment::deleteInterface()
{
    // at the moment no interface
}


void cReferenceAdjustment::pcbserverConnect()
{
    cSocket sock = m_pConfigData->m_PCBServerSocket;
    m_pPCBClient = m_pProxy->getConnection(sock.m_sIP, sock.m_nPort);
    m_pcbserverConnectState.addTransition(m_pPCBClient, SIGNAL(connected()), &m_set0VRangeState);

    m_pPCBInterface->setClient(m_pPCBClient);
    connect(m_pPCBInterface, SIGNAL(serverAnswer(quint32, quint8, QVariant)), this, SLOT(catchInterfaceAnswer(quint32, quint8, QVariant)));
    m_pProxy->startConnection(m_pPCBClient);
}


void cReferenceAdjustment::set0VRange()
{
    m_nRangeSetPending = m_ChannelList.count();
    for (int i = 0; i <  m_nRangeSetPending; i++)
    {
        m_MsgNrCmdList[m_ChannelList.at(i)->setRange("R0V")] = setrange0V;
    }
}


void cReferenceAdjustment::dspserverConnect()
{
    // we set up our dsp server connection
    cSocket sock = m_pConfigData->m_DSPServerSocket;
    m_pDspClient = m_pProxy->getConnection(sock.m_sIP, sock.m_nPort);
    m_pDSPInterFace->setClient(m_pDspClient);
    m_dspserverConnectState.addTransition(m_pDspClient, SIGNAL(connected()), &m_activationDoneState);
    connect(m_pDSPInterFace, SIGNAL(serverAnswer(quint32, quint8, QVariant)), this, SLOT(catchInterfaceAnswer(quint32, quint8, QVariant)));
    m_pProxy->startConnection(m_pDspClient);
}


void cReferenceAdjustment::activationDone()
{
    // we fetch a handle for gain correction and offset2 correction for
    // all possible channels because we do not know which channels become active

    m_pGainCorrectionDSP = m_pDSPInterFace->getMemHandle("GainCorrection");
    m_pGainCorrectionDSP->addVarItem( new cDspVar("GAINCORRECTION",32, DSPDATA::vDspIntVar));
    m_fGainCorr = m_pDSPInterFace->data(m_pGainCorrectionDSP, "GAINCORRECTION");

    m_pOffset2CorrectionDSP = m_pDSPInterFace->getMemHandle("OffsetCorrection");
    m_pOffset2CorrectionDSP->addVarItem( new cDspVar("OFFSETCORRECTION2",32, DSPDATA::vDspIntVar));
    m_fOffset2Corr = m_pDSPInterFace->data(m_pOffset2CorrectionDSP, "OFFSETCORRECTION2");

    m_nIgnoreCount = m_pConfigData->m_nIgnore;
    m_bAdjustmentDone = false;

    m_bActive = true;
    // emit activated();
    // we will emit activated after reference measurement adjustment is done
}


void cReferenceAdjustment::readGainCorr()
{
    if (m_bActive)
        m_MsgNrCmdList[m_pDSPInterFace->dspMemoryRead(m_pGainCorrectionDSP)] = readgaincorr;
}


void cReferenceAdjustment::readOffset2Corr()
{
    if (m_bActive)
        m_MsgNrCmdList[m_pDSPInterFace->dspMemoryRead(m_pOffset2CorrectionDSP)] = readoffset2corr;
}


void cReferenceAdjustment::writeOffsetAdjustment()
{
    cReferenceMeasChannel* rchn;
    quint8 dspchn;
    float cval;

#ifdef DEBUG
    QVector<float> corrData;
#endif
    // first we have to compute the offset correction data for dsp
    for (int i = 0; i < m_ChannelList.count(); i++)
    {
        rchn = m_ChannelList.at(i);
        dspchn = rchn->getDSPChannelNr();
        //double rej = rchn->getRejection();
        //double gaink = m_fGainCorr[dspchn];
        //double urv = rchn->getUrValue();
        double dc = m_ActualValues.at(i*2) / 2.0; // we only use the real part of dft but must devide by 2
        cval = -dc /* * rej / (gaink * urv)*/;
        m_fOffset2Corr[dspchn] = cval;
#ifdef DEBUG
        corrData.append(cval);
#endif
    }

#ifdef DEBUG
    QString s = "RefModule: ";
    for (int i = 0; i < m_ChannelList.count(); i++)
    {
        s = s + QString("K(%1)=%2;").arg(m_ChannelList.at(i)->getAlias()).arg(corrData.at(i));
    }
    qDebug() << s;
#endif

    if (m_bActive)
        m_MsgNrCmdList[m_pDSPInterFace->dspMemoryWrite(m_pOffset2CorrectionDSP)] = writeoffsetadjustment;
}


void cReferenceAdjustment::set10VRange()
{
    if (m_bActive)
    {
        m_nRangeSetPending = m_ChannelList.count();
        for (int i = 0; i <  m_nRangeSetPending; i++)
        {
            m_MsgNrCmdList[m_ChannelList.at(i)->setRange("R10V")] = setrange10V;
        }
    }
}


void cReferenceAdjustment::referenceAdjustDone()
{
    m_bAdjustmentDone = true; // our work is done and from here we ignore further actual values
    emit activated();
}


void cReferenceAdjustment::deactivationInit()
{
    m_bActive = false;
    m_pProxy->releaseConnection(m_pDspClient);
    m_pProxy->releaseConnection(m_pPCBClient);
    m_pDSPInterFace->deleteMemHandle(m_pGainCorrectionDSP);
    m_pDSPInterFace->deleteMemHandle(m_pOffset2CorrectionDSP);
    emit deactivationContinue();
}


void cReferenceAdjustment::deactivationDone()
{
    disconnect(m_pPCBInterface, 0, this, 0);
    disconnect(m_pDSPInterFace, 0, this, 0);

    emit deactivated();
}


void cReferenceAdjustment::catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant)
{
    if (msgnr == 0) // 0 was reserved for async. messages
    {
        // that we will ignore
    }
    else
    {
        if (m_MsgNrCmdList.contains(msgnr))
        {
            int cmd = m_MsgNrCmdList.take(msgnr);
            switch (cmd)
            {
            case setreferencemode:
                if (reply == ack)
                    emit activationContinue();
                else
                {
                    emit errMsg((tr(setMeasModeErrMsg)));
#ifdef DEBUG
                    qDebug() << setMeasModeErrMsg;
#endif
                    emit activationError();
                }
                break;
            case setacmode:
                if (reply == ack)
                    emit deactivationContinue();
                else
                {
                    emit errMsg((tr(setMeasModeErrMsg)));
#ifdef DEBUG
                    qDebug() << setMeasModeErrMsg;
#endif
                    emit deactivationError();
                }
                break;
            case readgaincorr:
                if (reply == ack)
                    emit adjustContinue();
                else
                {
                    emit errMsg((tr(readdspgaincorrErrMsg)));
#ifdef DEBUG
                    qDebug() << readdspgaincorrErrMsg;
#endif
                    emit executionError();
                }
                break;

            case readoffset2corr:
                if (reply == ack)
                    emit adjustContinue();
                else
                {
                    emit errMsg((tr(readdspoffsetcorrErrMsg)));
#ifdef DEBUG
                    qDebug() << readdspoffsetcorrErrMsg;
#endif
                    emit executionError();
                }
                break;

            case writeoffsetadjustment:
                if (reply == ack)
                    emit adjustContinue();
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

void cReferenceAdjustment::catchChannelReply(quint32 msgnr)
{
    int cmd = m_MsgNrCmdList.take(msgnr);
    switch (cmd)
    {
    case setrange0V:
        if (m_nRangeSetPending > 0)
        {
            m_nRangeSetPending--;
            if (m_nRangeSetPending == 0)
                emit activationContinue();
        }
        break;
    case setrange10V:
        if (m_nRangeSetPending > 0)
        {
            m_nRangeSetPending--;
            if (m_nRangeSetPending == 0)
                emit adjustContinue();
        }
        break;
    }
}

}





