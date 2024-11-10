#include "referenceadjustment.h"
#include "referencemoduleconfigdata.h"
#include "referencemodule.h"
#include "referencemeaschannel.h"
#include <errormessages.h>
#include <reply.h>
#include <proxy.h>
#include <math.h>
#include <QSignalTransition>

namespace REFERENCEMODULE
{

cReferenceAdjustment::cReferenceAdjustment(cReferenceModule* module, cReferenceModuleConfigData* confData) :
    cModuleActivist(module->getVeinModuleName()),
    m_pModule(module),
    m_pConfigData(confData)
{
    m_dspInterface = m_pModule->getServiceInterfaceFactory()->createDspInterfaceOther();
    m_pPCBInterface = std::make_shared<Zera::cPCBInterface>();

    for (int i = 0; i < m_pConfigData->m_referenceChannelList.count(); i++) // we fetch all our real channels first
        m_ChannelList.append(m_pModule->getMeasChannel(m_pConfigData->m_referenceChannelList.at(i)));

    // and then set up our state machines
    m_set0VRangeState.addTransition(this, &cReferenceAdjustment::activationContinue, &m_dspserverConnectState);
    m_activationMachine.addState(&m_pcbserverConnectState);
    m_activationMachine.addState(&m_set0VRangeState);
    m_activationMachine.addState(&m_dspserverConnectState);
    m_activationMachine.addState(&m_activationDoneState);

    if(m_pModule->getDemo())
        m_activationMachine.setInitialState(&m_activationDoneState);
    else
        m_activationMachine.setInitialState(&m_pcbserverConnectState);

    connect(&m_pcbserverConnectState, &QState::entered, this, &cReferenceAdjustment::pcbserverConnect);
    connect(&m_set0VRangeState, &QState::entered, this, &cReferenceAdjustment::set0VRange);
    connect(&m_dspserverConnectState, &QState::entered, this, &cReferenceAdjustment::dspserverConnect);
    connect(&m_activationDoneState, &QState::entered, this, &cReferenceAdjustment::activationDone);

    m_deactivationInitState.addTransition(this, &cReferenceAdjustment::deactivationContinue, &m_deactivationDoneState);
    m_deactivationMachine.addState(&m_deactivationInitState);
    m_deactivationMachine.addState(&m_deactivationDoneState);

    if(m_pModule->getDemo())
        m_deactivationMachine.setInitialState(&m_deactivationDoneState);
    else
        m_deactivationMachine.setInitialState(&m_deactivationInitState);

    connect(&m_deactivationInitState, &QState::entered, this, &cReferenceAdjustment::deactivationInit);
    connect(&m_deactivationDoneState, &QState::entered, this, &cReferenceAdjustment::deactivationDone);

    m_readGainCorrState.addTransition(this, &cReferenceAdjustment::adjustContinue, &m_readOffset2CorrState);
    m_readOffset2CorrState.addTransition(this, &cReferenceAdjustment::adjustContinue, &m_writeOffsetAdjustmentState);
    m_writeOffsetAdjustmentState.addTransition(this, &cReferenceAdjustment::adjustContinue, &m_set10VRangeState);
    m_set10VRangeState.addTransition(this, &cReferenceAdjustment::adjustContinue, &m_referenceAdjustDoneState);
    m_referenceAdjustMachine.addState(&m_readGainCorrState);
    m_referenceAdjustMachine.addState(&m_readOffset2CorrState);
    m_referenceAdjustMachine.addState(&m_writeOffsetAdjustmentState);
    m_referenceAdjustMachine.addState(&m_set10VRangeState);
    m_referenceAdjustMachine.addState(&m_referenceAdjustDoneState);

    if(m_pModule->getDemo())
        m_referenceAdjustMachine.setInitialState(&m_referenceAdjustDoneState);
    else
        m_referenceAdjustMachine.setInitialState(&m_readGainCorrState);

    connect(&m_readGainCorrState, &QState::entered, this, &cReferenceAdjustment::readGainCorr);
    connect(&m_readOffset2CorrState, &QState::entered, this, &cReferenceAdjustment::readOffset2Corr);
    connect(&m_writeOffsetAdjustmentState, &QState::entered, this, &cReferenceAdjustment::writeOffsetAdjustment);
    connect(&m_set10VRangeState, &QState::entered, this, &cReferenceAdjustment::set10VRange);
    connect(&m_referenceAdjustDoneState, &QState::entered, this, &cReferenceAdjustment::referenceAdjustDone);
}

void cReferenceAdjustment::ActionHandler(QVector<float> *actualValues)
{
    m_ActualValues = *actualValues;

    if (m_nIgnoreCount > 0)
    {
        m_nIgnoreCount--;
    }
    else
        // m_bactive in case measurement is faster than adjusting
        // adjustMachine.isRunning in caae actual values come faster than adjustment process duration
        if (m_bActive && !m_bAdjustmentDone && !m_referenceAdjustMachine.isRunning())
            if(!m_pModule->getDemo())
                m_referenceAdjustMachine.start();
}


void cReferenceAdjustment::generateInterface()
{
    // at the moment no interface
}


void cReferenceAdjustment::pcbserverConnect()
{
    m_pPCBClient = Zera::Proxy::getInstance()->getConnectionSmart(m_pModule->getNetworkConfig()->m_pcbServiceConnectionInfo,
                                                                  m_pModule->getNetworkConfig()->m_tcpNetworkFactory);
    m_pcbserverConnectState.addTransition(m_pPCBClient.get(), &Zera::ProxyClient::connected, &m_set0VRangeState);

    m_pPCBInterface->setClientSmart(m_pPCBClient);
    connect(m_pPCBInterface.get(), &Zera::cPCBInterface::serverAnswer, this, &cReferenceAdjustment::catchInterfaceAnswer);
    Zera::Proxy::getInstance()->startConnectionSmart(m_pPCBClient);
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
    m_dspClient = Zera::Proxy::getInstance()->getConnectionSmart(m_pModule->getNetworkConfig()->m_dspServiceConnectionInfo,
                                                                 m_pModule->getNetworkConfig()->m_tcpNetworkFactory);
    m_dspInterface->setClientSmart(m_dspClient);
    m_dspserverConnectState.addTransition(m_dspClient.get(), &Zera::ProxyClient::connected, &m_activationDoneState);
    connect(m_dspInterface.get(), &Zera::cDSPInterface::serverAnswer, this, &cReferenceAdjustment::catchInterfaceAnswer);
    Zera::Proxy::getInstance()->startConnectionSmart(m_dspClient);
}


void cReferenceAdjustment::activationDone()
{
    // we fetch a handle for gain correction and offset2 correction for
    // all possible channels because we do not know which channels become active
    if(!m_pModule->getDemo()) {
        m_pGainCorrectionDSP = m_dspInterface->getMemHandle("GainCorrection");
        m_pGainCorrectionDSP->addVarItem( new cDspVar("GAINCORRECTION",32, DSPDATA::vDspIntVar));
        m_fGainCorr = m_pGainCorrectionDSP->data("GAINCORRECTION");

        m_pOffset2CorrectionDSP = m_dspInterface->getMemHandle("OffsetCorrection");
        m_pOffset2CorrectionDSP->addVarItem( new cDspVar("OFFSETCORRECTION2",32, DSPDATA::vDspIntVar));
        m_fOffset2Corr = m_pOffset2CorrectionDSP->data("OFFSETCORRECTION2");
    }

    m_nIgnoreCount = m_pConfigData->m_nIgnore;
    m_bAdjustmentDone = false;

    m_bActive = true;
    if(m_pModule->getDemo())
        emit activated();
    // we will emit activated after reference measurement adjustment is done
}


void cReferenceAdjustment::readGainCorr()
{
    if (m_bActive)
        m_MsgNrCmdList[m_dspInterface->dspMemoryRead(m_pGainCorrectionDSP)] = readgaincorr;
}


void cReferenceAdjustment::readOffset2Corr()
{
    if (m_bActive)
        m_MsgNrCmdList[m_dspInterface->dspMemoryRead(m_pOffset2CorrectionDSP)] = readoffset2corr;
}


void cReferenceAdjustment::writeOffsetAdjustment()
{
    cReferenceMeasChannel* rchn;
    quint8 dspchn;
    float cval;

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
    }

    if (m_bActive)
        m_MsgNrCmdList[m_dspInterface->dspMemoryWrite(m_pOffset2CorrectionDSP)] = writeoffsetadjustment;
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
    Zera::Proxy::getInstance()->releaseConnection(m_dspClient.get());
    Zera::Proxy::getInstance()->releaseConnection(m_pPCBClient.get());
    m_dspInterface->deleteMemHandle(m_pGainCorrectionDSP);
    m_dspInterface->deleteMemHandle(m_pOffset2CorrectionDSP);
    emit deactivationContinue();
}


void cReferenceAdjustment::deactivationDone()
{
    disconnect(m_pPCBInterface.get(), 0, this, 0);
    disconnect(m_dspInterface.get(), 0, this, 0);

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
                    notifyError(setMeasModeErrMsg);
                break;
            case setacmode:
                if (reply == ack)
                    emit deactivationContinue();
                else
                    notifyError(setMeasModeErrMsg);
                break;
            case readgaincorr:
                if (reply == ack)
                    emit adjustContinue();
                else
                    notifyError(readdspgaincorrErrMsg);
                break;

            case readoffset2corr:
                if (reply == ack)
                    emit adjustContinue();
                else
                    notifyError(readdspoffsetcorrErrMsg);
                break;

            case writeoffsetadjustment:
                if (reply == ack)
                    emit adjustContinue();
                else
                    notifyError(writedspoffsetcorrErrMsg);
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





