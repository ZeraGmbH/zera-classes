#include "periodaveragemodulemeasprogram.h"
#include "periodaveragemodule.h"
#include "periodaveragemoduleconfiguration.h"
#include "servicechannelnamehelper.h"
#include "vfmodulecomponent.h"
#include <errormessages.h>
#include <intvalidator.h>
#include <scpi.h>
#include <reply.h>
#include <proxy.h>

namespace PERIODAVERAGEMODULE
{

PeriodAverageModuleMeasProgram::PeriodAverageModuleMeasProgram(PeriodAverageModule* module, std::shared_ptr<BaseModuleConfiguration> pConfiguration) :
    cBaseDspMeasProgram(pConfiguration, module->getVeinModuleName()),
    m_pModule(module),
    m_observer(m_pModule->getSharedChannelRangeObserver())
{
    m_dspInterface = m_pModule->getServiceInterfaceFactory()->createDspInterfacePeriodAverage(
        m_pModule->getEntityId(),
        getConfData()->m_valueChannelList);

    m_var2DSPState.addTransition(this, &PeriodAverageModuleMeasProgram::activationContinue, &m_cmd2DSPState);
    m_cmd2DSPState.addTransition(this, &PeriodAverageModuleMeasProgram::activationContinue, &m_activateDSPState);
    m_activateDSPState.addTransition(this, &PeriodAverageModuleMeasProgram::activationContinue, &m_loadDSPDoneState);

    m_activationMachine.addState(&m_dspserverConnectState);
    m_activationMachine.addState(&m_var2DSPState);
    m_activationMachine.addState(&m_cmd2DSPState);
    m_activationMachine.addState(&m_activateDSPState);
    m_activationMachine.addState(&m_loadDSPDoneState);

    m_activationMachine.setInitialState(&m_dspserverConnectState);

    connect(&m_dspserverConnectState, &QState::entered, this, &PeriodAverageModuleMeasProgram::dspserverConnect);
    connect(&m_var2DSPState, &QState::entered, this, &PeriodAverageModuleMeasProgram::varList2DSP);
    connect(&m_cmd2DSPState, &QState::entered, this, &PeriodAverageModuleMeasProgram::cmdList2DSP);
    connect(&m_activateDSPState, &QState::entered, this, &PeriodAverageModuleMeasProgram::activateDSP);
    connect(&m_loadDSPDoneState, &QState::entered, this, &PeriodAverageModuleMeasProgram::activateDSPdone);

    m_deactivationMachine.addState(&m_unloadStart);
    m_deactivationMachine.addState(&m_unloadDSPDoneState);

    m_unloadStart.addTransition(this, &PeriodAverageModuleMeasProgram::deactivationContinue, &m_unloadDSPDoneState);
    m_deactivationMachine.setInitialState(&m_unloadStart);

    connect(&m_unloadStart, &QState::entered, this, &PeriodAverageModuleMeasProgram::deactivateDSPStart);
    connect(&m_unloadDSPDoneState, &QState::entered, this, &cModuleActivist::deactivated);

    // setting up statemachine for data acquisition
    m_dataAcquisitionState.addTransition(this, &PeriodAverageModuleMeasProgram::dataAquisitionContinue, &m_dataAcquisitionDoneState);
    m_dataAcquisitionMachine.addState(&m_dataAcquisitionState);
    m_dataAcquisitionMachine.addState(&m_dataAcquisitionDoneState);
    m_dataAcquisitionMachine.setInitialState(&m_dataAcquisitionState);
    connect(&m_dataAcquisitionState, &QState::entered, this, &PeriodAverageModuleMeasProgram::dataAcquisitionDSP);
    connect(&m_dataAcquisitionDoneState, &QState::entered, this, &PeriodAverageModuleMeasProgram::dataReadDSP);

    connect(this, &PeriodAverageModuleMeasProgram::actualValues,
            &m_startStopHandler, &ActualValueStartStopHandler::onNewActualValues);
    connect(&m_startStopHandler, &ActualValueStartStopHandler::sigNewActualValues,
            this, &PeriodAverageModuleMeasProgram::setInterfaceActualValues);
}

void PeriodAverageModuleMeasProgram::start()
{
    m_startStopHandler.start();
}

void PeriodAverageModuleMeasProgram::stop()
{
    m_startStopHandler.stop();
}

void PeriodAverageModuleMeasProgram::generateVeinInterface()
{
    for (int i = 0; i < getConfData()->m_valueChannelList.count(); i++) {
        const QString channeMName = getConfData()->m_valueChannelList.at(i);
        ServiceChannelNameHelper::TChannelAliasUnit aliasUnit = ServiceChannelNameHelper::getChannelAndUnit(channeMName, m_observer);

        VfModuleComponent *pActvalue = new VfModuleComponent(m_pModule->getEntityId(),
                                                             m_pModule->getValidatorEventSystem(),
                                                             QString("ACT_PERIOD_VALUES%1").arg(i+1),
                                                             QString("Array of per period measurement values"));
        pActvalue->setChannelName(aliasUnit.m_channelAlias);
        pActvalue->setUnit(aliasUnit.m_channelUnit);
        pActvalue->setScpiInfo("MEASURE", aliasUnit.m_channelAlias, SCPI::isCmdwP);
        m_pModule->m_veinComponentsWithMetaAndScpi.append(pActvalue); // and for the modules interface
    }

    QString key;
    m_periodCountParameter = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                                   key = QString("PAR_PeriodCount"),
                                                   QString("Measurement periods collected per measurement"),
                                                   QVariant(getConfData()->m_periodCount.m_nValue));
    m_periodCountParameter->setScpiInfo("CONFIGURATION","PCOUNT", SCPI::isQuery|SCPI::isCmdwP);
    m_periodCountParameter->setValidator(new cIntValidator(10, MaxPeriods));
    m_pModule->m_veinModuleParameterMap[key] = m_periodCountParameter; // for modules use

    m_pMeasureSignal = new VfModuleComponent(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                                QString("SIG_Measuring"),
                                                QString("Signal indicating measurement activity"),
                                                QVariant(0));

    m_pModule->m_veinComponentsWithMetaAndScpi.append(m_pMeasureSignal);
}


void PeriodAverageModuleMeasProgram::setDspVarList()
{
    const int samplesPerPeriod = m_observer->getSamplesPerPeriod();
    const int channelCount = m_observer->getChannelMNames().count();

    // temporary values
    m_pTmpDataDsp = m_dspInterface->getMemHandle("TmpData");
    m_pTmpDataDsp->addDspVar("MEASSIGNAL", samplesPerPeriod, DSPDATA::vDspTemp);
    m_pTmpDataDsp->addDspVar("INTEGRAL_RESULT", channelCount, DSPDATA::vDspTemp);
    m_pTmpDataDsp->addDspVar("PERIODCURR", 1, DSPDATA::vDspTemp, DSPDATA::dInt);
    m_pTmpDataDsp->addDspVar("FILTER", 2*channelCount, DSPDATA::vDspTemp);
    m_pTmpDataDsp->addDspVar("N", 1, DSPDATA::vDspTemp);

    // parameter
    m_pParameterDSP =  m_dspInterface->getMemHandle("Parameter");
    m_pParameterDSP->addDspVar("PERIODCOUNT", 1, DSPDATA::vDspParam, DSPDATA::dInt);

    // results
    m_pActualValuesDSP = m_dspInterface->getMemHandle("ActualValues");
    m_pActualValuesDSP->addDspVar("VALS_PERIOD", MaxPeriods*channelCount, DSPDATA::vDspResult);
    m_pActualValuesDSP->addDspVar("VALUES_AVG", channelCount, DSPDATA::vDspResult);

    m_ModuleActualValues.resize(m_pActualValuesDSP->getSize()); // we provide a vector for generated actual values
}

void PeriodAverageModuleMeasProgram::setDspCmdList()
{
    int samplesPerPeriod = m_observer->getSamplesPerPeriod();
    const QStringList channelMNameListConfigured = getConfData()->m_valueChannelList;
    int channelCount = channelMNameListConfigured.count();

    m_dspInterface->addCycListItem("STARTCHAIN(1,1,0x0101)"); // run once
        m_dspInterface->addCycListItem(QString("CLEARN(%1,MEASSIGNAL)").arg(samplesPerPeriod) ); // clear meassignal
        m_dspInterface->addCycListItem(QString("CLEARN(%1,FILTER)").arg(2*channelCount+1) ); // clear the whole filter incl. count

        m_dspInterface->addCycListItem(QString("SETVAL(PERIODCURR,0)"));
        m_dspInterface->addCycListItem(QString("SETVAL(PERIODCOUNT,%1)").arg(getConfData()->m_periodCount.m_nValue));

        m_dspInterface->addCycListItem("DEACTIVATECHAIN(1,0x0101)"); // ende prozessnr., hauptkette 1 subkette 1
    m_dspInterface->addCycListItem("STOPCHAIN(1,0x0101)"); // ende prozessnr., hauptkette 1 subkette 1

    for (int channelNo=0; channelNo<channelCount; ++channelNo) { // per channel INTEGRAL_RESULT
        const QString &channelMName = channelMNameListConfigured[channelNo];
        ChannelRangeObserver::ChannelPtr channel = m_observer->getChannel(channelMName);
        int dspChannelNo = channel->getDspChannel();
        m_dspInterface->addCycListItem(QString("COPYDATA(CH%1,0,MEASSIGNAL)").arg(dspChannelNo));
        m_dspInterface->addCycListItem(QString("INTEGRAL(%1,MEASSIGNAL,INTEGRAL_RESULT+%2)").arg(samplesPerPeriod).arg(channelNo));
    }
    for (int periodNo=0; periodNo<MaxPeriods; ++periodNo) { // INTEGRAL_RESULT -> per period slot in VALS_PERIOD
        int offset = periodNo*channelCount;
        m_dspInterface->addCycListItem(QString("TESTVCSKIPGT(PERIODCURR,%1)").arg(periodNo));
        m_dspInterface->addCycListItem(QString("COPYMEM(%1,INTEGRAL_RESULT,VALS_PERIOD+%2)").arg(channelCount).arg(offset));
    }
    m_dspInterface->addCycListItem(QString("AVERAGE1(%1,INTEGRAL_RESULT,FILTER)").arg(2*channelCount)); // add results to filter

    m_dspInterface->addCycListItem("TESTVVSKIPEQ(PERIODCURR,PERIODCOUNT)");
    m_dspInterface->addCycListItem("ACTIVATECHAIN(1,0x0102)");
    m_dspInterface->addCycListItem("STARTCHAIN(0,1,0x0102)");
        m_dspInterface->addCycListItem(QString("SETVAL(PERIODCURR,0)"));
        m_dspInterface->addCycListItem(QString("CMPAVERAGE1(%1,FILTER,VALUES_AVG)").arg(2*channelCount));
        m_dspInterface->addCycListItem(QString("CLEARN(%1,FILTER)").arg(2*channelCount+1) );
        m_dspInterface->addCycListItem(QString("DSPINTTRIGGER(0x0,0x%1)").arg(0));
        m_dspInterface->addCycListItem("DEACTIVATECHAIN(1,0x0102)");
    m_dspInterface->addCycListItem("STOPCHAIN(1,0x0102)"); // end processnr., mainchain 1 subchain 2

    m_dspInterface->addCycListItem("INC(PERIODCURR)");
}

void PeriodAverageModuleMeasProgram::catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer)
{
    Q_UNUSED(answer)
    if (msgnr == 0) { // 0 was reserved for async. messages
        // we got an interrupt from our cmd chain and have to fetch our actual values
        // but we synchronize on ranging process
        if (m_bActive && !m_dataAcquisitionMachine.isRunning()) // in case of deactivation in progress, no dataaquisition
            m_dataAcquisitionMachine.start();
    }
    else {
        // maybe other objexts share the same dsp interface
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

            case writeparameter:
                if (reply == ack) // we ignore ack
                    ;
                else
                    notifyError(writedspmemoryErrMsg);
                break;

            case deactivatedsp:
                if (reply == ack)
                    emit deactivationContinue();
                else
                    notifyError(dspdeactiveErrMsg);
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

PeriodAverageModuleConfigData *PeriodAverageModuleMeasProgram::getConfData()
{
    return qobject_cast<PeriodAverageModuleConfiguration*>(m_pConfiguration.get())->getConfigurationData();
}

void PeriodAverageModuleMeasProgram::setInterfaceActualValues(QVector<float> *actualValues)
{
    if (m_bActive) { // maybe we are deactivating !!!!


    }
}

void PeriodAverageModuleMeasProgram::dspserverConnect()
{
    m_dspClient = Zera::Proxy::getInstance()->getConnectionSmart(m_pModule->getNetworkConfig()->m_dspServiceConnectionInfo,
                                                                 m_pModule->getNetworkConfig()->m_tcpNetworkFactory);
    m_dspInterface->setClientSmart(m_dspClient);
    m_dspserverConnectState.addTransition(m_dspClient.get(), &Zera::ProxyClient::connected, &m_var2DSPState);
    connect(m_dspInterface.get(), &AbstractServerInterface::serverAnswer, this, &PeriodAverageModuleMeasProgram::catchInterfaceAnswer);
    Zera::Proxy::getInstance()->startConnectionSmart(m_dspClient);
}

void PeriodAverageModuleMeasProgram::varList2DSP()
{
    setDspVarList();
    m_MsgNrCmdList[m_dspInterface->varList2Dsp()] = varlist2dsp;
}

void PeriodAverageModuleMeasProgram::cmdList2DSP()
{
    setDspCmdList();
    m_MsgNrCmdList[m_dspInterface->cmdList2Dsp()] = cmdlist2dsp;
}

void PeriodAverageModuleMeasProgram::activateDSP()
{
    m_MsgNrCmdList[m_dspInterface->activateInterface()] = activatedsp;
}

void PeriodAverageModuleMeasProgram::activateDSPdone()
{
    m_bActive = true;

    m_pMeasureSignal->setValue(QVariant(1));
    connect(m_periodCountParameter, &VfModuleParameter::sigValueChanged, this, &PeriodAverageModuleMeasProgram::newPeriodCount);

    emit activated();
}

void PeriodAverageModuleMeasProgram::deactivateDSPStart()
{
    m_dataAcquisitionMachine.stop();
    m_bActive = false;
    Zera::Proxy::getInstance()->releaseConnectionSmart(m_dspClient); // no async. messages anymore
    disconnect(m_dspInterface.get(), 0, this, 0);
    emit deactivationContinue();
}

void PeriodAverageModuleMeasProgram::dataAcquisitionDSP()
{
    m_pMeasureSignal->setValue(QVariant(0));
    if(m_bActive)
        m_MsgNrCmdList[m_dspInterface->dataAcquisition(m_pActualValuesDSP)] = dataaquistion;
}

void PeriodAverageModuleMeasProgram::dataReadDSP()
{
    if (m_bActive) {
        m_ModuleActualValues = m_pActualValuesDSP->getData();
        emit actualValues(&m_ModuleActualValues);
        m_pMeasureSignal->setValue(QVariant(1));
    }
}

void PeriodAverageModuleMeasProgram::newPeriodCount(const QVariant &periodCount)
{
    getConfData()->m_periodCount.m_nValue = periodCount.toInt();
    m_pParameterDSP->setVarData(QString("PERIODCOUNT:%1;PERIODCURR:0;").arg(getConfData()->m_periodCount.m_nValue));
    m_MsgNrCmdList[m_dspInterface->dspMemoryWrite(m_pParameterDSP)] = writeparameter;
    emit m_pModule->parameterChanged();
}

}
