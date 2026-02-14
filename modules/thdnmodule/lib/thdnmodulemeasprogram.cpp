#include "thdnmodulemeasprogram.h"
#include "thdnmodule.h"
#include "thdnmoduleconfiguration.h"
#include "servicechannelnamehelper.h"
#include "thdnmoduleconfigdata.h"
#include "errormessages.h"
#include <reply.h>
#include <movingwindowfilter.h>
#include <proxy.h>
#include <proxyclient.h>
#include <scpi.h>
#include <vfmodulemetadata.h>
#include <vfmodulecomponent.h>
#include <vfmoduleparameter.h>
#include <doublevalidator.h>
#include <intvalidator.h>
#include <dspinterface.h>
#include <math.h>
#include <QString>
#include <QStateMachine>
#include <QJsonObject>
#include <QJsonArray>
#include <timerfactoryqt.h>

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
    connect(&m_unloadDSPDoneState, &QAbstractState::entered, this, &cThdnModuleMeasProgram::deactivateDSPdone);

    // setting up statemachine for data acquisition
    m_dataAcquisitionState.addTransition(this, &cThdnModuleMeasProgram::dataAquisitionContinue, &m_dataAcquisitionDoneState);
    m_dataAcquisitionMachine.addState(&m_dataAcquisitionState);
    m_dataAcquisitionMachine.addState(&m_dataAcquisitionDoneState);
    m_dataAcquisitionMachine.setInitialState(&m_dataAcquisitionState);
    connect(&m_dataAcquisitionState, &QAbstractState::entered, this, &cThdnModuleMeasProgram::dataAcquisitionDSP);
    connect(&m_dataAcquisitionDoneState, &QAbstractState::entered, this, &cThdnModuleMeasProgram::dataReadDSP);

    connect(this, &cThdnModuleMeasProgram::actualValues,
            &m_startStopHandler, &ActualValueStartStopHandler::onNewActualValues);
    if (getConfData()->m_bmovingWindow) {
        m_movingwindowFilter.setIntegrationtime(getConfData()->m_fMeasInterval.m_fValue);
        connect(&m_startStopHandler, &ActualValueStartStopHandler::sigNewActualValues,
                &m_movingwindowFilter, &cMovingwindowFilter::receiveActualValues);
        connect(&m_movingwindowFilter, &cMovingwindowFilter::actualValues,
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
    int n = m_pModule->getSharedChannelRangeObserver()->getChannelMNames().count();
    for (int i = 0; i < n; i++) {
        VfModuleComponent *pActvalue = new VfModuleComponent(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                            QString("ACT_THD%1%2").arg(getConfData()->m_sTHDType).arg(i+1),
                                            QString("THD%1 actual value").arg(getConfData()->m_sTHDType.toLower()));
        m_veinActValueList.append(pActvalue); // we add the component for our measurement
        m_pModule->m_veinComponentsWithMetaAndScpi.append(pActvalue); // and for the modules interface
    }

    m_pThdnCountInfo = new VfModuleMetaData(QString("THD%1Count").arg(getConfData()->m_sTHDType), QVariant(n));
    m_pModule->veinModuleMetaDataList.append(m_pThdnCountInfo);

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
    // we fetch a handle for sampled data and other temporary values
    m_pTmpDataDsp = m_dspInterface->getMemHandle("TmpData");
    m_pTmpDataDsp->addDspVar("MEASSIGNAL", samples, DSPDATA::vDspTemp);
    m_pTmpDataDsp->addDspVar("VALXTHDN",m_veinActValueList.count(), DSPDATA::vDspTemp);
    m_pTmpDataDsp->addDspVar("FILTER",2*m_veinActValueList.count(), DSPDATA::vDspTemp);
    m_pTmpDataDsp->addDspVar("N",1,DSPDATA::vDspTemp);

    // a handle for parameter
    m_pParameterDSP =  m_dspInterface->getMemHandle("Parameter");
    m_pParameterDSP->addDspVar("TIPAR",1, DSPDATA::vDspParam, DSPDATA::dInt); // integrationtime res = 1ms
    // we use tistart as parameter, so we can finish actual measuring interval bei setting 0
    m_pParameterDSP->addDspVar("TISTART",1, DSPDATA::vDspParam, DSPDATA::dInt);

    // and one for filtered actual values
    m_pActualValuesDSP = m_dspInterface->getMemHandle("ActualValues");
    m_pActualValuesDSP->addDspVar("VALXTHDNF",m_veinActValueList.count(), DSPDATA::vDspResult);

    m_ModuleActualValues.resize(m_pActualValuesDSP->getSize()); // we provide a vector for generated actual values
    m_nDspMemUsed = m_pTmpDataDsp->getSize() + m_pParameterDSP->getSize() + m_pActualValuesDSP->getSize();
}

void cThdnModuleMeasProgram::setDspCmdList()
{
    ChannelRangeObserver::SystemObserverPtr observer = m_pModule->getSharedChannelRangeObserver();
    int samples = observer->getSamplesPerPeriod();
    m_dspInterface->addCycListItem("STARTCHAIN(1,1,0x0101)"); // aktiv, prozessnr. (dummy),hauptkette 1 subkette 1 start
        m_dspInterface->addCycListItem(QString("CLEARN(%1,MEASSIGNAL)").arg(samples) ); // clear meassignal
        m_dspInterface->addCycListItem(QString("CLEARN(%1,FILTER)").arg(2*m_veinActValueList.count()+1) ); // clear the whole filter incl. count

        if (getConfData()->m_bmovingWindow)
            m_dspInterface->addCycListItem(QString("SETVAL(TIPAR,%1)").arg(getConfData()->m_fmovingwindowInterval*1000.0)); // initial ti time
        else
            m_dspInterface->addCycListItem(QString("SETVAL(TIPAR,%1)").arg(getConfData()->m_fMeasInterval.m_fValue*1000.0)); // initial ti time

        m_dspInterface->addCycListItem("GETSTIME(TISTART)"); // einmal ti start setzen
        m_dspInterface->addCycListItem("DEACTIVATECHAIN(1,0x0101)"); // ende prozessnr., hauptkette 1 subkette 1
    m_dspInterface->addCycListItem("STOPCHAIN(1,0x0101)"); // ende prozessnr., hauptkette 1 subkette 1

    // we compute or copy our wanted actual values
    for (int i = 0; i < m_veinActValueList.count(); i++) {
        QString channelMName = m_pModule->getSharedChannelRangeObserver()->getChannelMNames()[i];
        int dspChannel = observer->getChannel(channelMName)->getDspChannel();
        m_dspInterface->addCycListItem(QString("COPYDATA(CH%1,0,MEASSIGNAL)").arg(dspChannel));
        m_dspInterface->addCycListItem(QString("THDN(MEASSIGNAL,VALXTHDN+%1)").arg(i));
    }

    // and filter them
    m_dspInterface->addCycListItem(QString("AVERAGE1(%1,VALXTHDN,FILTER)").arg(m_veinActValueList.count())); // we add results to filter

    m_dspInterface->addCycListItem("TESTTIMESKIPNEX(TISTART,TIPAR)");
    m_dspInterface->addCycListItem("ACTIVATECHAIN(1,0x0102)");

    m_dspInterface->addCycListItem("STARTCHAIN(0,1,0x0102)");
        m_dspInterface->addCycListItem("GETSTIME(TISTART)"); // set new system time
        m_dspInterface->addCycListItem(QString("CMPAVERAGE1(%1,FILTER,VALXTHDNF)").arg(m_veinActValueList.count()));
        m_dspInterface->addCycListItem(QString("CLEARN(%1,FILTER)").arg(2*m_veinActValueList.count()+1) );
        m_dspInterface->addCycListItem(QString("DSPINTTRIGGER(0x0,0x%1)").arg(irqNr)); // send interrupt to module
        m_dspInterface->addCycListItem("DEACTIVATECHAIN(1,0x0102)");
    m_dspInterface->addCycListItem("STOPCHAIN(1,0x0102)"); // end processnr., mainchain 1 subchain 2
}

void cThdnModuleMeasProgram::catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer)
{
    if (msgnr == 0) { // 0 was reserved for async. messages
        QString sintnr;
        sintnr = answer.toString().section(':', 1, 1);
        int service = sintnr.toInt();
        switch (service)
        {
        case irqNr:
            // we got an interrupt from our cmd chain and have to fetch our actual values
            // but we synchronize on ranging process
            if (m_bActive && !m_dataAcquisitionMachine.isRunning()) // in case of deactivation in progress, no dataaquisition
                m_dataAcquisitionMachine.start();
            break;
        }
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

cThdnModuleConfigData *cThdnModuleMeasProgram::getConfData()
{
    return qobject_cast<cThdnModuleConfiguration*>(m_pConfiguration.get())->getConfigurationData();
}

void cThdnModuleMeasProgram::setActualValuesNames()
{
    ChannelRangeObserver::SystemObserverPtr observer = m_pModule->getSharedChannelRangeObserver();
    const QStringList channelMNames = m_pModule->getSharedChannelRangeObserver()->getChannelMNames();
    for (int i=0; i<channelMNames.count(); i++) {
        const QString channelMName = channelMNames[i];
        ServiceChannelNameHelper::TChannelAliasUnit aliasUnit =
            ServiceChannelNameHelper::getChannelAndUnit(channelMName, observer);
        m_veinActValueList.at(i)->setChannelName(aliasUnit.m_channelAlias);
        m_veinActValueList.at(i)->setUnit("%"); // !!!
    }
}

void cThdnModuleMeasProgram::setSCPIMeasInfo()
{
    for (int i = 0; i < m_veinActValueList.count(); i++)
        m_veinActValueList.at(i)->setScpiInfo("MEASURE", m_veinActValueList.at(i)->getChannelName(), SCPI::isCmdwP);
}

void cThdnModuleMeasProgram::setInterfaceActualValues(QVector<float> *actualValues)
{
    if (m_bActive) // maybe we are deactivating !!!!
    {
        for (int i = 0; i < m_veinActValueList.count(); i++)
            m_veinActValueList.at(i)->setValue(QVariant((double)actualValues->at(i))); // and set entities
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

    setActualValuesNames();
    setSCPIMeasInfo();

    m_pMeasureSignal->setValue(QVariant(1));
    connect(m_pIntegrationTimeParameter, &VfModuleComponent::sigValueChanged, this, &cThdnModuleMeasProgram::newIntegrationtime);

    emit activated();
}

void cThdnModuleMeasProgram::deactivateDSPStart()
{
    m_bActive = false;
    m_dataAcquisitionMachine.stop();
    disconnect(m_dspInterface.get(), 0, this, 0);
    emit deactivationContinue();
}

void cThdnModuleMeasProgram::deactivateDSPdone()
{
    emit deactivated();
}

void cThdnModuleMeasProgram::dataAcquisitionDSP()
{
    m_pMeasureSignal->setValue(QVariant(0));
    if(m_bActive)
        m_MsgNrCmdList[m_dspInterface->dataAcquisition(m_pActualValuesDSP)] = dataaquistion; // we start our data aquisition now
}

void cThdnModuleMeasProgram::dataReadDSP()
{
    if (m_bActive) {
        m_ModuleActualValues = m_pActualValuesDSP->getData();
        if (getConfData()->m_sTHDType == "R") {
            double thdn, thdr;
            for (int i = 0; i < m_ModuleActualValues.length(); i++) {
                thdn = m_ModuleActualValues.at(i) / 100.0;
                thdr = 100.0 * thdn / sqrt(1 + (thdn * thdn));
                m_ModuleActualValues.replace(i, thdr);
            }
        }
        emit actualValues(&m_ModuleActualValues); // and send them
        m_pMeasureSignal->setValue(QVariant(1)); // signal measuring
    }
}

void cThdnModuleMeasProgram::newIntegrationtime(QVariant ti)
{
    getConfData()->m_fMeasInterval.m_fValue = ti.toDouble();
    if (getConfData()->m_bmovingWindow)
        m_movingwindowFilter.setIntegrationtime(getConfData()->m_fMeasInterval.m_fValue);
    else {
        m_pParameterDSP->setVarData(QString("TIPAR:%1;TISTART:0;").arg(getConfData()->m_fMeasInterval.m_fValue*1000));
        m_MsgNrCmdList[m_dspInterface->dspMemoryWrite(m_pParameterDSP)] = writeparameter;
    }
    emit m_pModule->parameterChanged();
}

}
