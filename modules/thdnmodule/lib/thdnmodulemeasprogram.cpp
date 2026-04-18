#include "thdnmodulemeasprogram.h"
#include "thdnmodule.h"
#include "thdnmoduleconfiguration.h"
#include "servicechannelnamehelper.h"
#include "errormessages.h"
#include "taskdspdataacquisition.h"
#include <reply.h>
#include <movingwindowfilter.h>
#include <proxy.h>
#include <scpi.h>
#include <vfmodulecomponent.h>
#include <vfmoduleparameter.h>
#include <doublevalidator.h>
#include <intvalidator.h>
#include <dspinterface.h>
#include <math.h>
#include <QString>

namespace THDNMODULE
{

cThdnModuleMeasProgram::cThdnModuleMeasProgram(cThdnModule *module, std::shared_ptr<BaseModuleConfiguration> pConfiguration) :
    cBaseDspMeasProgram(pConfiguration, module->getVeinModuleName()),
    m_pModule(module)
{
    m_dspInterface = m_pModule->getServiceInterfaceFactory()->createDspInterfaceThdn(
        m_pModule->getEntityId(),
        m_pModule->getSharedChannelRangeObserver()->getChannelMNames());

    m_var2DSPState.addTransition(this, &cThdnModuleMeasProgram::activationContinue, &m_cmd2DSPState);
    m_cmd2DSPState.addTransition(this, &cThdnModuleMeasProgram::activationContinue, &m_activateDSPState);
    m_activateDSPState.addTransition(this, &cThdnModuleMeasProgram::activationContinue, &m_loadDSPDoneState);

    m_activationMachine.addState(&m_dspserverConnectState);
    m_activationMachine.addState(&m_var2DSPState);
    m_activationMachine.addState(&m_cmd2DSPState);
    m_activationMachine.addState(&m_activateDSPState);
    m_activationMachine.addState(&m_loadDSPDoneState);

    m_activationMachine.setInitialState(&m_dspserverConnectState);

    connect(&m_dspserverConnectState, &QAbstractState::entered, this, &cThdnModuleMeasProgram::dspserverConnect);
    connect(&m_var2DSPState, &QAbstractState::entered, this, &cThdnModuleMeasProgram::varList2DSP);
    connect(&m_cmd2DSPState, &QAbstractState::entered, this, &cThdnModuleMeasProgram::cmdList2DSP);
    connect(&m_activateDSPState, &QAbstractState::entered, this, &cThdnModuleMeasProgram::activateDSP);
    connect(&m_loadDSPDoneState, &QAbstractState::entered, this, &cThdnModuleMeasProgram::activateDSPdone);

    m_deactivationMachine.addState(&m_unloadStart);
    m_deactivationMachine.addState(&m_unloadDSPDoneState);

    m_unloadStart.addTransition(this, &cThdnModuleMeasProgram::deactivationContinue, &m_unloadDSPDoneState);
    m_deactivationMachine.setInitialState(&m_unloadStart);

    connect(&m_unloadStart, &QState::entered, this, &cThdnModuleMeasProgram::deactivateDSPStart);
    connect(&m_unloadDSPDoneState, &QState::entered, this, &cModuleActivist::deactivated);

    connect(this, &cThdnModuleMeasProgram::actualValues,
            &m_startStopHandler, &ActualValueStartStopHandler::onNewActualValues);
    if (getConfData()->m_bmovingWindow) {
        m_movingwindowFilter.setIntegrationTime(getConfData()->m_fMeasInterval.m_fValue);
        connect(&m_startStopHandler, &ActualValueStartStopHandler::sigNewActualValues,
                &m_movingwindowFilter, &MovingwindowFilter::receiveActualValues);
        connect(&m_movingwindowFilter, &MovingwindowFilter::sigActualValues,
                this, &cThdnModuleMeasProgram::setInterfaceActualValues);
    }
    else
        connect(&m_startStopHandler, &ActualValueStartStopHandler::sigNewActualValues,
                this, &cThdnModuleMeasProgram::setInterfaceActualValues);
}

void cThdnModuleMeasProgram::start()
{
    m_startStopHandler.start();
}

void cThdnModuleMeasProgram::stop()
{
    m_startStopHandler.stop();
}

void cThdnModuleMeasProgram::generateVeinInterface()
{
    ChannelRangeObserver::SystemObserverPtr observer = m_pModule->getSharedChannelRangeObserver();
    const QStringList channelMNames = observer->getChannelMNames();
    for (int i = 0; i < channelMNames.size(); i++) {
        const QString channelMName = channelMNames[i];
        ServiceChannelNameHelper::TChannelAliasUnit aliasUnit =
            ServiceChannelNameHelper::getChannelAndUnit(channelMName, observer);
        const QString &channelAlias = aliasUnit.m_channelAlias;

        VfModuleComponent *pActvalue;
        pActvalue = new VfModuleComponent(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                          QString("ACT_THDN%1").arg(i+1),
                                          QString("THDn actual value"));
        pActvalue->setChannelName(channelAlias);
        pActvalue->setUnit("%");
        pActvalue->setScpiInfo("MEASURE", channelAlias, SCPI::isCmdwP);
        m_veinActValueList.append(pActvalue);
        m_pModule->m_veinComponentsWithMetaAndScpi.append(pActvalue);

        // for thdn shadow module - no SCPI
        pActvalue = new VfModuleComponent(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                          QString("ACT_THDR%1").arg(i+1),
                                          QString("THDr actual value"));
        pActvalue->setChannelName(channelAlias);
        pActvalue->setUnit("%");
        m_veinActValueList.append(pActvalue);
        m_pModule->m_veinComponentsWithMetaAndScpi.append(pActvalue);
    }
    QString key;
    m_pIntegrationTimeParameter = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                                           key = QString("PAR_Interval"),
                                                           QString("Integration time"),
                                                           QVariant(getConfData()->m_fMeasInterval.m_fValue));
    m_pIntegrationTimeParameter->setUnit("s");
    m_pIntegrationTimeParameter->setScpiInfo("CONFIGURATION","TINTEGRATION", SCPI::isQuery|SCPI::isCmdwP);
    m_pIntegrationTimeParameter->setValidator(new cDoubleValidator(1.0, 100.0, 0.5));
    m_pModule->m_veinModuleParameterMap[key] = m_pIntegrationTimeParameter; // for modules use

    m_pMeasureSignal = new VfModuleComponent(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                                QString("SIG_Measuring"),
                                                QString("Signal indicating measurement activity"),
                                                QVariant(0));

    m_pModule->m_veinComponentsWithMetaAndScpi.append(m_pMeasureSignal);
}

void cThdnModuleMeasProgram::setDspVarList()
{
    int samples = m_pModule->getSharedChannelRangeObserver()->getSamplesPerPeriod();
    ChannelRangeObserver::SystemObserverPtr observer = m_pModule->getSharedChannelRangeObserver();
    const QStringList channelMNames = observer->getChannelMNames();

    // work variables without I/O
    const int thdnCountDsp = channelMNames.size();
    DspVarGroupClientInterface* tmpDspVarGroup = m_dspInterface->createVariableGroup("TmpData");
    tmpDspVarGroup->addDspVar("TMP_SAMPLES_SINGLE_1", samples, dspDataTypeFloat, moduleGlobalSegment);
    tmpDspVarGroup->addDspVar("VALXTHDN", thdnCountDsp);
    tmpDspVarGroup->addDspVar("FILTER", DspBuffLen::avgFilterLen(thdnCountDsp));

    // a handle for parameter
    m_pParameterDSP =  m_dspInterface->createVariableGroup("Parameter");
    m_pParameterDSP->addDspVar("TIPAR", 1, dspDataTypeInt); // integrationtime res = 1ms
    // we use tistart as parameter, so we can finish actual measuring interval bei setting 0
    m_pParameterDSP->addDspVar("TISTART", 1, dspDataTypeInt);

    // and one for filtered actual values
    m_pActualValuesDSP = m_dspInterface->createVariableGroup("ActualValues");
    m_pActualValuesDSP->addDspVar("VALXTHDNF", thdnCountDsp);
    m_ModuleActualValues.resize(m_pActualValuesDSP->getUserMemSize()); // we provide a vector for generated actual values
}

void cThdnModuleMeasProgram::setDspCmdList()
{
    ChannelRangeObserver::SystemObserverPtr observer = m_pModule->getSharedChannelRangeObserver();
    int samples = observer->getSamplesPerPeriod();
    const QStringList channelMNames = observer->getChannelMNames();
    const int thdnCountDsp = channelMNames.size();
    m_dspInterface->addCycListItem("STARTCHAIN(1,1,0x0101)"); // aktiv, prozessnr. (dummy),hauptkette 1 subkette 1 start
        m_dspInterface->addCycListItem(QString("CLEARN(%1,TMP_SAMPLES_SINGLE_1)").arg(samples) ); // clear TMP_SAMPLES_SINGLE_1
        m_dspInterface->addCycListItem(QString("CLEARN(%1,FILTER)").arg(DspBuffLen::avgFilterLen(thdnCountDsp)));

        if (getConfData()->m_bmovingWindow)
            m_dspInterface->addCycListItem(QString("SETVAL(TIPAR,%1)").arg(getConfData()->m_fmovingwindowInterval*1000.0)); // initial ti time
        else
            m_dspInterface->addCycListItem(QString("SETVAL(TIPAR,%1)").arg(getConfData()->m_fMeasInterval.m_fValue*1000.0)); // initial ti time

        m_dspInterface->addCycListItem("GETSTIME(TISTART)"); // einmal ti start setzen
        m_dspInterface->addCycListItem("DEACTIVATECHAIN(1,0x0101)"); // ende prozessnr., hauptkette 1 subkette 1
    m_dspInterface->addCycListItem("STOPCHAIN(1,0x0101)"); // ende prozessnr., hauptkette 1 subkette 1

    // we compute or copy our wanted actual values
    for (int i = 0; i < thdnCountDsp; i++) {
        QString channelMName = channelMNames[i];
        int dspChannel = observer->getChannel(channelMName)->getDspChannel();
        m_dspInterface->addCycListItem(QString("COPYDATA(CH%1,0,TMP_SAMPLES_SINGLE_1)").arg(dspChannel));
        m_dspInterface->addCycListItem(QString("THDN(TMP_SAMPLES_SINGLE_1,VALXTHDN+%1)").arg(i));
    }

    // and filter them
    m_dspInterface->addCycListItem(QString("AVERAGE1(%1,VALXTHDN,FILTER)").arg(thdnCountDsp)); // we add results to filter

    m_dspInterface->addCycListItem("TESTTIMESKIPNEX(TISTART,TIPAR)");
    m_dspInterface->addCycListItem("ACTIVATECHAIN(1,0x0102)");

    m_dspInterface->addCycListItem("STARTCHAIN(0,1,0x0102)");
        m_dspInterface->addCycListItem("GETSTIME(TISTART)"); // set new system time
        m_dspInterface->addCycListItem(QString("CMPAVERAGE1(%1,FILTER,VALXTHDNF)").arg(thdnCountDsp));
        m_dspInterface->addCycListItem(QString("CLEARN(%1,FILTER)").arg(DspBuffLen::avgFilterLen(thdnCountDsp)));
        m_dspInterface->addCycListItem(QString("DSPINTTRIGGER(0x0,0x%1)").arg(0)); // send interrupt to module
        m_dspInterface->addCycListItem("DEACTIVATECHAIN(1,0x0102)");
    m_dspInterface->addCycListItem("STOPCHAIN(1,0x0102)"); // end processnr., mainchain 1 subchain 2
}

enum thdnmoduleCmds
{
    varlist2dsp,
    cmdlist2dsp,
    activatedsp,
    writeparameter,
};

void cThdnModuleMeasProgram::catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer)
{
    Q_UNUSED(answer)
    if (msgnr == 0) // 0 was reserved for async. messages
        dataAcquisitionDSP();
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
            }
        }
    }
}

cThdnModuleConfigData *cThdnModuleMeasProgram::getConfData()
{
    return qobject_cast<cThdnModuleConfiguration*>(m_pConfiguration.get())->getConfigurationData();
}

void cThdnModuleMeasProgram::setInterfaceActualValues(QVector<float> *actualValues)
{
    if (m_bActive) { // maybe we are deactivating !!!!
        for (int i = 0; i < actualValues->count(); i++) {
            double dspThdnValue = actualValues->at(i);
            m_veinActValueList.at(2*i)->setValue(dspThdnValue);

            double thdn = actualValues->at(i) / 100.0;
            double thdr = 100.0 * thdn / sqrt(1 + (thdn * thdn));
            m_veinActValueList.at(2*i+1)->setValue(thdr);
        }
    }
}

void cThdnModuleMeasProgram::dspserverConnect()
{
    m_dspClient = Zera::Proxy::getInstance()->getConnectionSmart(m_pModule->getNetworkConfig()->m_dspServiceConnectionInfo,
                                                                 m_pModule->getNetworkConfig()->m_tcpNetworkFactory);
    m_dspInterface->setClientSmart(m_dspClient);
    m_dspserverConnectState.addTransition(m_dspClient.get(), &Zera::ProxyClient::connected, &m_var2DSPState);
    connect(m_dspInterface.get(), &AbstractServerInterface::serverAnswer, this, &cThdnModuleMeasProgram::catchInterfaceAnswer);
    Zera::Proxy::getInstance()->startConnectionSmart(m_dspClient);
}

void cThdnModuleMeasProgram::varList2DSP()
{
    setDspVarList();
    m_MsgNrCmdList[m_dspInterface->varList2Dsp()] = varlist2dsp;
}

void cThdnModuleMeasProgram::cmdList2DSP()
{
    setDspCmdList();
    m_MsgNrCmdList[m_dspInterface->cmdList2Dsp()] = cmdlist2dsp;
}

void cThdnModuleMeasProgram::activateDSP()
{
    m_MsgNrCmdList[m_dspInterface->activateInterface()] = activatedsp; // aktiviert die var- und cmd-listen im dsp
}

void cThdnModuleMeasProgram::activateDSPdone()
{
    m_bActive = true;

    m_pMeasureSignal->setValue(QVariant(1));
    connect(m_pIntegrationTimeParameter, &VfModuleComponent::sigValueChanged, this, &cThdnModuleMeasProgram::newIntegrationtime);

    emit activated();
}

void cThdnModuleMeasProgram::deactivateDSPStart()
{
    m_bActive = false;
    Zera::Proxy::getInstance()->releaseConnectionSmart(m_dspClient); // no async. messages anymore
    disconnect(m_dspInterface.get(), 0, this, 0);
    emit deactivationContinue();
}

void cThdnModuleMeasProgram::dataAcquisitionDSP()
{
    if (m_bActive && m_taskDataAcquisition == nullptr) {
        m_pMeasureSignal->setValue(QVariant(0));
        m_taskDataAcquisition = TaskDspDataAcquisition::create(m_dspInterface, m_pActualValuesDSP);
        connect(m_taskDataAcquisition.get(), &TaskTemplate::sigFinish, this, [&](bool ok) {
            m_taskDataAcquisition.reset();
            if (ok)
                dataReadDSP();
            else
                notifyError(dataaquisitionErrMsg);
        });
        m_taskDataAcquisition->start();
    }
}

void cThdnModuleMeasProgram::dataReadDSP()
{
    if (m_bActive) {
        m_ModuleActualValues = m_pActualValuesDSP->getData();
        emit actualValues(&m_ModuleActualValues); // and send them
        m_pMeasureSignal->setValue(QVariant(1)); // signal measuring
    }
}

void cThdnModuleMeasProgram::newIntegrationtime(const QVariant &ti)
{
    getConfData()->m_fMeasInterval.m_fValue = ti.toDouble();
    if (getConfData()->m_bmovingWindow)
        m_movingwindowFilter.setIntegrationTime(getConfData()->m_fMeasInterval.m_fValue);
    else {
        m_pParameterDSP->setVarData(QString("TIPAR:%1;TISTART:0;").arg(getConfData()->m_fMeasInterval.m_fValue*1000));
        m_MsgNrCmdList[m_dspInterface->dspMemoryWrite(m_pParameterDSP)] = writeparameter;
    }
    emit m_pModule->parameterChanged();
}

}
