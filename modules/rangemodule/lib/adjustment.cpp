#include "adjustment.h"
#include "boolvalidator.h"
#include "doublevalidator.h"
#include "rangemodule.h"
#include "rangemeaschannel.h"
#include "rangemodulemeasprogram.h"
#include <errormessages.h>
#include <reply.h>
#include <proxy.h>
#include <timerfactoryqt.h>
#include <vfmodulecomponent.h>
#include <rminterface.h>
#include <dspinterface.h>
#include <dspinterfacecmddecoder.h>
#include <QSignalTransition>

namespace RANGEMODULE 
{

cAdjustManagement::cAdjustManagement(cRangeModule *module,
                                     QStringList chnlist,
                                     QStringList subdclist,
                                     adjustConfPar *adjustmentConfig) :
    m_pModule(module),
    m_ChannelNameList(chnlist),
    m_subdcChannelNameList(subdclist),
    m_adjustmentConfig(adjustmentConfig)
{
    m_dspInterface = m_pModule->getServiceInterfaceFactory()->createDspInterfaceRange(
        m_ChannelNameList,
        false /* just for demo COM5003 ref-session - postpone better solution now */);
    m_pcbInterface = std::make_shared<Zera::cPCBInterface>();

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

    m_getGainCorrFromPcbServerState.addTransition(this, &cAdjustManagement::activationContinue, &m_prepareGainCorrForDspServerState);
    m_prepareGainCorrForDspServerState.addTransition(this, &cAdjustManagement::activationContinue, &m_getPhaseCorrFromPcbServerState);
    m_prepareGainCorrForDspServerState.addTransition(this, &cAdjustManagement::repeatStateMachine, &m_getGainCorrFromPcbServerState);
    m_getPhaseCorrFromPcbServerState.addTransition(this, &cAdjustManagement::activationContinue, &m_preparePhaseCorrForDspServerState);
    m_preparePhaseCorrForDspServerState.addTransition(this, &cAdjustManagement::repeatStateMachine, &m_getPhaseCorrFromPcbServerState);
    m_preparePhaseCorrForDspServerState.addTransition(this, &cAdjustManagement::activationContinue, &m_getOffsetCorrFromPcbServerState);
    m_getOffsetCorrFromPcbServerState.addTransition(this, &cAdjustManagement::activationContinue, &m_prepareOffsetCorrForDspServerState);
    m_prepareOffsetCorrForDspServerState.addTransition(this, &cAdjustManagement::repeatStateMachine, &m_getOffsetCorrFromPcbServerState);
    m_prepareOffsetCorrForDspServerState.addTransition(this, &cAdjustManagement::activationContinue, &m_adjustDoneState);
    m_adjustMachine.addState(&m_getGainCorrFromPcbServerState);
    m_adjustMachine.addState(&m_prepareGainCorrForDspServerState);
    m_adjustMachine.addState(&m_getPhaseCorrFromPcbServerState);
    m_adjustMachine.addState(&m_preparePhaseCorrForDspServerState);
    m_adjustMachine.addState(&m_getOffsetCorrFromPcbServerState);
    m_adjustMachine.addState(&m_prepareOffsetCorrForDspServerState);
    m_adjustMachine.addState(&m_adjustDoneState);
    m_adjustMachine.setInitialState(&m_getGainCorrFromPcbServerState);
    connect(&m_getGainCorrFromPcbServerState, &QState::entered, this, &cAdjustManagement::getGainCorrFromPcbServer);
    connect(&m_prepareGainCorrForDspServerState, &QState::entered, this, &cAdjustManagement::prepareGainCorrForDspServer);
    connect(&m_getPhaseCorrFromPcbServerState, &QState::entered, this, &cAdjustManagement::getPhaseCorrFromPcbServer);
    connect(&m_preparePhaseCorrForDspServerState, &QState::entered, this, &cAdjustManagement::preparePhaseCorrForDspServer);
    connect(&m_getOffsetCorrFromPcbServerState, &QState::entered, this, &cAdjustManagement::getOffsetCorrFromPcbServer);
    connect(&m_prepareOffsetCorrForDspServerState, &QState::entered, this, &cAdjustManagement::prepareOffsetCorrForDspServer);
    connect(&m_adjustDoneState, &QState::entered, this, &cAdjustManagement::getCorrDone);
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

    m_ParIgnoreRmsValuesOnOff = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->m_pModuleValidator,
                                                      QString("PAR_IgnoreRmsValuesOnOff"),
                                                      QString("Enable or disable channel ignore limit option"),
                                                      QVariant(m_adjustmentConfig->m_ignoreRmsValuesEnable.m_nActive));

    m_ParIgnoreRmsValuesOnOff->setValidator(new cBoolValidator());
    m_pModule->m_veinModuleParameterMap["PAR_IgnoreRmsValuesOnOff"] = m_ParIgnoreRmsValuesOnOff;
    m_ParIgnoreRmsValuesOnOff->setSCPIInfo(new cSCPIInfo("CONFIGURATION","ENABLEIGNORERMSVAL", "10", m_ParIgnoreRmsValuesOnOff->getName(), "0", ""));
    connect(m_ParIgnoreRmsValuesOnOff, &VfModuleParameter::sigValueChanged, this, &cAdjustManagement::parIgnoreRmsValuesOnOffChanged);

    m_ParIgnoreRmsValues = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->m_pModuleValidator,
                                                 QString("PAR_IgnoreRmsValues"),
                                                 QString("Percentage of nominal range below which channel values are ignored"),
                                                 QVariant(m_adjustmentConfig->m_ignoreRmsValuesThreshold.m_fValue));

    m_ParIgnoreRmsValues->setValidator(new cDoubleValidator(0, 2, 1e-1));
    m_pModule->m_veinModuleParameterMap["PAR_IgnoreRmsValues"] = m_ParIgnoreRmsValues;
    m_ParIgnoreRmsValues->setUnit("%");
    m_ParIgnoreRmsValues->setSCPIInfo(new cSCPIInfo("CONFIGURATION","IGNORERMSVAL", "10", m_ParIgnoreRmsValues->getName(), "0", m_ParIgnoreRmsValues->getUnit()));
    connect(m_ParIgnoreRmsValues, &VfModuleParameter::sigValueChanged, this, &cAdjustManagement::parIgnoreRmsValuesChanged);

    VfModuleParameter *pParameter;
    QString key;
    for (int i = 0; i < m_ChannelNameList.count(); i++) {
        pParameter = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->m_pModuleValidator,
                                           key = QString("PAR_InvertPhase%1").arg(i+1),
                                           QString("Inverted Phase"),
                                           QVariant(m_adjustmentConfig->m_senseChannelInvertParameter[i].m_nActive));
        pParameter->setValidator(new cBoolValidator());
        m_pModule->m_veinModuleParameterMap[key] = pParameter; // for modules use
        m_invertedPhasesParList.append(pParameter);
        connect(pParameter, &VfModuleParameter::sigValueChanged, this, &cAdjustManagement::parInvertedPhaseStateChanged);
        m_ChannelList[i]->setInvertedPhaseState(m_adjustmentConfig->m_senseChannelInvertParameter[i].m_nActive);
    }
}


void cAdjustManagement::pcbserverConnect()
{
    // we connect cmddone of our meas channels so we get informed if commands are finished
    for (int i = 0; i < m_ChannelList.count(); i++)
        connect(m_ChannelList.at(i), &cRangeMeasChannel::cmdDone, this, &cAdjustManagement::catchChannelReply);

    // we set up our pcb server connection
    m_pcbClient = Zera::Proxy::getInstance()->getConnectionSmart(m_pModule->getNetworkConfig()->m_pcbServiceConnectionInfo.m_sIP,
                                                                 m_pModule->getNetworkConfig()->m_pcbServiceConnectionInfo.m_nPort,
                                                                 m_pModule->getNetworkConfig()->m_tcpNetworkFactory);
    m_pcbInterface->setClientSmart(m_pcbClient);
    m_pcbserverConnectState.addTransition(m_pcbClient.get(), &Zera::ProxyClient::connected, &m_dspserverConnectState);
    connect(m_pcbInterface.get(), &Zera::cPCBInterface::serverAnswer, this, &cAdjustManagement::catchInterfaceAnswer);
    Zera::Proxy::getInstance()->startConnectionSmart(m_pcbClient);
}


void cAdjustManagement::dspserverConnect()
{
    // we set up our dsp server connection
    m_dspClient = Zera::Proxy::getInstance()->getConnectionSmart(m_pModule->getNetworkConfig()->m_dspServiceConnectionInfo.m_sIP,
                                                                 m_pModule->getNetworkConfig()->m_dspServiceConnectionInfo.m_nPort,
                                                                 m_pModule->getNetworkConfig()->m_tcpNetworkFactory);
    m_dspInterface->setClientSmart(m_dspClient);
    m_dspserverConnectState.addTransition(m_dspClient.get(), &Zera::ProxyClient::connected, &m_readGainCorrState);
    connect(m_dspInterface.get(), &Zera::cDSPInterface::serverAnswer, this, &cAdjustManagement::catchInterfaceAnswer);
    Zera::Proxy::getInstance()->startConnectionSmart(m_dspClient);
}


void cAdjustManagement::readGainCorr()
{
    // we fetch a handle for each gain-, phase, offset correction for
    // all possible channels because we do not know which channels become active

    m_pGainCorrectionDSP = m_dspInterface->getMemHandle("GainCorrection");
    m_pGainCorrectionDSP->addVarItem( new cDspVar("GAINCORRECTION",32, DSPDATA::vDspIntVar));
    m_fGainCorr = m_pGainCorrectionDSP->data("GAINCORRECTION");

    m_pPhaseCorrectionDSP = m_dspInterface->getMemHandle("PhaseCorrection");
    m_pPhaseCorrectionDSP->addVarItem( new cDspVar("PHASECORRECTION",32, DSPDATA::vDspIntVar));
    m_fPhaseCorr = m_pPhaseCorrectionDSP->data("PHASECORRECTION");

    m_pOffsetCorrectionDSP = m_dspInterface->getMemHandle("OffsetCorrection");
    m_pOffsetCorrectionDSP->addVarItem( new cDspVar("OFFSETCORRECTION",32, DSPDATA::vDspIntVar));
    m_fOffsetCorr = m_pOffsetCorrectionDSP->data("OFFSETCORRECTION");

    m_MsgNrCmdList[m_dspInterface->dspMemoryRead(m_pGainCorrectionDSP)] = readgaincorr;
}


void cAdjustManagement::readPhaseCorr()
{
    m_MsgNrCmdList[m_dspInterface->dspMemoryRead(m_pPhaseCorrectionDSP)] = readphasecorr;
}


void cAdjustManagement::readOffsetCorr()
{
    m_MsgNrCmdList[m_dspInterface->dspMemoryRead(m_pOffsetCorrectionDSP)] = readoffsetcorr;
}


void cAdjustManagement::setSubDC()
{
    quint32 subdc = 0;
    for (int i = 0; i < m_subDCChannelList.count(); i++) {
        subdc |= (1 << m_subDCChannelList.at(i)->getDSPChannelNr());
    }
    cDspMeasData* pSubDCMaskDSP = m_dspInterface->getMemHandle("SubDC"); // here we can set if sub dc or not
    pSubDCMaskDSP->addVarItem( new cDspVar("SUBDC",1, DSPDATA::vDspIntVar, DSPDATA::dInt));
    DspInterfaceCmdDecoder::setVarData(pSubDCMaskDSP, QString("SUBDC:%1;").arg(subdc), DSPDATA::dInt);
    m_MsgNrCmdList[m_dspInterface->dspMemoryWrite(pSubDCMaskDSP)] = subdcdsp;
}


void cAdjustManagement::activationDone()
{
    m_AdjustTimer = TimerFactoryQt::createPeriodic(m_adjustmentConfig->m_fAdjInterval*1000.0);
    m_AdjustTimer->start();
    connect(m_AdjustTimer.get(), &TimerTemplateQt::sigExpired, this, &cAdjustManagement::adjustPrepare);
    m_fGainKeeperForFakingRmsValues.resize(32); //prepared for 32 dsp channels
    for(int i = 0; i < m_ChannelList.count(); i++) {
        m_fGainKeeperForFakingRmsValues[m_ChannelList.at(i)->getDSPChannelNr()] = 1;
        m_invertedPhasesParList.at(i)->setSCPIInfo(new cSCPIInfo("CONFIGURATION",QString("%1:INVERTPHASE").arg(m_ChannelList.at(i)->getAlias()), "10", m_invertedPhasesParList.at(i)->getName(), "0", ""));
    }
    m_bActive = true;
    emit activated();
}


void cAdjustManagement::deactivationInit()
{
    m_bActive = false;
    Zera::Proxy::getInstance()->releaseConnection(m_dspClient.get());
    disconnect(m_dspInterface.get(), 0, this, 0);

    for (int i = 0; i < m_ChannelList.count(); i++)
        disconnect(m_ChannelList.at(i), 0, this, 0);

    m_dspInterface->deleteMemHandle(m_pGainCorrectionDSP);
    m_dspInterface->deleteMemHandle(m_pPhaseCorrectionDSP);
    m_dspInterface->deleteMemHandle(m_pOffsetCorrectionDSP);

    emit deactivationContinue();
}


void cAdjustManagement::deactivationDone()
{
    emit deactivated();
}


void cAdjustManagement::writeGainCorr()
{
    if (m_bActive)
        m_MsgNrCmdList[m_dspInterface->dspMemoryWrite(m_pGainCorrectionDSP)] = writegaincorr;
}


void cAdjustManagement::writePhaseCorr()
{
    if (m_bActive)
        m_MsgNrCmdList[m_dspInterface->dspMemoryWrite(m_pPhaseCorrectionDSP)] = writephasecorr;
}


void cAdjustManagement::writeOffsetCorr()
{
    if (m_bActive)
        m_MsgNrCmdList[m_dspInterface->dspMemoryWrite(m_pOffsetCorrectionDSP)] = writeoffsetcorr;
}


void cAdjustManagement::getGainCorrFromPcbServer()
{
    if (m_bActive){
        double actualValue=m_ChannelList.at(m_nChannelIt)->getRmsValue();
        double preScalingFact=m_ChannelList[m_nChannelIt]->getPreScaling();
        cRangeMeasChannel *measChannel = m_ChannelList.at(m_nChannelIt);
        double unscaledActualValue = actualValue * preScalingFact / m_fGainKeeperForFakingRmsValues[measChannel->getDSPChannelNr()];
        m_MsgNrCmdList[m_ChannelList.at(m_nChannelIt)->readGainCorrection(unscaledActualValue)] = getgaincorr;
    }
}


void cAdjustManagement::prepareGainCorrForDspServer()
{
    cRangeMeasChannel *measChannel;
    float fCorr;

    if (m_bActive)
    {
        measChannel = m_ChannelList.at(m_nChannelIt);
        fCorr = measChannel->getGainCorrection();
        if (measChannel->getInvertedPhaseState())
            fCorr = fCorr * -1;
        m_fGainCorr[measChannel->getDSPChannelNr()] = fCorr*getIgnoreRmsCorrFactor();
        m_fGainKeeperForFakingRmsValues[measChannel->getDSPChannelNr()] = getIgnoreRmsCorrFactor();
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

double cAdjustManagement::getIgnoreRmsCorrFactor()
{
    double ignoreRmsCorrFactor = 1.0;
    cRangeMeasChannel *measChannel = m_ChannelList.at(m_nChannelIt);
    if(m_adjustmentConfig->m_ignoreRmsValuesEnable.m_nActive) {
        double threshold = m_adjustmentConfig->m_ignoreRmsValuesThreshold.m_fValue * measChannel->getUrValue() / 100;
        double unscaledRmsValues = measChannel->getRmsValue() * measChannel->getPreScaling();
        double rmsValues = unscaledRmsValues / m_fGainKeeperForFakingRmsValues[measChannel->getDSPChannelNr()];
        if(rmsValues < threshold)
            ignoreRmsCorrFactor = 1e-10;
    }
    return ignoreRmsCorrFactor;
}

void cAdjustManagement::getPhaseCorrFromPcbServer()
{
    if (m_bActive){
        double frequency=m_ChannelList.at(m_nChannelIt)->getSignalFrequency();
        m_MsgNrCmdList[m_ChannelList.at(m_nChannelIt)->readPhaseCorrection(frequency)] = getphasecorr;
    }
}


void cAdjustManagement::preparePhaseCorrForDspServer()
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


void cAdjustManagement::getOffsetCorrFromPcbServer()
{
    if (m_bActive){
        double actualValue=m_ChannelList.at(m_nChannelIt)->getRmsValue();
        double preScalingFact=m_ChannelList[m_nChannelIt]->getPreScaling();
        m_MsgNrCmdList[m_ChannelList.at(m_nChannelIt)->readOffsetCorrection(actualValue*preScalingFact)] = getoffsetcore;
    }
}


void cAdjustManagement::prepareOffsetCorrForDspServer()
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

void cAdjustManagement::parIgnoreRmsValuesOnOffChanged(QVariant newValue)
{
    m_adjustmentConfig->m_ignoreRmsValuesEnable.m_nActive = newValue.toInt();
    emit m_pModule->parameterChanged();
}

void cAdjustManagement::parIgnoreRmsValuesChanged(QVariant newValue)
{
    m_adjustmentConfig->m_ignoreRmsValuesThreshold.m_fValue = newValue.toDouble();
    emit m_pModule->parameterChanged();
}

void cAdjustManagement::parInvertedPhaseStateChanged(QVariant newValue)
{
    VfModuleParameter *pParameter = qobject_cast<VfModuleParameter*>(sender()); // get sender of updated signal
    int index = m_invertedPhasesParList.indexOf(pParameter); // which channel is it
    m_ChannelList[index]->setInvertedPhaseState(newValue.toBool());
    m_adjustmentConfig->m_senseChannelInvertParameter[index].m_nActive = newValue.toUInt();
    emit m_pModule->parameterChanged();
}

}








