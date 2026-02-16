#include "rangemodulemeasprogram.h"
#include "rangemodule.h"
#include "rangemeaschannel.h"
#include "rangemoduleconfiguration.h"
#include "rangemoduleconfigdata.h"
#include <errormessages.h>
#include <reply.h>
#include <proxy.h>
#include <scpi.h>
#include <timerfactoryqt.h>
#include <math.h>

namespace RANGEMODULE
{

cRangeModuleMeasProgram::cRangeModuleMeasProgram(cRangeModule* module, std::shared_ptr<BaseModuleConfiguration> pConfiguration) :
    cBaseDspMeasProgram(pConfiguration, module->getVeinModuleName()),
    m_pModule(module),
    m_dspWatchdogTimer(TimerFactoryQt::createSingleShot(3000)),
    m_frequencyLogStatistics(10000)
{
    const QStringList channelMNames = m_pModule->getSharedChannelRangeObserver()->getChannelMNames();
    m_dspInterface = m_pModule->getServiceInterfaceFactory()->createDspInterfaceRangeProg(
        m_pModule->getEntityId(),
        channelMNames,
        getConfData()->m_session.m_sPar == "ref");
    m_bRanging = false;
    m_bIgnore = false;

    m_var2DSPState.addTransition(this, &cRangeModuleMeasProgram::activationContinue, &m_cmd2DSPState);
    m_cmd2DSPState.addTransition(this, &cRangeModuleMeasProgram::activationContinue, &m_activateDSPState);
    m_activateDSPState.addTransition(this, &cRangeModuleMeasProgram::activationContinue, &m_loadDSPDoneState);

    m_activationMachine.addState(&m_dspserverConnectState);
    m_activationMachine.addState(&m_var2DSPState);
    m_activationMachine.addState(&m_cmd2DSPState);
    m_activationMachine.addState(&m_activateDSPState);
    m_activationMachine.addState(&m_loadDSPDoneState);

    m_activationMachine.setInitialState(&m_dspserverConnectState);

    connect(&m_dspserverConnectState, &QState::entered, this, &cRangeModuleMeasProgram::dspserverConnect);
    connect(&m_var2DSPState, &QState::entered, this, &cRangeModuleMeasProgram::varList2DSP);
    connect(&m_cmd2DSPState, &QState::entered, this, &cRangeModuleMeasProgram::cmdList2DSP);
    connect(&m_activateDSPState, &QState::entered, this, &cRangeModuleMeasProgram::activateDSP);
    connect(&m_loadDSPDoneState, &QState::entered, this, &cRangeModuleMeasProgram::activateDSPdone);

    m_deactivationMachine.addState(&m_unloadStart);
    m_deactivationMachine.addState(&m_unloadDSPDoneState);

    connect(&m_unloadStart, &QState::entered, this, &cRangeModuleMeasProgram::deactivateDSPStart);
    connect(&m_unloadDSPDoneState, &QState::entered, this, &cModuleActivist::deactivated);

    m_unloadStart.addTransition(this, &cRangeModuleMeasProgram::deactivationContinue, &m_unloadDSPDoneState);
    m_deactivationMachine.setInitialState(&m_unloadStart);

    // setting up statemachine for data acquisition
    m_dataAcquisitionState.addTransition(this, &cRangeModuleMeasProgram::dataAquisitionContinue, &m_dataAcquisitionDoneState);
    m_dataAcquisitionMachine.addState(&m_dataAcquisitionState);
    m_dataAcquisitionMachine.addState(&m_dataAcquisitionDoneState);
    m_dataAcquisitionMachine.setInitialState(&m_dataAcquisitionState);
    connect(&m_dataAcquisitionState, &QState::entered, this, &cRangeModuleMeasProgram::dataAcquisitionDSP);
    connect(&m_dataAcquisitionDoneState, &QState::entered, this, &cRangeModuleMeasProgram::dataReadDSP);

    connect(this, &cRangeModuleMeasProgram::actualValues, this, &cRangeModuleMeasProgram::setInterfaceActualValues);
    connect(m_dspWatchdogTimer.get(), &TimerTemplateQt::sigExpired, this, &cRangeModuleMeasProgram::onDspWatchdogTimeout);

    connect(&m_frequencyLogStatistics, &LogStatisticsAsyncFloat::sigNewStatistics, [](float min, float max, float avg, int msgCnt) {
        Q_UNUSED(msgCnt)
        qInfo("Measured frequency (Hz) min: %.3f, max: %.3f, mean: %.3f",
              min, max, avg);
    });
}

void cRangeModuleMeasProgram::syncRanging(QVariant sync)
{
    m_bRanging = (sync != 0); // we are ranging from the moment sync becomes 1
    m_bIgnore = (sync == 0); // when ranging has finished we ignore 1 more actual values
}

void cRangeModuleMeasProgram::generateVeinInterface()
{
    VfModuleComponent *pActvalue;
    const QStringList channelMNames = m_pModule->getSharedChannelRangeObserver()->getChannelMNames();
    for (int i = 0; i < channelMNames.count(); i++) {
        pActvalue = new VfModuleComponent(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                            QString("ACT_Channel%1Peak").arg(i+1),
                                            QString("Actual peak value"));
        m_veinActValueList.append(pActvalue); // we add the component for our measurement
        m_pModule->m_veinComponentsWithMetaAndScpi.append(pActvalue); // and for the modules interface
    }

    pActvalue = new VfModuleComponent(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                        QString("ACT_Frequency"),
                                        QString("Actual frequency"));

    pActvalue->setUnit("Hz");
    pActvalue->setScpiInfo("MEASURE","F", SCPI::isCmdwP);

    m_veinActValueList.append(pActvalue); // we add the component for our measurement
    m_pModule->m_veinComponentsWithMetaAndScpi.append(pActvalue); // and for the modules interface

    for (int i = 0; i < channelMNames.count(); i++) {
        pActvalue = new VfModuleComponent(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                            QString("ACT_Channel%1Rms").arg(i+1),
                                            QString("Actual RMS value (unscaled)"));
        m_veinRmsValueList.append(pActvalue); // we add the component for our measurement
        m_pModule->m_veinComponentsWithMetaAndScpi.append(pActvalue); // and for the modules interface
    }


    m_pMeasureSignal = new VfModuleComponent(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                                QString("SIG_Measuring"),
                                                QString("Signal indicating measurement activity"),
                                                QVariant(0) );
    m_pModule->m_veinComponentsWithMetaAndScpi.append(m_pMeasureSignal); // and for the modules interface
}

void cRangeModuleMeasProgram::setDspVarList()
{
    const QStringList channelMNames = m_pModule->getSharedChannelRangeObserver()->getChannelMNames();
    int samples = m_pModule->getSharedChannelRangeObserver()->getSamplesPerPeriod();
    // we fetch a handle for sampled data and other temporary values
    m_pTmpDataDsp = m_dspInterface->getMemHandle("TmpData");
    m_pTmpDataDsp->addDspVar("MEASSIGNAL", samples, DSPDATA::vDspTemp);
    m_pTmpDataDsp->addDspVar("MAXRESET", 32, DSPDATA::vDspTemp);
    m_pTmpDataDsp->addDspVar("TISTART",1, DSPDATA::vDspTemp, DSPDATA::dInt);
    m_pTmpDataDsp->addDspVar("CHXPEAK",channelMNames.count(), DSPDATA::vDspTemp);
    m_pTmpDataDsp->addDspVar("CHXRMS",channelMNames.count(), DSPDATA::vDspTemp);
    m_pTmpDataDsp->addDspVar("FREQ", 1, DSPDATA::vDspTemp);
    m_pTmpDataDsp->addDspVar("FILTER",2*(2*channelMNames.count()+1),DSPDATA::vDspTemp); // filter workspace for scaled peak, rms and freq
    m_pTmpDataDsp->addDspVar("N",1,DSPDATA::vDspTemp);

    // a handle for parameter
    m_pParameterDSP =  m_dspInterface->getMemHandle("Parameter");
    m_pParameterDSP->addDspVar("TIPAR",1, DSPDATA::vDspParam, DSPDATA::dInt); // integrationtime res = 1ms

    // and one for filtered actual values
    m_pActualValuesDSP = m_dspInterface->getMemHandle("ActualValues");
    m_pActualValuesDSP->addDspVar("CHXPEAKF",channelMNames.count(), DSPDATA::vDspResult); // only copied values from channels maximum from dsp workspace
    m_pActualValuesDSP->addDspVar("CHXRMSF",channelMNames.count(), DSPDATA::vDspResult);
    m_pActualValuesDSP->addDspVar("FREQF", 1, DSPDATA::vDspResult);
    m_pActualValuesDSP->addDspVar("CHXRAWPEAK",channelMNames.count(), DSPDATA::vDspResult);

    m_ModuleActualValues.resize(m_pActualValuesDSP->getSize()); // we provide a vector for generated actual values
}

void cRangeModuleMeasProgram::setDspCmdList()
{
    const QStringList channelMNames = m_pModule->getSharedChannelRangeObserver()->getChannelMNames();
    int samples = m_pModule->getSharedChannelRangeObserver()->getSamplesPerPeriod();
    m_dspInterface->addCycListItem("STARTCHAIN(1,1,0x0101)"); // aktiv, prozessnr. (dummy),hauptkette 1 subkette 1 start
        m_dspInterface->addCycListItem(QString("CLEARN(%1,MEASSIGNAL)").arg(samples) ); // clear meassignal
        m_dspInterface->addCycListItem(QString("CLEARN(%1,FILTER)").arg(2*(2*channelMNames.count()+1)+1) ); // clear the whole filter incl. count
        m_dspInterface->addCycListItem(QString("SETVAL(TIPAR,%1)").arg(getConfData()->m_fMeasInterval*1000.0)); // initial ti time  /* todo variabel */
        m_dspInterface->addCycListItem("GETSTIME(TISTART)"); // einmal ti start setzen
        m_dspInterface->addCycListItem("CLKMODE(1)"); // clk mode auf 48bit einstellen
        m_dspInterface->addCycListItem("DEACTIVATECHAIN(1,0x0101)"); // ende prozessnr., hauptkette 1 subkette 1
    m_dspInterface->addCycListItem("STOPCHAIN(1,0x0101)"); // ende prozessnr., hauptkette 1 subkette 1

    // we compute or copy our wanted actual values
    for (int i = 0; i < channelMNames.count(); i++)
    {
        const ChannelRangeObserver::ChannelPtr channel =
            m_pModule->getSharedChannelRangeObserver()->getChannel(channelMNames.at(i));
        m_dspInterface->addCycListItem(QString("COPYDATA(CH%1,0,MEASSIGNAL)").arg(channel->getDspChannel())); // for each channel we work on
        m_dspInterface->addCycListItem(QString("SETPEAK(MEASSIGNAL,CHXPEAK+%1)").arg(i)); // here we have signal with dc regardless subdc is configured
        m_dspInterface->addCycListItem(QString("MULCCV(MEASSIGNAL,MEASSIGNAL,CHXRMS+%1)").arg(i));
    }
    m_dspInterface->addCycListItem("COPYDU(1,FREQENCY,FREQ)");

    // and filter them
    m_dspInterface->addCycListItem(QString("AVERAGE1(%1,CHXPEAK,FILTER)").arg(2*channelMNames.count()+1)); // we add results to filter
    m_dspInterface->addCycListItem("TESTTIMESKIPNEX(TISTART,TIPAR)");
    m_dspInterface->addCycListItem("ACTIVATECHAIN(1,0x0102)");

    m_dspInterface->addCycListItem("STARTCHAIN(0,1,0x0102)");
        m_dspInterface->addCycListItem("GETSTIME(TISTART)"); // set new system time
        // The following is so assember-ish: We copy CHXPEAKF, CHXRMSF and FREQF here in one line!!!
        m_dspInterface->addCycListItem(QString("CMPAVERAGE1(%1,FILTER,CHXPEAKF)").arg(2*channelMNames.count()+1));

        for (int i = 0; i < channelMNames.count(); i++)
            m_dspInterface->addCycListItem(QString("SQRT(CHXRMSF+%1,CHXRMSF+%2)").arg(i).arg(i));

        m_dspInterface->addCycListItem(QString("CLEARN(%1,FILTER)").arg(2*(2*channelMNames.count()+1)+1) );

        m_dspInterface->addCycListItem("COPYDU(32,MAXIMUMSAMPLE,MAXRESET)"); // all raw adc maximum samples to userspace

        for (int i = 0; i < channelMNames.count(); i++) {
            cRangeMeasChannel* mchn = m_pModule->getMeasChannel(channelMNames.at(i));
            quint8 chnnr = mchn->getDSPChannelNr();
            m_dspInterface->addCycListItem(QString("COPYDU(1,MAXIMUMSAMPLE+%1,CHXRAWPEAK+%2)").arg(chnnr).arg(i)); // raw adc value maximum
            m_dspInterface->addCycListItem(QString("SETVAL(MAXRESET+%1,0.0)").arg(chnnr)); // raw adc value maximum
        }
        m_dspInterface->addCycListItem("COPYUD(32,MAXRESET,MAXIMUMSAMPLE)"); // reset dspworkspace maximum samples

        m_dspInterface->addCycListItem(QString("DSPINTTRIGGER(0x0,0x%1)").arg(/* dummy */ 0)); // send interrupt to module
        m_dspInterface->addCycListItem("DEACTIVATECHAIN(1,0x0102)");
    m_dspInterface->addCycListItem("STOPCHAIN(1,0x0102)"); // end processnr., mainchain 1 subchain 2
}

void cRangeModuleMeasProgram::catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer)
{
    Q_UNUSED(answer)
    if (msgnr == 0) { // 0 was reserved for async. messages
        restartDspWachdog();
        // we got an interrupt from our cmd chain and have to fetch our actual values
        // but we synchronize on ranging process
        if (!m_bRanging) {
            if (!m_bIgnore) {
                if (m_bActive && !m_dataAcquisitionMachine.isRunning()) // in case of deactivation in progress, no dataaquisition
                    m_dataAcquisitionMachine.start();
            }
            else
                m_bIgnore = false;
        }
    }
    else {
        // because rangemodulemeasprogram, justifynorm share the same dsp interface
        if (m_MsgNrCmdList.contains(msgnr)) {
            int cmd = m_MsgNrCmdList.take(msgnr);
            switch (cmd)
            {
            case varlist2dsp:
                if (reply == ack)
                    emit activationContinue();
                else
                    notifyError(dspvarlistwriteErrMsg);
                break;
            case cmdlist2dsp:
                if (reply == ack)
                    emit activationContinue();
                else
                    notifyError(dspcmdlistwriteErrMsg);
                break;
            case activatedsp:
                if (reply == ack)
                    emit activationContinue();
                else
                    notifyError(dspactiveErrMsg);
                break;

            case dataaquistion:
                if (reply == ack)
                    emit dataAquisitionContinue();
                else {
                    m_dataAcquisitionMachine.stop();
                    notifyError(dataaquisitionErrMsg);
                }
                break;
            }
        }
    }
}

cRangeModuleConfigData *cRangeModuleMeasProgram::getConfData()
{
    return qobject_cast<cRangeModuleConfiguration*>(m_pConfiguration.get())->getConfigurationData();
}

void cRangeModuleMeasProgram::setActualValuesNames()
{
    const QStringList channelMNames = m_pModule->getSharedChannelRangeObserver()->getChannelMNames();
    for (int i = 0; i < channelMNames.count(); i++) {
        cRangeMeasChannel* mchn = m_pModule->getMeasChannel(channelMNames.at(i));
        m_veinActValueList.at(i)->setChannelName(mchn->getAlias());
        m_veinActValueList.at(i)->setUnit(mchn->getUnit());
    }
}

void cRangeModuleMeasProgram::setSCPIMeasInfo()
{
    const QStringList channelMNames = m_pModule->getSharedChannelRangeObserver()->getChannelMNames();
    for (int i = 0; i < channelMNames.count(); i++) {
        cRangeMeasChannel* mchn = m_pModule->getMeasChannel(channelMNames.at(i));
        m_veinActValueList.at(i)->setScpiInfo("MEASURE", mchn->getAlias(), SCPI::isCmdwP);
    }
}

void cRangeModuleMeasProgram::restartDspWachdog()
{
    m_dspWatchdogTimer->stop();
    m_dspWatchdogTimer->start();
}

void cRangeModuleMeasProgram::setInterfaceActualValues(QVector<float> *actualValues)
{
    if (m_bActive) {// maybe we are deactivating !!!!
        // ACT_Channel<idx>Peak
        int i;
        for (i = 0; i < m_veinActValueList.count()-1; i++)
            m_veinActValueList.at(i)->setValue(QVariant((*actualValues)[i]));

        // ACT_Frequency
        const int frqIndex = 2*i;
        float freq = (*actualValues)[frqIndex];
        m_veinActValueList.at(i)->setValue(QVariant(freq));
        m_frequencyLogStatistics.addValue(freq);

        // ACT_Channel<idx>Rms
        const QStringList channelMNames = m_pModule->getSharedChannelRangeObserver()->getChannelMNames();
        int rmsOffsetInActual = channelMNames.count();
        for(int channelIdx=0; channelIdx<channelMNames.count(); channelIdx++) {
            cRangeMeasChannel* mchn = m_pModule->getMeasChannel(channelMNames.at(channelIdx));
            float rmsValueScaled = (*actualValues)[channelIdx+rmsOffsetInActual];
            float preScalingFact = mchn->getChannelData()->getPreScaling();
            float rmsValue = rmsValueScaled * preScalingFact; // yes we expected 1/preScalingFact
            m_veinRmsValueList.at(channelIdx)->setValue(QVariant(rmsValue));
        }
    }
}

void cRangeModuleMeasProgram::dspserverConnect()
{
    m_dspClient = Zera::Proxy::getInstance()->getConnectionSmart(m_pModule->getNetworkConfig()->m_dspServiceConnectionInfo,
                                                                 m_pModule->getNetworkConfig()->m_tcpNetworkFactory);
    m_dspInterface->setClientSmart(m_dspClient);
    m_dspserverConnectState.addTransition(m_dspClient.get(), &Zera::ProxyClient::connected, &m_var2DSPState);
    connect(m_dspInterface.get(), &AbstractServerInterface::serverAnswer, this, &cRangeModuleMeasProgram::catchInterfaceAnswer);
    Zera::Proxy::getInstance()->startConnectionSmart(m_dspClient);
}

void cRangeModuleMeasProgram::varList2DSP()
{
    setDspVarList();
    m_MsgNrCmdList[m_dspInterface->varList2Dsp()] = varlist2dsp;
}

void cRangeModuleMeasProgram::cmdList2DSP()
{
    setDspCmdList();
    m_MsgNrCmdList[m_dspInterface->cmdList2Dsp()] = cmdlist2dsp;
}

void cRangeModuleMeasProgram::activateDSP()
{
    m_MsgNrCmdList[m_dspInterface->activateInterface()] = activatedsp; // aktiviert die var- und cmd-listen im dsp
}

void cRangeModuleMeasProgram::activateDSPdone()
{
    m_bActive = true;
    setActualValuesNames();
    setSCPIMeasInfo();
    m_pMeasureSignal->setValue(QVariant(1));
    restartDspWachdog();
    emit activated();
}

void cRangeModuleMeasProgram::deactivateDSPStart()
{
    m_dataAcquisitionMachine.stop();
    m_bActive = false;
    Zera::Proxy::getInstance()->releaseConnectionSmart(m_dspClient);
    disconnect(m_dspInterface.get(), 0, this, 0);
    emit deactivationContinue();
}

void cRangeModuleMeasProgram::dataAcquisitionDSP()
{
    m_pMeasureSignal->setValue(QVariant(0));
    if (m_bActive)
        m_MsgNrCmdList[m_dspInterface->dataAcquisition(m_pActualValuesDSP)] = dataaquistion; // we start our data aquisition now
}

void cRangeModuleMeasProgram::dataReadDSP()
{
    if (m_bActive) {
        m_ModuleActualValues = m_pActualValuesDSP->getData();
        emit actualValues(&m_ModuleActualValues); // and send them
        m_pMeasureSignal->setValue(QVariant(1)); // signal measuring
    }
}

void cRangeModuleMeasProgram::onDspWatchdogTimeout()
{
    qCritical("Rangemodule: DSP is stuck!");
    restartDspWachdog();
}

}
