#include "dspsupermodulemeasprogram.h"
#include "dspsupermodule.h"
#include "dspsupermoduleconfiguration.h"
#include "taskdspdataacquisition.h"
#include <errormessages.h>
#include <intvalidator.h>
#include <scpi.h>
#include <reply.h>
#include <proxy.h>

namespace DSPSUPERMODULE
{

DspSuperModuleMeasProgram::DspSuperModuleMeasProgram(DspSuperModule* module, std::shared_ptr<BaseModuleConfiguration> pConfiguration) :
    cBaseDspMeasProgram(pConfiguration, module->getVeinModuleName()),
    m_pModule(module)
{
    m_dspInterface = m_pModule->getServiceInterfaceFactory()->createDspInterfaceDspSuper(m_pModule->getEntityId());

    m_var2DSPState.addTransition(this, &DspSuperModuleMeasProgram::activationContinue, &m_cmd2DSPState);
    m_cmd2DSPState.addTransition(this, &DspSuperModuleMeasProgram::activationContinue, &m_cmdMakeDspSuperModule);
    m_cmdMakeDspSuperModule.addTransition(this, &DspSuperModuleMeasProgram::activationContinue, &m_activateDSPState);
    m_activateDSPState.addTransition(this, &DspSuperModuleMeasProgram::activationContinue, &m_loadDSPDoneState);

    m_activationMachine.addState(&m_dspserverConnectState);
    m_activationMachine.addState(&m_var2DSPState);
    m_activationMachine.addState(&m_cmd2DSPState);
    m_activationMachine.addState(&m_cmdMakeDspSuperModule);
    m_activationMachine.addState(&m_activateDSPState);
    m_activationMachine.addState(&m_loadDSPDoneState);

    m_activationMachine.setInitialState(&m_dspserverConnectState);

    connect(&m_dspserverConnectState, &QState::entered, this, &DspSuperModuleMeasProgram::dspserverConnect);
    connect(&m_var2DSPState, &QState::entered, this, &DspSuperModuleMeasProgram::varList2DSP);
    connect(&m_cmd2DSPState, &QState::entered, this, &DspSuperModuleMeasProgram::cmdList2DSP);
    connect(&m_cmdMakeDspSuperModule, &QState::entered, this, &DspSuperModuleMeasProgram::sendSuperModuleCmd);
    connect(&m_activateDSPState, &QState::entered, this, &DspSuperModuleMeasProgram::activateDSP);
    connect(&m_loadDSPDoneState, &QState::entered, this, &DspSuperModuleMeasProgram::activateDSPdone);

    m_deactivationMachine.addState(&m_unloadStart);
    m_deactivationMachine.addState(&m_unloadDSPDoneState);

    m_unloadStart.addTransition(this, &DspSuperModuleMeasProgram::deactivationContinue, &m_unloadDSPDoneState);
    m_deactivationMachine.setInitialState(&m_unloadStart);

    connect(&m_unloadStart, &QState::entered, this, &DspSuperModuleMeasProgram::deactivateDSPStart);
    connect(&m_unloadDSPDoneState, &QState::entered, this, &cModuleActivist::deactivated);
}

void DspSuperModuleMeasProgram::generateVeinInterface()
{
    // maybe just for temp debug
    m_veinDspBusyList = new VfModuleComponent(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                              QString("ACT_DSP_BUSY_LIST"),
                                              QString("List of DSP Busy percent values"));
    m_pModule->m_veinComponentsWithMetaAndScpi.append(m_veinDspBusyList);
    m_veinDspPeriodCount = new VfModuleComponent(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                                 QString("ACT_DSP_PERIOD_COUNT"),
                                                 QString("List of DSP period counts"));
    m_pModule->m_veinComponentsWithMetaAndScpi.append(m_veinDspPeriodCount);
    m_veinDspMsTimer = new VfModuleComponent(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                             QString("ACT_DSP_MS_TIMER"),
                                             QString("List of DSP ms timer"));
    m_pModule->m_veinComponentsWithMetaAndScpi.append(m_veinDspMsTimer);
}

void DspSuperModuleMeasProgram::setDspVarList()
{
    // work variables without I/O
    DspVarGroupClientInterface* tmpDspVarGroup = m_dspInterface->createVariableGroup("TmpData");
    tmpDspVarGroup->addDspVar("GLOBALPERIODCOUNT", 1, dspDataTypeInt, moduleGlobalSegment);
    tmpDspVarGroup->addDspVar("LOCAL_BUSY", 1, dspDataTypeFloat);
    tmpDspVarGroup->addDspVar("LOCAL_SYSTIME", 1, dspDataTypeInt);
    tmpDspVarGroup->addDspVar("PERIODCURR", 1, dspDataTypeInt);
    tmpDspVarGroup->addDspVar("PERIODMAX", 1, dspDataTypeInt);
    tmpDspVarGroup->addDspVar("RESULT_BUFF_IDX", 1, dspDataTypeInt);
    tmpDspVarGroup->addDspVar("RESULT_ARRAY_WORK", getConfData()->m_maxPeriods * COUNT_SUPER_ENTRIES, dspDataTypeInt); // double buffer -> RESULT_ARRAY

    // result array (we choose int for transparent transfer)
    m_pActualValuesDSP = m_dspInterface->createVariableGroup("ActualValues");
    m_pActualValuesDSP->addDspVar("RESULT_ARRAY", getConfData()->m_maxPeriods  * COUNT_SUPER_ENTRIES, dspDataTypeInt);
}

void DspSuperModuleMeasProgram::setDspCmdList()
{
    // DspSuperModule
    // * supports other modules on adding reproducible timestamps to measurements
    // * stores DSP busy load indicator on each period
    // It is super special
    // * It is designed to be LAST module in chain because of:
    //   * To reduce number of interrupts thrown it tries to piggy back on other modules interrupts.
    //     In the unlikely case that DspSuperModule's buffers overflow, it causes an own interrupt
    //   * Being last it can prepare global period count for other modules' use in next period
    // * DspSuperModule' asks zdsp1d to handle it's interrupts FIRST. So other modules using timestamps
    //   generated here can rely on up to date timestamps in storage

    // run once
    m_dspInterface->addCycListItem("STARTCHAIN(1,1,0x0101)");
        m_dspInterface->addCycListItem(QString("SETVAL(GLOBALPERIODCOUNT,1)"));
        m_dspInterface->addCycListItem(QString("SETVAL(PERIODCURR,1)"));
        m_dspInterface->addCycListItem(QString("SETVAL(PERIODMAX,%1)").arg(getConfData()->m_maxPeriods));
        m_dspInterface->addCycListItem(QString("SETVAL(RESULT_BUFF_IDX,RESULT_ARRAY_WORK)"));
        m_dspInterface->addCycListItem("DEACTIVATECHAIN(1,0x0101)");
    m_dspInterface->addCycListItem("STOPCHAIN(1,0x0101)");

    // Store tupel of BUSY / global period / ms timer value
    m_dspInterface->addCycListItem("COPYDU(1,BUSY,LOCAL_BUSY)");
    m_dspInterface->addCycListItem("GETSTIME(LOCAL_SYSTIME)");
    // 1. ZDSP Busy
    // 2. Period count
    // 3. Ms Time
    m_dspInterface->addCycListItem("XCOPYMEM(1,LOCAL_BUSY,RESULT_BUFF_IDX)");
    m_dspInterface->addCycListItem("XCOPYMEM(1,GLOBALPERIODCOUNT,RESULT_BUFF_IDX)");
    m_dspInterface->addCycListItem("XCOPYMEM(1,LOCAL_SYSTIME,RESULT_BUFF_IDX)");

    // increment local & global period count
    m_dspInterface->addCycListItem("INC(PERIODCURR)");
    m_dspInterface->addCycListItem("INC(GLOBALPERIODCOUNT)"); // This module runs last => increment after storage to RESULT_ARRAY_WORK

    // notify either on
    // * our loop max reached or
    // * any module has thrown an interupt
    m_dspInterface->addCycListItem("TESTVVSKIPGT(PERIODCURR,PERIODMAX)");
    m_dspInterface->addCycListItem("TESTSKIPNOIRQPENDING()");
    m_dspInterface->addCycListItem("ACTIVATECHAIN(1,0x0102)");

    m_dspInterface->addCycListItem("STARTCHAIN(0,1,0x0102)");
        m_dspInterface->addCycListItem("SETVAL(PERIODCURR,1)");
        m_dspInterface->addCycListItem(QString("SETVAL(RESULT_BUFF_IDX,RESULT_ARRAY_WORK)"));
        m_dspInterface->addCycListItem(QString("COPYMEM(%1,RESULT_ARRAY_WORK,RESULT_ARRAY)").arg(getConfData()->m_maxPeriods*COUNT_SUPER_ENTRIES));
        m_dspInterface->addCycListItem("DSPINTTRIGGER(0x0,0x0)");
        m_dspInterface->addCycListItem("DEACTIVATECHAIN(1,0x0102)");
    m_dspInterface->addCycListItem("STOPCHAIN(1,0x0102)");
}

enum dspsupermoduleCmds
{
    varlist2dsp,
    cmdlist2dsp,
    sendSuperDspCmd,
    activatedsp,
};

void DspSuperModuleMeasProgram::catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer)
{
    Q_UNUSED(answer)
    if (msgnr == 0) // 0 was reserved for async. messages
        startDataAcquisitionDSP();
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
            case sendSuperDspCmd:
                if (reply == ack)
                    emit activationContinue();
                else
                    notifyError(dspSuperCmdErrMsg);
                break;
            case activatedsp:
                if (reply == ack)
                    emit activationContinue();
                else
                    notifyError(dspactiveErrMsg);
                break;
            }
        }
    }
}

DspSuperModuleConfigData *DspSuperModuleMeasProgram::getConfData()
{
    return qobject_cast<DspSuperModuleConfiguration*>(m_pConfiguration.get())->getConfigurationData();
}

void DspSuperModuleMeasProgram::dspserverConnect()
{
    m_dspClient = Zera::Proxy::getInstance()->getConnectionSmart(m_pModule->getNetworkConfig()->m_dspServiceConnectionInfo,
                                                                 m_pModule->getNetworkConfig()->m_tcpNetworkFactory);
    m_dspInterface->setClientSmart(m_dspClient);
    m_dspserverConnectState.addTransition(m_dspClient.get(), &Zera::ProxyClient::connected, &m_var2DSPState);
    connect(m_dspInterface.get(), &AbstractServerInterface::serverAnswer, this, &DspSuperModuleMeasProgram::catchInterfaceAnswer);
    Zera::Proxy::getInstance()->startConnectionSmart(m_dspClient);
}

void DspSuperModuleMeasProgram::varList2DSP()
{
    setDspVarList();
    m_MsgNrCmdList[m_dspInterface->varList2Dsp()] = varlist2dsp;
}

void DspSuperModuleMeasProgram::cmdList2DSP()
{
    setDspCmdList();
    m_MsgNrCmdList[m_dspInterface->cmdList2Dsp()] = cmdlist2dsp;
}

void DspSuperModuleMeasProgram::sendSuperModuleCmd()
{
    m_MsgNrCmdList[m_dspInterface->scpiCommand("SYSTEM:DSP:DSPSUPERCLIENT;")] = sendSuperDspCmd;
}

void DspSuperModuleMeasProgram::activateDSP()
{
    m_MsgNrCmdList[m_dspInterface->activateInterface()] = activatedsp;
}

void DspSuperModuleMeasProgram::activateDSPdone()
{
    m_bActive = true;
    emit activated();
}

void DspSuperModuleMeasProgram::deactivateDSPStart()
{
    m_bActive = false;
    Zera::Proxy::getInstance()->releaseConnectionSmart(m_dspClient);
    disconnect(m_dspInterface.get(), 0, this, 0);
    emit deactivationContinue();
}

void DspSuperModuleMeasProgram::decodeDspDataAcquired()
{
    const QVector<float> &actualValues = m_pActualValuesDSP->getData();
    if (actualValues.size() % COUNT_SUPER_ENTRIES != 0) {
        qCritical("Malformed data received!");
        return;
    }
    DspCommonSupervisorPtr dspCommonSupervisor = m_pModule->getDspCommonSupervisor();
    QList<DspCommonSupervisorEntry> superList = dspCommonSupervisor->getSuperList();

    for (int entryNum=0; entryNum < actualValues.size()/COUNT_SUPER_ENTRIES; ++entryNum) {
        DspCommonSupervisorEntry currEntry;
        for (int idxInEntry=0; idxInEntry<COUNT_SUPER_ENTRIES; ++idxInEntry) {
            float currValue = actualValues[entryNum*COUNT_SUPER_ENTRIES+idxInEntry];
            switch(idxInEntry) {
            case PERCENT_BUSY_FIELD:
                currEntry.m_percentBusy = currValue;
                break;
            // On the way from DSP memory to here we have several horrible casts:
            // 1. zdsp1d / DspVarDeviceNodeInOut::readDspVarList: Memory -> ulong -> string representation
            // 2. Client DSP interface / DspVarGroupClientInterface::setVarData: String representation -> uint
            //    -> float entry within DspVarClientInterface::m_dspVarData float array
            // In DspVarGroupClientInterface::getData() we get a concatenated vector of float of all variables
            // (here just one)
            // => Undo cast in 2.
            case PERIOD_COUNT_FIELD:
                m_currPeriodCount = *reinterpret_cast<uint*>(&currValue); // last is highest and wins
                currEntry.m_periodCount = m_currPeriodCount;
                break;
            case MS_TIMER_FIELD:
                currEntry.m_msTimer  = *reinterpret_cast<uint*>(&currValue);
                break;
            }
        }
        superList.append(currEntry);
    }
    while (superList.size() > getConfData()->m_maxPeriods)
        superList.removeFirst();
    dspCommonSupervisor->setSuperList(superList);
}

void DspSuperModuleMeasProgram::dataToVein()
{
    // Avoid high frequency fire / no perfect accuracy von vein required (maybe lists will go)
    if (m_currPeriodCount - m_periodCountToVeinLast < getConfData()->m_maxPeriods)
        return;
    m_periodCountToVeinLast = m_currPeriodCount;

    QList<double> dspBusyList;
    QList<int> periodCountList;
    QList<int> msTimerList;
    DspCommonSupervisorPtr dspCommonSupervisor = m_pModule->getDspCommonSupervisor();
    const QList<DspCommonSupervisorEntry> &superList = dspCommonSupervisor->getSuperList();
    for (const DspCommonSupervisorEntry &entry : superList) {
        dspBusyList.append(entry.m_percentBusy);
        periodCountList.append(entry.m_periodCount);
        msTimerList.append(entry.m_msTimer);
    }
    m_veinDspBusyList->setValue(QVariant::fromValue<QList<double>>(dspBusyList));
    m_veinDspPeriodCount->setValue(QVariant::fromValue<QList<int>>(periodCountList));
    m_veinDspMsTimer->setValue(QVariant::fromValue<QList<int>>(msTimerList));
}

void DspSuperModuleMeasProgram::startDataAcquisitionDSP()
{
    if (m_bActive && m_taskDataAcquisition == nullptr) {
        m_taskDataAcquisition = TaskDspDataAcquisition::create(m_dspInterface, m_pActualValuesDSP);
        connect(m_taskDataAcquisition.get(), &TaskTemplate::sigFinish, [&](bool ok) {
            m_taskDataAcquisition.reset();
            if (ok && m_bActive) {
                decodeDspDataAcquired();
                dataToVein();
            }
            else
                notifyError(dataaquisitionErrMsg);
        });
        m_taskDataAcquisition->start();
    }
}

}
