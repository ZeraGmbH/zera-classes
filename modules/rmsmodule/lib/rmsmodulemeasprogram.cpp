#include "rmsmodulemeasprogram.h"
#include "rmsmodule.h"
#include "rmsmoduleconfiguration.h"
#include "servicechannelnamehelper.h"
#include <errormessages.h>
#include <reply.h>
#include <proxy.h>
#include <doublevalidator.h>
#include <intvalidator.h>
#include <scpi.h>
#include <timerfactoryqt.h>

namespace RMSMODULE
{

cRmsModuleMeasProgram::cRmsModuleMeasProgram(cRmsModule* module,
                                             std::shared_ptr<BaseModuleConfiguration> pConfiguration) :
    cBaseDspMeasProgram(pConfiguration, module->getVeinModuleName()),
    m_pModule(module)
{
    m_dspInterface = m_pModule->getServiceInterfaceFactory()->createDspInterfaceRms(
        m_pModule->getEntityId(),
        getConfData()->m_valueChannelList);

    m_var2DSPState.addTransition(this, &cRmsModuleMeasProgram::activationContinue, &m_cmd2DSPState);
    m_cmd2DSPState.addTransition(this, &cRmsModuleMeasProgram::activationContinue, &m_activateDSPState);
    m_activateDSPState.addTransition(this, &cRmsModuleMeasProgram::activationContinue, &m_loadDSPDoneState);

    m_activationMachine.addState(&m_dspserverConnectState);
    m_activationMachine.addState(&m_var2DSPState);
    m_activationMachine.addState(&m_cmd2DSPState);
    m_activationMachine.addState(&m_activateDSPState);
    m_activationMachine.addState(&m_loadDSPDoneState);

    m_activationMachine.setInitialState(&m_dspserverConnectState);

    connect(&m_dspserverConnectState, &QAbstractState::entered, this, &cRmsModuleMeasProgram::dspserverConnect);
    connect(&m_var2DSPState, &QAbstractState::entered, this, &cRmsModuleMeasProgram::varList2DSP);
    connect(&m_cmd2DSPState, &QAbstractState::entered, this, &cRmsModuleMeasProgram::cmdList2DSP);
    connect(&m_activateDSPState, &QAbstractState::entered, this, &cRmsModuleMeasProgram::activateDSP);
    connect(&m_loadDSPDoneState, &QAbstractState::entered, this, &cRmsModuleMeasProgram::activateDSPdone);

    m_deactivationMachine.addState(&m_unloadDSPDoneState);

    m_deactivationMachine.setInitialState(&m_unloadDSPDoneState);

    connect(&m_unloadDSPDoneState, &QAbstractState::entered, this, &cRmsModuleMeasProgram::deactivateDSPdone);

    // setting up statemachine for data acquisition
    m_dataAcquisitionState.addTransition(this,&cRmsModuleMeasProgram::dataAquisitionContinue, &m_dataAcquisitionDoneState);
    m_dataAcquisitionMachine.addState(&m_dataAcquisitionState);
    m_dataAcquisitionMachine.addState(&m_dataAcquisitionDoneState);
    m_dataAcquisitionMachine.setInitialState(&m_dataAcquisitionState);
    connect(&m_dataAcquisitionState, &QAbstractState::entered, this, &cRmsModuleMeasProgram::dataAcquisitionDSP);
    connect(&m_dataAcquisitionDoneState, &QAbstractState::entered, this, &cRmsModuleMeasProgram::dataReadDSP);

    connect(this, &cRmsModuleMeasProgram::actualValues,
            &m_startStopHandler, &ActualValueStartStopHandler::onNewActualValues);
    if (getConfData()->m_bmovingWindow) {
        m_movingwindowFilter.setIntegrationtime(getConfData()->m_fMeasIntervalTime.m_fValue);
        connect(&m_startStopHandler, &ActualValueStartStopHandler::sigNewActualValues,
                &m_movingwindowFilter, &cMovingwindowFilter::receiveActualValues);
        connect(&m_movingwindowFilter, &cMovingwindowFilter::actualValues,
                this, &cRmsModuleMeasProgram::setInterfaceActualValues);
    }
    else
        connect(&m_startStopHandler, &ActualValueStartStopHandler::sigNewActualValues,
                this, &cRmsModuleMeasProgram::setInterfaceActualValues);
}

void cRmsModuleMeasProgram::start()
{
    m_startStopHandler.start();
}

void cRmsModuleMeasProgram::stop()
{
    m_startStopHandler.stop();
}

void cRmsModuleMeasProgram::generateVeinInterface()
{
    VfModuleComponent *pActvalue;
    int n,p;
    n = p = 0; //
    QString channelDescription;
    for (int i = 0; i < getConfData()->m_valueChannelList.count(); i++) {
        QStringList sl = getConfData()->m_valueChannelList.at(i).split('-');
        // we have 1 or 2 entries for each value
        if (sl.count() == 1) { // in this case we have phase,neutral value
            if(sl.contains("m0") || sl.contains("m1") || sl.contains("m2") || sl.contains("m6")) //voltage channels
                channelDescription = QString("Actual rms value phase/neutral");
            else //current channels
                channelDescription = QString("Actual rms value");
            pActvalue = new VfModuleComponent(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                             QString("ACT_RMSPN%1").arg(n+1),
                                             channelDescription);
            m_veinActValueList.append(pActvalue); // we add the component for our measurement
            m_pModule->m_veinComponentsWithMetaAndScpi.append(pActvalue); // and for the modules interface
            n++;
        }
        else {
            pActvalue = new VfModuleComponent(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                                QString("ACT_RMSPP%1").arg(p+1),
                                                QString("Actual rms value phase/phase"));
            m_veinActValueList.append(pActvalue); // we add the component for our measurement
            m_pModule->m_veinComponentsWithMetaAndScpi.append(pActvalue); // and for the modules interface
            p++;
        }
    }

    m_pRMSPNCountInfo = new VfModuleMetaData(QString("RMSPNCount"), QVariant(n));
    m_pModule->veinModuleMetaDataList.append(m_pRMSPNCountInfo);
    m_pRMSPPCountInfo = new VfModuleMetaData(QString("RMSPPCount"), QVariant(p));
    m_pModule->veinModuleMetaDataList.append(m_pRMSPPCountInfo);

    QVariant val;
    QString intervalDescription, unit;

    bool timeIntegration = (getConfData()->m_sIntegrationMode == "time");
    if (timeIntegration) {
        val = QVariant(getConfData()->m_fMeasIntervalTime.m_fValue);
        intervalDescription = QString("Integration time");
        unit = QString("s");
    }
    else {
        val = QVariant(getConfData()->m_nMeasIntervalPeriod.m_nValue);
        intervalDescription = QString("Integration period");
        unit = QString("period");
    }
    QString key;
    m_pIntegrationParameter = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                                           key = QString("PAR_Interval"),
                                                           intervalDescription,
                                                           val);
    m_pIntegrationParameter->setUnit(unit);
    if (timeIntegration) {
        m_pIntegrationParameter->setScpiInfo("CONFIGURATION","TINTEGRATION", SCPI::isQuery|SCPI::isCmdwP);
        m_pIntegrationParameter->setValidator(new cDoubleValidator(1.0, 100.0, 0.5));
    }
    else {
        m_pIntegrationParameter->setScpiInfo("CONFIGURATION","TPERIOD", SCPI::isQuery|SCPI::isCmdwP);
        m_pIntegrationParameter->setValidator(new cIntValidator(5, 5000, 1));
    }
    m_pModule->m_veinModuleParameterMap[key] = m_pIntegrationParameter; // for modules use

    m_pMeasureSignal = new VfModuleComponent(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                                QString("SIG_Measuring"),
                                                QString("Signal indicating measurement activity"),
                                                QVariant(0));
    m_pModule->m_veinComponentsWithMetaAndScpi.append(m_pMeasureSignal);
}

void cRmsModuleMeasProgram::setDspVarList()
{
    int samples = m_pModule->getSharedChannelRangeObserver()->getSamplesPerPeriod();
    // we fetch a handle for sampled data and other temporary values
    m_pTmpDataDsp = m_dspInterface->getMemHandle("TmpData");
    m_pTmpDataDsp->addDspVar("MEASSIGNAL", samples, DSPDATA::vDspTemp);
    m_pTmpDataDsp->addDspVar("VALXRMS",m_veinActValueList.count(), DSPDATA::vDspTemp);
    m_pTmpDataDsp->addDspVar("FILTER",2*m_veinActValueList.count(),DSPDATA::vDspTemp);
    m_pTmpDataDsp->addDspVar("N",1,DSPDATA::vDspTemp);

    // a handle for parameter
    m_pParameterDSP =  m_dspInterface->getMemHandle("Parameter");
    m_pParameterDSP->addDspVar("TIPAR",1, DSPDATA::vDspParam, DSPDATA::dInt); // integrationtime res = 1ms
    // we use tistart as parameter, so we can finish actual measuring interval bei setting 0
    m_pParameterDSP->addDspVar("TISTART",1, DSPDATA::vDspParam, DSPDATA::dInt);

    // and one for filtered actual values
    m_pActualValuesDSP = m_dspInterface->getMemHandle("ActualValues");
    m_pActualValuesDSP->addDspVar("VALXRMSF",m_veinActValueList.count(), DSPDATA::vDspResult);

    m_ModuleActualValues.resize(m_pActualValuesDSP->getSize()); // we provide a vector for generated actual values
    m_nDspMemUsed = m_pTmpDataDsp->getSize() + m_pParameterDSP->getSize() + m_pActualValuesDSP->getSize();
}

void cRmsModuleMeasProgram::setDspCmdList()
{
    ChannelRangeObserver::SystemObserverPtr observer = m_pModule->getSharedChannelRangeObserver();
    int samples = observer->getSamplesPerPeriod();
    m_dspInterface->addCycListItem("STARTCHAIN(1,1,0x0101)"); // aktiv, prozessnr. (dummy),hauptkette 1 subkette 1 start
        m_dspInterface->addCycListItem(QString("CLEARN(%1,MEASSIGNAL)").arg(samples) ); // clear meassignal
        m_dspInterface->addCycListItem(QString("CLEARN(%1,FILTER)").arg(2*m_veinActValueList.count()+1) ); // clear the whole filter incl. count
        if (getConfData()->m_sIntegrationMode == "time") {
            if (getConfData()->m_bmovingWindow)
                m_dspInterface->addCycListItem(QString("SETVAL(TIPAR,%1)").arg(getConfData()->m_fmovingwindowInterval*1000.0)); // initial ti time
            else
                m_dspInterface->addCycListItem(QString("SETVAL(TIPAR,%1)").arg(getConfData()->m_fMeasIntervalTime.m_fValue*1000.0)); // initial ti time

            m_dspInterface->addCycListItem("GETSTIME(TISTART)"); // einmal ti start setzen
        }
        else
        {
            m_dspInterface->addCycListItem(QString("SETVAL(TIPAR,%1)").arg(getConfData()->m_nMeasIntervalPeriod.m_nValue)); // initial ti time
        }
        m_dspInterface->addCycListItem("DEACTIVATECHAIN(1,0x0101)"); // ende prozessnr., hauptkette 1 subkette 1
    m_dspInterface->addCycListItem("STOPCHAIN(1,0x0101)"); // ende prozessnr., hauptkette 1 subkette 1

    // we compute or copy our wanted actual values
    for (int i = 0; i < getConfData()->m_valueChannelList.count(); i++) {
        QStringList channelMNameList = getConfData()->m_valueChannelList.at(i).split('-');
        // we have 1 or 2 entries for each value
        if (channelMNameList.count() == 1) {
            int dspChannel = observer->getChannel(channelMNameList[0])->getDspChannel();
            m_dspInterface->addCycListItem(QString("COPYDATA(CH%1,0,MEASSIGNAL)").arg(dspChannel));
        }
        else {
            int dspChannel0 = observer->getChannel(channelMNameList[0])->getDspChannel();
            int dspChannel1 = observer->getChannel(channelMNameList[1])->getDspChannel();
            m_dspInterface->addCycListItem(QString("COPYDIFF(CH%1,CH%2,MEASSIGNAL)").arg(dspChannel0).arg(dspChannel1));
        }
        m_dspInterface->addCycListItem(QString("MULCCV(MEASSIGNAL,MEASSIGNAL,VALXRMS+%1)").arg(i));
        //m_dspInterface->addCycListItem(QString("RMS(MEASSIGNAL,VALXRMS+%1)").arg(i));
    }

    // and filter them
    m_dspInterface->addCycListItem(QString("AVERAGE1(%1,VALXRMS,FILTER)").arg(m_veinActValueList.count())); // we add results to filter

    if (getConfData()->m_sIntegrationMode == "time")
    {
        // in case intergration mode is time
        m_dspInterface->addCycListItem("TESTTIMESKIPNEX(TISTART,TIPAR)");
        m_dspInterface->addCycListItem("ACTIVATECHAIN(1,0x0102)");

        m_dspInterface->addCycListItem("STARTCHAIN(0,1,0x0102)");
            m_dspInterface->addCycListItem("GETSTIME(TISTART)"); // set new system time
            m_dspInterface->addCycListItem(QString("CMPAVERAGE1(%1,FILTER,VALXRMSF)").arg(m_veinActValueList.count()));

            for (int i = 0; i < m_veinActValueList.count(); i++)
                m_dspInterface->addCycListItem(QString("SQRT(VALXRMSF+%1,VALXRMSF+%2)").arg(i).arg(i));

            m_dspInterface->addCycListItem(QString("CLEARN(%1,FILTER)").arg(2*m_veinActValueList.count()+1) );
            m_dspInterface->addCycListItem(QString("DSPINTTRIGGER(0x0,0x%1)").arg(0 /* dummy */)); // send interrupt to module
            m_dspInterface->addCycListItem("DEACTIVATECHAIN(1,0x0102)");
        m_dspInterface->addCycListItem("STOPCHAIN(1,0x0102)"); // end processnr., mainchain 1 subchain 2
    }
    else
    {
        // otherwise it is period
        m_dspInterface->addCycListItem("TESTVVSKIPLT(N,TIPAR)");
        m_dspInterface->addCycListItem("ACTIVATECHAIN(1,0x0103)");
        m_dspInterface->addCycListItem("STARTCHAIN(0,1,0x0103)");
            m_dspInterface->addCycListItem(QString("CMPAVERAGE1(%1,FILTER,VALXRMSF)").arg(m_veinActValueList.count()));

            for (int i = 0; i < m_veinActValueList.count(); i++)
                m_dspInterface->addCycListItem(QString("SQRT(VALXRMSF+%1,VALXRMSF+%2)").arg(i).arg(i));

            m_dspInterface->addCycListItem(QString("CLEARN(%1,FILTER)").arg(2*m_veinActValueList.count()+1) );
            m_dspInterface->addCycListItem(QString("DSPINTTRIGGER(0x0,0x%1)").arg(0 /* dummy */)); // send interrupt to module
            m_dspInterface->addCycListItem("DEACTIVATECHAIN(1,0x0103)");
        m_dspInterface->addCycListItem("STOPCHAIN(1,0x0103)"); // end processnr., mainchain 1 subchain 2
    }
}

void cRmsModuleMeasProgram::catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer)
{
    if (msgnr == 0) { // 0 was reserved for async. messages
        // we got an interrupt from our cmd chain and have to fetch our actual values
        // but we synchronize on ranging process
        if (m_bActive && !m_dataAcquisitionMachine.isRunning()) // in case of deactivation in progress, no dataaquisition
            m_dataAcquisitionMachine.start();
    }
    else
    {
        // maybe other objexts share the same dsp interface
        if (m_MsgNrCmdList.contains(msgnr))
        {
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

cRmsModuleConfigData *cRmsModuleMeasProgram::getConfData()
{
    return qobject_cast<cRmsModuleConfiguration*>(m_pConfiguration.get())->getConfigurationData();
}

void cRmsModuleMeasProgram::setActualValuesNames()
{
    ChannelRangeObserver::SystemObserverPtr observer = m_pModule->getSharedChannelRangeObserver();
    const QStringList &channelList = getConfData()->m_valueChannelList;
    for(int i = 0; i < channelList.count(); i++) {
        const QString &channelMNamesEntry = getConfData()->m_valueChannelList.at(i);
        ServiceChannelNameHelper::TChannelAliasUnit aliasUnit =
            ServiceChannelNameHelper::getChannelAndUnit(channelMNamesEntry, observer);
        m_veinActValueList.at(i)->setChannelName(aliasUnit.m_channelAlias);
        m_veinActValueList.at(i)->setUnit(aliasUnit.m_channelUnit);
    }
}

void cRmsModuleMeasProgram::setSCPIMeasInfo()
{
    for (int i = 0; i < getConfData()->m_valueChannelList.count(); i++)
        m_veinActValueList.at(i)->setScpiInfo("MEASURE", m_veinActValueList.at(i)->getChannelName(), SCPI::isCmdwP);
}

void cRmsModuleMeasProgram::setInterfaceActualValues(QVector<float> *actualValues)
{
    if (m_bActive) // maybe we are deactivating !!!!
    {
        for (int i = 0; i < m_veinActValueList.count(); i++)
            m_veinActValueList.at(i)->setValue(QVariant((*actualValues)[i])); // and set entities
    }
}

void cRmsModuleMeasProgram::dspserverConnect()
{
    m_dspClient = Zera::Proxy::getInstance()->getConnectionSmart(m_pModule->getNetworkConfig()->m_dspServiceConnectionInfo,
                                                                 m_pModule->getNetworkConfig()->m_tcpNetworkFactory);
    m_dspInterface->setClientSmart(m_dspClient);
    m_dspserverConnectState.addTransition(m_dspClient.get(), &Zera::ProxyClient::connected, &m_var2DSPState);
    connect(m_dspInterface.get(), &AbstractServerInterface::serverAnswer, this, &cRmsModuleMeasProgram::catchInterfaceAnswer);
    Zera::Proxy::getInstance()->startConnectionSmart(m_dspClient);
}

void cRmsModuleMeasProgram::varList2DSP()
{
    setDspVarList(); // first we set the var list for our dsp
    setDspCmdList(); // and the cmd list he has to work on
    m_MsgNrCmdList[m_dspInterface->varList2Dsp()] = varlist2dsp;
}

void cRmsModuleMeasProgram::cmdList2DSP()
{
    m_MsgNrCmdList[m_dspInterface->cmdList2Dsp()] = cmdlist2dsp;
}

void cRmsModuleMeasProgram::activateDSP()
{
    m_MsgNrCmdList[m_dspInterface->activateInterface()] = activatedsp; // aktiviert die var- und cmd-listen im dsp
}

void cRmsModuleMeasProgram::activateDSPdone()
{
    m_bActive = true;

    setActualValuesNames();
    setSCPIMeasInfo();

    m_pMeasureSignal->setValue(QVariant(1));
    if (getConfData()->m_sIntegrationMode == "time")
        connect(m_pIntegrationParameter, &VfModuleComponent::sigValueChanged, this, &cRmsModuleMeasProgram::newIntegrationtime);
    else
        connect(m_pIntegrationParameter, &VfModuleComponent::sigValueChanged, this, &cRmsModuleMeasProgram::newIntegrationPeriod);

    emit activated();
}

void cRmsModuleMeasProgram::deactivateDSPdone()
{
    m_bActive = false;
    m_dataAcquisitionMachine.stop();
    disconnect(m_dspInterface.get(), 0, this, 0);
    emit deactivated();
}

void cRmsModuleMeasProgram::dataAcquisitionDSP()
{
    m_pMeasureSignal->setValue(QVariant(0));
    if (m_bActive)
        m_MsgNrCmdList[m_dspInterface->dataAcquisition(m_pActualValuesDSP)] = dataaquistion; // we start our data aquisition now
}

void cRmsModuleMeasProgram::dataReadDSP()
{
    if (m_bActive) {
        m_ModuleActualValues = m_pActualValuesDSP->getData();
        emit actualValues(&m_ModuleActualValues); // and send them
        m_pMeasureSignal->setValue(QVariant(1)); // signal measuring
    }
}

void cRmsModuleMeasProgram::newIntegrationtime(QVariant ti)
{
    bool ok;
    getConfData()->m_fMeasIntervalTime.m_fValue = ti.toDouble(&ok);
    if (getConfData()->m_sIntegrationMode == "time") {
        if (getConfData()->m_bmovingWindow)
            m_movingwindowFilter.setIntegrationtime(getConfData()->m_fMeasIntervalTime.m_fValue);
        else {
            m_pParameterDSP->setVarData(QString("TIPAR:%1;TISTART:0;").arg(getConfData()->m_fMeasIntervalTime.m_fValue*1000));
            m_MsgNrCmdList[m_dspInterface->dspMemoryWrite(m_pParameterDSP)] = writeparameter;
        }
        emit m_pModule->parameterChanged();
    }  
}

void cRmsModuleMeasProgram::newIntegrationPeriod(QVariant period)
{
    bool ok;
    getConfData()->m_nMeasIntervalPeriod.m_nValue = period.toInt(&ok);
    if (getConfData()->m_sIntegrationMode == "period") {
        m_pParameterDSP->setVarData(QString("TIPAR:%1;TISTART:0;").arg(getConfData()->m_nMeasIntervalPeriod.m_nValue));
        m_MsgNrCmdList[m_dspInterface->dspMemoryWrite(m_pParameterDSP)] = writeparameter;
    }
    emit m_pModule->parameterChanged();
}

}
