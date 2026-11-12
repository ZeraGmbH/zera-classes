#include "rangechanneladjustment.h"
#include "boolvalidator.h"
#include "doublevalidator.h"
#include "rangemodule.h"
#include "rangemeaschannelstatichelperfunctions.h"
#include "rangemodulemeasprogram.h"
#include <errormessages.h>
#include <reply.h>
#include <proxy.h>
#include <scpi.h>
#include <timerfactoryqt.h>
#include <vfmodulecomponent.h>

namespace RANGEMODULE 
{

RangeChannelAdjustment::RangeChannelAdjustment(cRangeModule *module,
                                     QStringList chnlist,
                                     QStringList subdclist,
                                     RangeAdjustmentConfigParams *adjustmentConfig) :
    cModuleActivist(QString("%1 / AdjustManagement").arg(qPrintable(module->getVeinModuleName()))),
    m_pModule(module),
    m_channelNameList(chnlist),
    m_subdcChannelNameList(subdclist),
    m_adjustmentConfig(adjustmentConfig)
{
    m_dspInterface = m_pModule->getServiceInterfaceFactory()->createDspInterfaceRangeAdj(
        m_pModule->getEntityId(),
        m_channelNameList,
        false /* just for demo COM5003 ref-session - postpone better solution now */
        );

    for (int channelIdx = 0; channelIdx < m_channelNameList.count(); channelIdx++) {
        // we fetch all our real channels first
        m_ChannelList.append(m_pModule->getMeasChannel(m_channelNameList.at(channelIdx)));
        // we connect cmddone of our meas channels so we get informed if commands are finished
        connect(m_ChannelList.at(channelIdx), &cRangeMeasChannel::cmdDone, this, &RangeChannelAdjustment::catchChannelReply);
    }

    for (int i = 0; i < m_subdcChannelNameList.count(); i++) // we fetch all our real channels first
        m_subDCChannelList.append(m_pModule->getMeasChannel(m_subdcChannelNameList.at(i)));

    // and then set up our state machines
    m_readGainCorrState.addTransition(this, &RangeChannelAdjustment::activationContinue, &m_readPhaseCorrState);
    m_readPhaseCorrState.addTransition(this, &RangeChannelAdjustment::activationContinue, &m_readOffsetCorrState);
    m_readOffsetCorrState.addTransition(this, &RangeChannelAdjustment::activationContinue, &m_setSubDCState);
    m_setSubDCState.addTransition(this, &RangeChannelAdjustment::activationContinue, &m_activationDoneState);
    m_activationMachine.addState(&m_dspserverConnectState);
    m_activationMachine.addState(&m_readGainCorrState);
    m_activationMachine.addState(&m_readPhaseCorrState);
    m_activationMachine.addState(&m_readOffsetCorrState);
    m_activationMachine.addState(&m_setSubDCState);
    m_activationMachine.addState(&m_activationDoneState);
    m_activationMachine.setInitialState(&m_dspserverConnectState);
    connect(&m_dspserverConnectState, &QState::entered, this, &RangeChannelAdjustment::dspserverConnect);
    connect(&m_readGainCorrState, &QState::entered, this, &RangeChannelAdjustment::readGainCorr);
    connect(&m_readPhaseCorrState, &QState::entered, this, &RangeChannelAdjustment::readPhaseCorr);
    connect(&m_readOffsetCorrState, &QState::entered, this, &RangeChannelAdjustment::readOffsetCorr);
    connect(&m_setSubDCState, &QState::entered, this, &RangeChannelAdjustment::setSubDC);
    connect(&m_activationDoneState, &QState::entered, this, &RangeChannelAdjustment::activationDone);

    m_deactivationInitState.addTransition(this, &RangeChannelAdjustment::deactivationContinue, &m_deactivationDoneState);
    m_deactivationMachine.addState(&m_deactivationInitState);
    m_deactivationMachine.addState(&m_deactivationDoneState);
    m_deactivationMachine.setInitialState(&m_deactivationInitState);
    connect(&m_deactivationInitState, &QState::entered, this, &RangeChannelAdjustment::deactivationInit);
    connect(&m_deactivationDoneState, &QState::entered, this, &RangeChannelAdjustment::deactivationDone);

    m_writeGainCorrState.addTransition(this, &RangeChannelAdjustment::activationContinue, &m_writePhaseCorrState);
    m_writePhaseCorrState.addTransition(this, &RangeChannelAdjustment::activationContinue, &m_writeOffsetCorrState);
    m_writeOffsetCorrState.addTransition(this, &RangeChannelAdjustment::activationContinue, &m_writeCorrDoneState);
    m_writeCorrectionDSPMachine.addState(&m_writeGainCorrState);
    m_writeCorrectionDSPMachine.addState(&m_writePhaseCorrState);
    m_writeCorrectionDSPMachine.addState(&m_writeOffsetCorrState);
    m_writeCorrectionDSPMachine.addState(&m_writeCorrDoneState);
    m_writeCorrectionDSPMachine.setInitialState(&m_writeGainCorrState);
    connect(&m_writeGainCorrState, &QState::entered, this, &RangeChannelAdjustment::writeGainCorr);
    connect(&m_writePhaseCorrState, &QState::entered, this, &RangeChannelAdjustment::writePhaseCorr);
    connect(&m_writeOffsetCorrState, &QState::entered, this, &RangeChannelAdjustment::writeOffsetCorr);

    m_getGainCorrFromPcbServerState.addTransition(this, &RangeChannelAdjustment::activationContinue, &m_prepareGainCorrForDspServerState);
    m_prepareGainCorrForDspServerState.addTransition(this, &RangeChannelAdjustment::activationContinue, &m_getPhaseCorrFromPcbServerState);
    m_prepareGainCorrForDspServerState.addTransition(this, &RangeChannelAdjustment::repeatStateMachine, &m_getGainCorrFromPcbServerState);
    m_getPhaseCorrFromPcbServerState.addTransition(this, &RangeChannelAdjustment::activationContinue, &m_preparePhaseCorrForDspServerState);
    m_preparePhaseCorrForDspServerState.addTransition(this, &RangeChannelAdjustment::repeatStateMachine, &m_getPhaseCorrFromPcbServerState);
    m_preparePhaseCorrForDspServerState.addTransition(this, &RangeChannelAdjustment::activationContinue, &m_getOffsetCorrFromPcbServerState);
    m_getOffsetCorrFromPcbServerState.addTransition(this, &RangeChannelAdjustment::activationContinue, &m_prepareOffsetCorrForDspServerState);
    m_prepareOffsetCorrForDspServerState.addTransition(this, &RangeChannelAdjustment::repeatStateMachine, &m_getOffsetCorrFromPcbServerState);
    m_prepareOffsetCorrForDspServerState.addTransition(this, &RangeChannelAdjustment::activationContinue, &m_adjustDoneState);
    m_adjustMachine.addState(&m_getGainCorrFromPcbServerState);
    m_adjustMachine.addState(&m_prepareGainCorrForDspServerState);
    m_adjustMachine.addState(&m_getPhaseCorrFromPcbServerState);
    m_adjustMachine.addState(&m_preparePhaseCorrForDspServerState);
    m_adjustMachine.addState(&m_getOffsetCorrFromPcbServerState);
    m_adjustMachine.addState(&m_prepareOffsetCorrForDspServerState);
    m_adjustMachine.addState(&m_adjustDoneState);
    m_adjustMachine.setInitialState(&m_getGainCorrFromPcbServerState);
    connect(&m_getGainCorrFromPcbServerState, &QState::entered, this, &RangeChannelAdjustment::getGainCorrFromPcbServer);
    connect(&m_prepareGainCorrForDspServerState, &QState::entered, this, &RangeChannelAdjustment::prepareGainCorrForDspServer);
    connect(&m_getPhaseCorrFromPcbServerState, &QState::entered, this, &RangeChannelAdjustment::getPhaseCorrFromPcbServer);
    connect(&m_preparePhaseCorrForDspServerState, &QState::entered, this, &RangeChannelAdjustment::preparePhaseCorrForDspServer);
    connect(&m_getOffsetCorrFromPcbServerState, &QState::entered, this, &RangeChannelAdjustment::getOffsetCorrFromPcbServer);
    connect(&m_prepareOffsetCorrForDspServerState, &QState::entered, this, &RangeChannelAdjustment::prepareOffsetCorrForDspServer);
    connect(&m_adjustDoneState, &QState::entered, this, &RangeChannelAdjustment::getCorrDone);
}

void RangeChannelAdjustment::ActionHandler(QVector<float> *actualValues)
{
    m_nChannelIt = 0; // we start with first channel
    // in case measurement is faster than adjusting
    if (m_bActive && m_bAdjustTrigger && !m_adjustMachine.isRunning()) {
        m_bAdjustTrigger = false;
        m_adjustMachine.start();
    }
}

void RangeChannelAdjustment::generateVeinInterface()
{
    m_ParIgnoreRmsValuesOnOff = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                                      QString("PAR_IgnoreRmsValuesOnOff"),
                                                      QString("Enable or disable channel ignore limit option"),
                                                      QVariant(m_adjustmentConfig->m_ignoreRmsValuesEnable.m_nActive));

    m_ParIgnoreRmsValuesOnOff->setValidator(new cBoolValidator());
    m_pModule->m_veinModuleParameterMap["PAR_IgnoreRmsValuesOnOff"] = m_ParIgnoreRmsValuesOnOff;
    m_ParIgnoreRmsValuesOnOff->setScpiInfo("CONFIGURATION","ENABLEIGNORERMSVAL", SCPI::isQuery|SCPI::isCmdwP);
    connect(m_ParIgnoreRmsValuesOnOff, &VfModuleParameter::sigValueChanged, this, &RangeChannelAdjustment::parIgnoreRmsValuesOnOffChanged);

    m_ParIgnoreRmsValues = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                                 QString("PAR_IgnoreRmsValues"),
                                                 QString("Percentage of nominal range below which channel values are ignored"),
                                                 QVariant(m_adjustmentConfig->m_ignoreRmsValuesThreshold.m_fValue));

    m_ParIgnoreRmsValues->setValidator(new cDoubleValidator(0, 2, 1e-1));
    m_pModule->m_veinModuleParameterMap["PAR_IgnoreRmsValues"] = m_ParIgnoreRmsValues;
    m_ParIgnoreRmsValues->setUnit("%");
    m_ParIgnoreRmsValues->setScpiInfo("CONFIGURATION","IGNORERMSVAL", SCPI::isQuery|SCPI::isCmdwP);
    connect(m_ParIgnoreRmsValues, &VfModuleParameter::sigValueChanged, this, &RangeChannelAdjustment::parIgnoreRmsValuesChanged);

    VfModuleParameter *pParameter;
    for (int channelIdx = 0; channelIdx < m_channelNameList.count(); channelIdx++) {
        QString key = QString("PAR_InvertPhase%1").arg(channelIdx+1);
        pParameter = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                           key,
                                           QString("Inverted Phase"),
                                           QVariant(m_adjustmentConfig->m_senseChannelInvertParameter[channelIdx].m_nActive));
        pParameter->setValidator(new cBoolValidator());
        m_pModule->m_veinModuleParameterMap[key] = pParameter; // for modules use
        m_invertedPhasesParList.append(pParameter);
        connect(pParameter, &VfModuleParameter::sigValueChanged, this, &RangeChannelAdjustment::parInvertedPhaseStateChanged);
        ChannelCommonStorage *channelCommon = m_ChannelList[channelIdx]->getChannelObserver()->getModuleCommonStorage();
        channelCommon->setInvertedPhaseState(m_adjustmentConfig->m_senseChannelInvertParameter[channelIdx].m_nActive);
    }
}

void RangeChannelAdjustment::dspserverConnect()
{
    // we set up our dsp server connection
    m_dspInterface->setClientSuperSmart(m_pModule->getNetworkConfig()->m_dspServiceConnectionInfo, m_pModule->getNetworkConfig()->m_tcpNetworkFactory);
    m_dspserverConnectState.addTransition(m_dspInterface->getClientSmart().get(), &Zera::ProxyClient::connected, &m_readGainCorrState);
    connect(m_dspInterface.get(), &AbstractServerInterface::serverAnswer, this, &RangeChannelAdjustment::catchInterfaceAnswer);
    Zera::Proxy::getInstance()->startConnectionSmart(m_dspInterface->getClientSmart());
}

enum adjustmentCmds
{
    readgaincorr,
    readphasecorr,
    readoffsetcorr,

    writegaincorr,
    writephasecorr,
    writeoffsetcorr,

    getgaincorr,
    getphasecorr,
    getoffsetcore,

    subdcdsp,
};

void RangeChannelAdjustment::readGainCorr()
{
    // we fetch a handle for each gain-, phase, offset correction for
    // all possible channels because we do not know which channels become active

    m_pGainCorrectionDSP = m_dspInterface->createVariableGroup("GainCorrection");
    m_pGainCorrectionDSP->addDspVar("GAINCORRECTION", 32, dspDataTypeFloat, dspInternalSegment);
    m_dspGainCorrValues = m_pGainCorrectionDSP->data("GAINCORRECTION");

    m_pPhaseCorrectionDSP = m_dspInterface->createVariableGroup("PhaseCorrection");
    m_pPhaseCorrectionDSP->addDspVar("PHASECORRECTION", 32, dspDataTypeFloat, dspInternalSegment);
    m_dspPhaseCorrValues = m_pPhaseCorrectionDSP->data("PHASECORRECTION");

    m_pOffsetCorrectionDSP = m_dspInterface->createVariableGroup("OffsetCorrection");
    m_pOffsetCorrectionDSP->addDspVar("OFFSETCORRECTION", 32, dspDataTypeFloat, dspInternalSegment);
    m_dspOffsetCorrValues = m_pOffsetCorrectionDSP->data("OFFSETCORRECTION");

    m_MsgNrCmdList[m_dspInterface->dspMemoryRead(m_pGainCorrectionDSP)] = readgaincorr;
}

void RangeChannelAdjustment::readPhaseCorr()
{
    m_MsgNrCmdList[m_dspInterface->dspMemoryRead(m_pPhaseCorrectionDSP)] = readphasecorr;
}

void RangeChannelAdjustment::readOffsetCorr()
{
    m_MsgNrCmdList[m_dspInterface->dspMemoryRead(m_pOffsetCorrectionDSP)] = readoffsetcorr;
}

void RangeChannelAdjustment::setSubDC()
{
    quint32 subdc = 0;
    for (int i = 0; i < m_subDCChannelList.count(); i++)
        subdc |= (1 << m_subDCChannelList.at(i)->getDSPChannelNr());

    DspVarGroupClientInterface* pSubDCMaskDSP = m_dspInterface->createVariableGroup("SubDC"); // here we can set if sub dc or not
    pSubDCMaskDSP->addDspVar("SUBDC", 1, dspDataTypeInt, dspInternalSegment);
    pSubDCMaskDSP->setVarData(QString("SUBDC:%1;").arg(subdc));
    m_MsgNrCmdList[m_dspInterface->dspMemoryWrite(pSubDCMaskDSP)] = subdcdsp;
}

void RangeChannelAdjustment::activationDone()
{
    m_AdjustTimer = TimerFactoryQt::createPeriodic(m_adjustmentConfig->m_fAdjInterval*1000.0);
    m_AdjustTimer->start();
    connect(m_AdjustTimer.get(), &TimerTemplateQt::sigExpired, this, &RangeChannelAdjustment::adjustPrepare);
    m_fGainKeeperForFakingRmsValues.resize(32); //prepared for 32 dsp channels
    for(int i = 0; i < m_ChannelList.count(); i++) {
        m_fGainKeeperForFakingRmsValues[m_ChannelList.at(i)->getDSPChannelNr()] = 1;
        m_invertedPhasesParList.at(i)->setScpiInfo("CONFIGURATION",QString("%1:INVERTPHASE").arg(m_ChannelList.at(i)->getAlias()), SCPI::isQuery|SCPI::isCmdwP);
    }
    m_bActive = true;
    emit activated();
}

void RangeChannelAdjustment::deactivationInit()
{
    m_bActive = false;
    Zera::Proxy::getInstance()->releaseConnectionSmart(m_dspInterface->getClientSmart());
    disconnect(m_dspInterface.get(), 0, this, 0);

    for (int i = 0; i < m_ChannelList.count(); i++)
        disconnect(m_ChannelList.at(i), 0, this, 0);

    emit deactivationContinue();
}

void RangeChannelAdjustment::deactivationDone()
{
    emit deactivated();
}

void RangeChannelAdjustment::writeGainCorr()
{
    if (m_bActive)
        m_MsgNrCmdList[m_dspInterface->dspMemoryWrite(m_pGainCorrectionDSP)] = writegaincorr;
}

void RangeChannelAdjustment::writePhaseCorr()
{
    if (m_bActive)
        m_MsgNrCmdList[m_dspInterface->dspMemoryWrite(m_pPhaseCorrectionDSP)] = writephasecorr;
}

void RangeChannelAdjustment::writeOffsetCorr()
{
    if (m_bActive)
        m_MsgNrCmdList[m_dspInterface->dspMemoryWrite(m_pOffsetCorrectionDSP)] = writeoffsetcorr;
}

void RangeChannelAdjustment::getGainCorrFromPcbServer()
{
    if (m_bActive){
        const RangeChannelData *channelData = m_ChannelList.at(m_nChannelIt)->getSharedChannelData();
        double actualValue = channelData->getRmsValue();
        double preScalingFact = channelData->getPreScaling();
        cRangeMeasChannel *measChannel = m_ChannelList.at(m_nChannelIt);
        double unscaledActualValue = actualValue * preScalingFact / m_fGainKeeperForFakingRmsValues[measChannel->getDSPChannelNr()];
        m_MsgNrCmdList[m_ChannelList.at(m_nChannelIt)->readGainCorrection(unscaledActualValue)] = getgaincorr;
    }
}

void RangeChannelAdjustment::prepareGainCorrForDspServer()
{
    if (m_bActive) {
        cRangeMeasChannel *measChannel = m_ChannelList.at(m_nChannelIt);
        // TODO Phase inversion to GAINCORRECTION2
        float fCorr = measChannel->getGainCorrection();
        ChannelCommonStorage *channelCommon = measChannel->getChannelObserver()->getModuleCommonStorage();
        if (channelCommon->getInvertedPhaseState())
            fCorr = fCorr * -1;
        m_dspGainCorrValues[measChannel->getDSPChannelNr()] = fCorr*getIgnoreRmsCorrFactor();
        m_fGainKeeperForFakingRmsValues[measChannel->getDSPChannelNr()] = getIgnoreRmsCorrFactor();
        m_nChannelIt++;
        if (m_nChannelIt < m_channelNameList.count())
            emit repeatStateMachine();
        else {
            m_nChannelIt = 0;
            emit activationContinue();
        }
    }
}

double RangeChannelAdjustment::getIgnoreRmsCorrFactor()
{
    double ignoreRmsCorrFactor = 1.0;
    cRangeMeasChannel *measChannel = m_ChannelList.at(m_nChannelIt);
    if(m_adjustmentConfig->m_ignoreRmsValuesEnable.m_nActive) {
        double actUrValue = RangeMeasChannelStaticHelperFunctions::getUrValueRangeAct(measChannel);
        double threshold = m_adjustmentConfig->m_ignoreRmsValuesThreshold.m_fValue * actUrValue / 100;
        const RangeChannelData *channelData = measChannel->getSharedChannelData();
        double unscaledRmsValues = channelData->getRmsValue() * channelData->getPreScaling();
        double rmsValues = unscaledRmsValues / m_fGainKeeperForFakingRmsValues[measChannel->getDSPChannelNr()];
        if(rmsValues < threshold)
            ignoreRmsCorrFactor = 1e-10;
    }
    return ignoreRmsCorrFactor;
}

void RangeChannelAdjustment::getPhaseCorrFromPcbServer()
{
    if (m_bActive) {
        const RangeChannelData *channelData = m_ChannelList.at(m_nChannelIt)->getSharedChannelData();
        double frequency = channelData->getSignalFrequency();
        m_MsgNrCmdList[m_ChannelList.at(m_nChannelIt)->readPhaseCorrection(frequency)] = getphasecorr;
    }
}


void RangeChannelAdjustment::preparePhaseCorrForDspServer()
{
    if (m_bActive) {
        cRangeMeasChannel *measChannel = m_ChannelList.at(m_nChannelIt);
        float fCorr = measChannel->getPhaseCorrection();
        m_dspPhaseCorrValues[measChannel->getDSPChannelNr()] = fCorr;
        m_nChannelIt++;
        if (m_nChannelIt < m_channelNameList.count())
            emit repeatStateMachine();
        else {
            m_nChannelIt = 0;
            emit activationContinue();
        }
    }
}

void RangeChannelAdjustment::getOffsetCorrFromPcbServer()
{
    if (m_bActive) {
        cRangeMeasChannel* measChannel = m_ChannelList.at(m_nChannelIt);
        const RangeChannelData *channelData = measChannel->getSharedChannelData();
        double actualValue = channelData->getRmsValue();
        double preScalingFact = channelData->getPreScaling();
        double actualValueForOffset = actualValue * preScalingFact;
        m_MsgNrCmdList[m_ChannelList.at(m_nChannelIt)->readOffsetCorrection(actualValueForOffset)] = getoffsetcore;
    }
}

void RangeChannelAdjustment::prepareOffsetCorrForDspServer()
{
    if (m_bActive) {
        cRangeMeasChannel *measChannel = m_ChannelList.at(m_nChannelIt);
        float fCorr = measChannel->getOffsetCorrection();
        double preScale=measChannel->getSharedChannelData()->getPreScaling();
        // Offset is an summand. It must be multiplied with the
        // scale factor.
        m_dspOffsetCorrValues[measChannel->getDSPChannelNr()] = fCorr*preScale;

        m_nChannelIt++;
        if (m_nChannelIt < m_channelNameList.count())
            emit repeatStateMachine();
        else
            emit activationContinue();
    }
}

void RangeChannelAdjustment::getCorrDone()
{
    m_nChannelIt = 0;
    m_writeCorrectionDSPMachine.start(); // we have all correction data and write it to dsp
}

void RangeChannelAdjustment::catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer)
{
    Q_UNUSED(answer)
    if (msgnr == 0) { // 0 was reserved for async. messages
        // that we will ignore
    }
    else {
        // because rangemodulemeasprogram, adjustment and rangeobsermatic share the same dsp interface
        if (m_MsgNrCmdList.contains(msgnr)) {
            int cmd = m_MsgNrCmdList.take(msgnr);
            switch (cmd)
            {
            case readgaincorr:
                if (reply == ack)
                    emit activationContinue();
                else
                    notifyError(readdspgaincorrErrMsg);
                break;
            case readphasecorr:
                if (reply == ack)
                    emit activationContinue();
                else
                    notifyError(readdspphasecorrErrMsg);
                break;
            case readoffsetcorr:
                if (reply == ack)
                    emit activationContinue();
                else
                    notifyError(readdspoffsetcorrErrMsg);
                break;

            case subdcdsp:
                if (reply == ack)
                    emit activationContinue();
                else
                    notifyError(writesubdcErrMsg);
                break;

            case writegaincorr:
                if (reply == ack)
                    emit activationContinue();
                else
                    notifyError(writedspgaincorrErrMsg);
                break;
            case writephasecorr:
                if (reply == ack)
                    emit activationContinue();
                else
                    notifyError(writedspphasecorrErrMsg);
                break;
            case writeoffsetcorr:
                if (reply == ack)
                    emit activationContinue();
                else
                    notifyError(writedspoffsetcorrErrMsg);
                break;
            }
        }
    }
}

void RangeChannelAdjustment::catchChannelReply(quint32 msgnr)
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

void RangeChannelAdjustment::adjustPrepare()
{
    m_bAdjustTrigger = true;
}

void RangeChannelAdjustment::parIgnoreRmsValuesOnOffChanged(QVariant newValue)
{
    m_adjustmentConfig->m_ignoreRmsValuesEnable.m_nActive = newValue.toInt();
    emit m_pModule->parameterChanged();
}

void RangeChannelAdjustment::parIgnoreRmsValuesChanged(QVariant newValue)
{
    m_adjustmentConfig->m_ignoreRmsValuesThreshold.m_fValue = newValue.toDouble();
    emit m_pModule->parameterChanged();
}

void RangeChannelAdjustment::parInvertedPhaseStateChanged(QVariant newValue)
{
    VfModuleParameter *pParameter = qobject_cast<VfModuleParameter*>(sender()); // get sender of updated signal
    int index = m_invertedPhasesParList.indexOf(pParameter); // which channel is it
    ChannelCommonStorage *channelCommon = m_ChannelList[index]->getChannelObserver()->getModuleCommonStorage();
    channelCommon->setInvertedPhaseState(newValue.toBool());
    m_adjustmentConfig->m_senseChannelInvertParameter[index].m_nActive = newValue.toUInt();
    emit m_pModule->parameterChanged();
}

}
