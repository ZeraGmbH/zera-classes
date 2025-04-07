#include "fftmodulemeasprogram.h"
#include "fftmoduleconfiguration.h"
#include "servicechannelnamehelper.h"
#include <errormessages.h>
#include <reply.h>
#include <proxy.h>
#include <doublevalidator.h>
#include <scpi.h>
#include <stringvalidator.h>
#include <math.h>
#include <timerfactoryqt.h>

namespace FFTMODULE
{

cFftModuleMeasProgram::cFftModuleMeasProgram(cFftModule* module, std::shared_ptr<BaseModuleConfiguration> pConfiguration) :
    cBaseDspMeasProgram(pConfiguration, module->getVeinModuleName()),
    m_pModule(module)
{
    m_dspInterface = m_pModule->getServiceInterfaceFactory()->createDspInterfaceFft(
        m_pModule->getEntityId(),
        getConfData()->m_valueChannelList,
        getConfData()->m_nFftOrder);

    m_IdentifyState.addTransition(this, &cFftModuleMeasProgram::activationContinue, &m_pcbserverConnectState);
    m_pcbserverConnectState.addTransition(this, &cFftModuleMeasProgram::activationContinue, &m_dspserverConnectState);
    m_claimPGRMemState.addTransition(this, &cFftModuleMeasProgram::activationContinue, &m_claimUSERMemState);
    m_claimUSERMemState.addTransition(this, &cFftModuleMeasProgram::activationContinue, &m_var2DSPState);
    m_var2DSPState.addTransition(this, &cFftModuleMeasProgram::activationContinue, &m_cmd2DSPState);
    m_cmd2DSPState.addTransition(this, &cFftModuleMeasProgram::activationContinue, &m_activateDSPState);
    m_activateDSPState.addTransition(this, &cFftModuleMeasProgram::activationContinue, &m_loadDSPDoneState);

    m_activationMachine.addState(&m_resourceManagerConnectState);
    m_activationMachine.addState(&m_IdentifyState);
    m_activationMachine.addState(&m_pcbserverConnectState);
    m_activationMachine.addState(&m_dspserverConnectState);
    m_activationMachine.addState(&m_claimPGRMemState);
    m_activationMachine.addState(&m_claimUSERMemState);
    m_activationMachine.addState(&m_var2DSPState);
    m_activationMachine.addState(&m_cmd2DSPState);
    m_activationMachine.addState(&m_activateDSPState);
    m_activationMachine.addState(&m_loadDSPDoneState);

    m_activationMachine.setInitialState(&m_resourceManagerConnectState);

    connect(&m_resourceManagerConnectState, &QState::entered, this, &cFftModuleMeasProgram::resourceManagerConnect);
    connect(&m_IdentifyState, &QState::entered, this, &cFftModuleMeasProgram::sendRMIdent);
    connect(&m_pcbserverConnectState, &QState::entered, this, &cFftModuleMeasProgram::pcbserverConnect);
    connect(&m_dspserverConnectState, &QState::entered, this, &cFftModuleMeasProgram::dspserverConnect);
    connect(&m_claimPGRMemState, &QState::entered, this, &cFftModuleMeasProgram::claimPGRMem);
    connect(&m_claimUSERMemState, &QState::entered, this, &cFftModuleMeasProgram::claimUSERMem);
    connect(&m_var2DSPState, &QState::entered, this, &cFftModuleMeasProgram::varList2DSP);
    connect(&m_cmd2DSPState, &QState::entered, this, &cFftModuleMeasProgram::cmdList2DSP);
    connect(&m_activateDSPState, &QState::entered, this, &cFftModuleMeasProgram::activateDSP);
    connect(&m_loadDSPDoneState, &QState::entered, this, &cFftModuleMeasProgram::activateDSPdone);

    // setting up statemachine for unloading dsp and setting resources free
    m_deactivateDSPState.addTransition(this, &cFftModuleMeasProgram::deactivationContinue, &m_freePGRMemState);
    m_freePGRMemState.addTransition(this, &cFftModuleMeasProgram::deactivationContinue, &m_freeUSERMemState);
    m_freeUSERMemState.addTransition(this, &cFftModuleMeasProgram::deactivationContinue, &m_unloadDSPDoneState);
    m_deactivationMachine.addState(&m_deactivateDSPState);
    m_deactivationMachine.addState(&m_freePGRMemState);
    m_deactivationMachine.addState(&m_freeUSERMemState);
    m_deactivationMachine.addState(&m_unloadDSPDoneState);

    m_deactivationMachine.setInitialState(&m_deactivateDSPState);

    connect(&m_deactivateDSPState, &QState::entered, this, &cFftModuleMeasProgram::deactivateDSP);
    connect(&m_freePGRMemState, &QState::entered, this, &cFftModuleMeasProgram::freePGRMem);
    connect(&m_freeUSERMemState, &QState::entered, this, &cFftModuleMeasProgram::freeUSERMem);
    connect(&m_unloadDSPDoneState, &QState::entered, this, &cFftModuleMeasProgram::deactivateDSPdone);

    // setting up statemachine for data acquisition
    m_dataAcquisitionState.addTransition(this, &cFftModuleMeasProgram::dataAquisitionContinue, &m_dataAcquisitionDoneState);
    m_dataAcquisitionMachine.addState(&m_dataAcquisitionState);
    m_dataAcquisitionMachine.addState(&m_dataAcquisitionDoneState);
    m_dataAcquisitionMachine.setInitialState(&m_dataAcquisitionState);
    connect(&m_dataAcquisitionState, &QState::entered, this, &cFftModuleMeasProgram::dataAcquisitionDSP);
    connect(&m_dataAcquisitionDoneState, &QState::entered, this, &cFftModuleMeasProgram::dataReadDSP);

    connect(this, &cFftModuleMeasProgram::actualValues,
            &m_startStopHandler, &ActualValueStartStopHandler::onNewActualValues);
    if (getConfData()->m_bmovingWindow) {
        m_movingwindowFilter.setIntegrationtime(getConfData()->m_fMeasInterval.m_fValue);
        connect(&m_startStopHandler, &ActualValueStartStopHandler::sigNewActualValues,
                &m_movingwindowFilter, &cMovingwindowFilter::receiveActualValues);
        connect(&m_movingwindowFilter, &cMovingwindowFilter::actualValues,
                this, &cFftModuleMeasProgram::setInterfaceActualValues);
    }
    else
        connect(&m_startStopHandler, &ActualValueStartStopHandler::sigNewActualValues,
                this, &cFftModuleMeasProgram::setInterfaceActualValues);
}

void cFftModuleMeasProgram::start()
{
    m_startStopHandler.start();
}

void cFftModuleMeasProgram::stop()
{
    m_startStopHandler.stop();
}

void cFftModuleMeasProgram::generateVeinInterface()
{
    QString key;
    VfModuleActvalue *pActvalue;
    int n = getConfData()->m_valueChannelList.count();
    for (int i = 0; i < n; i++) {
        pActvalue = new VfModuleActvalue(m_pModule->getEntityId(), m_pModule->m_pModuleValidator,
                                            QString("ACT_FFT%1").arg(i+1),
                                            QString("FFT actual values"));
        m_veinActValueList.append(pActvalue); // we add the component for our measurement
        m_pModule->veinModuleActvalueList.append(pActvalue); // and for the modules interface

        pActvalue = new VfModuleActvalue(m_pModule->getEntityId(), m_pModule->m_pModuleValidator,
                                            QString("ACT_DC%1").arg(i+1),
                                            QString("DC actual value"));
        m_DCValueList.append(pActvalue);
        m_pModule->veinModuleActvalueList.append(pActvalue);
    }

    m_pFFTCountInfo = new VfModuleMetaData(QString("FFTCount"), QVariant(n));
    m_pModule->veinModuleMetaDataList.append(m_pFFTCountInfo);
    m_pFFTOrderInfo = new VfModuleMetaData(QString("FFTOrder"), QVariant(getConfData()->m_nFftOrder));
    m_pModule->veinModuleMetaDataList.append(m_pFFTOrderInfo);

    m_pIntegrationTimeParameter = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->m_pModuleValidator,
                                                           key = QString("PAR_Interval"),
                                                           QString("Integration time"),
                                                           QVariant(getConfData()->m_fMeasInterval.m_fValue));
    m_pIntegrationTimeParameter->setUnit("s");
    m_pIntegrationTimeParameter->setScpiInfo("CONFIGURATION","TINTEGRATION", SCPI::isQuery|SCPI::isCmdwP, "PAR_Interval", SCPI::isComponent);
    m_pIntegrationTimeParameter->setValidator(new cDoubleValidator(1.0, 100.0, 0.5));
    m_pModule->m_veinModuleParameterMap[key] = m_pIntegrationTimeParameter; // for modules use


    QString refChannelMNameConfigured = getConfData()->m_RefChannel.m_sPar;
    m_pRefChannelParameter = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->m_pModuleValidator,
                                                   key = QString("PAR_RefChannel"),
                                                   QString("Reference channel"),
                                                   refChannelMNameConfigured);

    cStringValidator *sValidator;
    sValidator = new cStringValidator(getConfData()->m_valueChannelList);
    m_pRefChannelParameter->setValidator(sValidator);

    m_pRefChannelParameter->setScpiInfo("CONFIGURATION","REFCHANNEL", SCPI::isQuery|SCPI::isCmdwP, "PAR_RefChannel", SCPI::isComponent);
    m_pModule->m_veinModuleParameterMap[key] = m_pRefChannelParameter; // for modules use

    m_pMeasureSignal = new VfModuleComponent(m_pModule->getEntityId(), m_pModule->m_pModuleValidator,
                                                QString("SIG_Measuring"),
                                                QString("Signal indicating measurement activity"),
                                                QVariant(0));

    m_pModule->veinModuleComponentList.append(m_pMeasureSignal);
}


quint16 FFTMODULE::cFftModuleMeasProgram::calcFftResultLenHalf(quint8 fftOrder)
{
    quint16 fftLen = 2 << (int)(floor(log((fftOrder * 2) -1) / log(2.0)));
    if (fftLen < 32) // minimum fftlen is 32 !!!!
        fftLen = fftLen * 2;
    return fftLen;
}

void cFftModuleMeasProgram::setDspVarList()
{
    int samples = m_pModule->getSharedChannelRangeObserver()->getSamplesPerPeriod();
    m_nfftLen = calcFftResultLenHalf(getConfData()->m_nFftOrder);
    // we fetch a handle for sampled data and other temporary values
    // global data segment is 1k words and lies on 1k boundary, so we put fftinput and fftouptut
    // at the beginning of that page because bitreversal adressing of fft only works properly if so
    m_pTmpDataDsp = m_dspInterface->getMemHandle("TmpData");
    m_pTmpDataDsp->addVarItem( new cDspVar("FFTINPUT", 2 * m_nfftLen, DSPDATA::vDspTempGlobal));
    m_pTmpDataDsp->addVarItem( new cDspVar("FFTOUTPUT", 2 * m_nfftLen, DSPDATA::vDspTempGlobal));
    // meassignal will also still fit in global mem ... so we save memory
    m_pTmpDataDsp->addVarItem( new cDspVar("MEASSIGNAL", 2 * samples, DSPDATA::vDspTemp));
    m_pTmpDataDsp->addVarItem( new cDspVar("FFTXOUTPUT", 2 * m_nfftLen * m_veinActValueList.count(), DSPDATA::vDspTemp));
    m_pTmpDataDsp->addVarItem( new cDspVar("FILTER", 2 * 2 * m_nfftLen * m_veinActValueList.count(),DSPDATA::vDspTemp));
    m_pTmpDataDsp->addVarItem( new cDspVar("N",1,DSPDATA::vDspTemp));
    m_pTmpDataDsp->addVarItem( new cDspVar("IPOLADR", 1, DSPDATA::vDspTemp, DSPDATA::dInt));
    m_pTmpDataDsp->addVarItem( new cDspVar("DFTREF", 2, DSPDATA::vDspTemp));
    m_pTmpDataDsp->addVarItem(new cDspVar("DEBUGCOUNT",1,DSPDATA::vDspTemp, DSPDATA::dInt));

    // a handle for parameter
    m_pParameterDSP =  m_dspInterface->getMemHandle("Parameter");
    m_pParameterDSP->addVarItem( new cDspVar("TIPAR",1, DSPDATA::vDspParam, DSPDATA::dInt)); // integrationtime res = 1ms
    // we use tistart as parameter, so we can finish actual measuring interval bei setting 0
    m_pParameterDSP->addVarItem( new cDspVar("TISTART",1, DSPDATA::vDspParam, DSPDATA::dInt));
    m_pParameterDSP->addVarItem( new cDspVar("REFCHN",1, DSPDATA::vDspParam, DSPDATA::dInt));

    // and one for filtered actual values
    m_pActualValuesDSP = m_dspInterface->getMemHandle("ActualValues");
    m_pActualValuesDSP->addVarItem( new cDspVar("VALXFFTF", 2 * m_nfftLen * m_veinActValueList.count(), DSPDATA::vDspResult));

    m_ModuleActualValues.resize(m_pActualValuesDSP->getSize()); // we provide a vector for generated actual values
    m_FFTModuleActualValues.resize(m_veinActValueList.count() * getConfData()->m_nFftOrder * 2);
    m_nDspMemUsed = m_pTmpDataDsp->getUserMemSize() + m_pParameterDSP->getSize() + m_pActualValuesDSP->getSize();
}


void cFftModuleMeasProgram::deleteDspVarList()
{
    m_dspInterface->deleteMemHandle(m_pTmpDataDsp);
    m_dspInterface->deleteMemHandle(m_pParameterDSP);
    m_dspInterface->deleteMemHandle(m_pActualValuesDSP);
}


void cFftModuleMeasProgram::setDspCmdList()
{
    ChannelRangeObserver::SystemObserverPtr observer = m_pModule->getSharedChannelRangeObserver();
    int samples = observer->getSamplesPerPeriod();
    QString referenceChannel = getConfData()->m_RefChannel.m_sPar;
    int referenceDspChannel = observer->getChannel(referenceChannel)->getDspChannel();
    m_dspInterface->addCycListItem("STARTCHAIN(1,1,0x0101)"); // aktiv, prozessnr. (dummy),hauptkette 1 subkette 1 start
        m_dspInterface->addCycListItem(QString("CLEARN(%1,MEASSIGNAL)").arg(2*samples) ); // clear meassignal
        m_dspInterface->addCycListItem(QString("CLEARN(%1,FILTER)").arg(2 * 2 * m_nfftLen * m_veinActValueList.count()+1) ); // clear the whole filter incl. count
        if (getConfData()->m_bmovingWindow)
            m_dspInterface->addCycListItem(QString("SETVAL(TIPAR,%1)").arg(getConfData()->m_fmovingwindowInterval*1000.0)); // initial ti time
        else
            m_dspInterface->addCycListItem(QString("SETVAL(TIPAR,%1)").arg(getConfData()->m_fMeasInterval.m_fValue*1000.0)); // initial ti time
        m_dspInterface->addCycListItem(QString("SETVAL(REFCHN,%1)").arg(referenceDspChannel));
        m_dspInterface->addCycListItem("GETSTIME(TISTART)"); // einmal ti start setzen
        m_dspInterface->addCycListItem("DEACTIVATECHAIN(1,0x0101)"); // ende prozessnr., hauptkette 1 subkette 1
        m_dspInterface->addCycListItem("SETVAL(DEBUGCOUNT,0)");
    m_dspInterface->addCycListItem("STOPCHAIN(1,0x0101)"); // ende prozessnr., hauptkette 1 subkette 1

    // we compute the phase of our reference channel first
    m_dspInterface->addCycListItem("COPYDATAIND(REFCHN,0,MEASSIGNAL)");
    m_dspInterface->addCycListItem("DFT(1,MEASSIGNAL,DFTREF)");
    m_dspInterface->addCycListItem("GENADR(MEASSIGNAL,DFTREF,IPOLADR)");


    // next 3 commands for debug purpose , will be removed later
    //m_dspInterface->addCycListItem("INC(DEBUGCOUNT)");
    //m_dspInterface->addCycListItem("TESTVCSKIPLT(DEBUGCOUNT,1000)");
    //m_dspInterface->addCycListItem("BREAK(1)");


    // we compute or copy our wanted actual values
    for (int i = 0; i < getConfData()->m_valueChannelList.count(); i++) {
        QString channelMName = getConfData()->m_valueChannelList[i];
        int dspChannel = observer->getChannel(channelMName)->getDspChannel();
        m_dspInterface->addCycListItem(QString("COPYDATA(CH%1,0,MEASSIGNAL)").arg(dspChannel) );
        m_dspInterface->addCycListItem(QString("COPYDATA(CH%1,0,MEASSIGNAL+%2)").arg(dspChannel).arg(samples));
        m_dspInterface->addCycListItem(QString("INTERPOLATIONIND(%1,IPOLADR,FFTINPUT)").arg(m_nfftLen));
        m_dspInterface->addCycListItem(QString("FFTREAL(%1,FFTINPUT,FFTOUTPUT)").arg(m_nfftLen));
        m_dspInterface->addCycListItem(QString("COPYMEM(%1,FFTOUTPUT,FFTXOUTPUT+%2)").arg(2 * m_nfftLen).arg(2 * m_nfftLen * i));
    }

    // and filter them
    m_dspInterface->addCycListItem(QString("AVERAGE1(%1,FFTXOUTPUT,FILTER)").arg(2 * m_nfftLen * m_veinActValueList.count())); // we add results to filter

    m_dspInterface->addCycListItem("TESTTIMESKIPNEX(TISTART,TIPAR)");
    m_dspInterface->addCycListItem("ACTIVATECHAIN(1,0x0102)");

    m_dspInterface->addCycListItem("STARTCHAIN(0,1,0x0102)");
        m_dspInterface->addCycListItem("GETSTIME(TISTART)"); // set new system time
        m_dspInterface->addCycListItem(QString("CMPAVERAGE1(%1,FILTER,VALXFFTF)").arg(2 * m_nfftLen * m_veinActValueList.count()));
        m_dspInterface->addCycListItem(QString("CLEARN(%1,FILTER)").arg(2 * 2 * m_nfftLen * m_veinActValueList.count()+1) );
        m_dspInterface->addCycListItem(QString("DSPINTTRIGGER(0x0,0x%1)").arg(0)); // send interrupt to module
        m_dspInterface->addCycListItem("DEACTIVATECHAIN(1,0x0102)");
    m_dspInterface->addCycListItem("STOPCHAIN(1,0x0102)"); // end processnr., mainchain 1 subchain 2
}


void cFftModuleMeasProgram::deleteDspCmdList()
{
    m_dspInterface->clearCmdList();
}


void cFftModuleMeasProgram::catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer)
{
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
            case sendrmident:
                if (reply == ack)
                    emit activationContinue();
                else
                    notifyError(rmidentErrMSG);
                break;
            case claimpgrmem:
                if (reply == ack)
                    emit activationContinue();
                else
                    notifyError(claimresourceErrMsg);
                break;
            case claimusermem:
                if (reply == ack)
                    emit activationContinue();
                else
                    notifyError(claimresourceErrMsg);
                break;
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
            case freepgrmem:
                if (reply == ack)
                    emit deactivationContinue();
                else
                    notifyError(freeresourceErrMsg);
                break;
            case freeusermem:
                if (reply == ack)
                    emit deactivationContinue();
                else
                    notifyError(freeresourceErrMsg);
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

cFftModuleConfigData *cFftModuleMeasProgram::getConfData()
{
    return qobject_cast<cFftModuleConfiguration*>(m_pConfiguration.get())->getConfigurationData();
}


void cFftModuleMeasProgram::setSCPIMeasInfo()
{
    for (int i = 0; i < getConfData()->m_valueChannelList.count(); i++) {
        m_veinActValueList.at(i)->setScpiInfo("MEASURE", m_veinActValueList.at(i)->getChannelName(), SCPI::isCmdwP, m_veinActValueList.at(i)->getName(), SCPI::isComponent);
        m_DCValueList.at(i)->setScpiInfo("MEASURE", m_DCValueList.at(i)->getChannelName() + "_DC", SCPI::isCmdwP, m_DCValueList.at(i)->getName(), SCPI::isComponent);
    }
}

void cFftModuleMeasProgram::setActualValuesNames()
{
    ChannelRangeObserver::SystemObserverPtr observer = m_pModule->getSharedChannelRangeObserver();
    const QStringList &channelList = getConfData()->m_valueChannelList;
    for(int i = 0; i < channelList.count(); i++) {
        const QString &channelMNamesEntry = getConfData()->m_valueChannelList.at(i);
        ServiceChannelNameHelper::TChannelAliasUnit aliasUnit =
            ServiceChannelNameHelper::getChannelAndUnit(channelMNamesEntry, observer);
        m_veinActValueList.at(i)->setChannelName(aliasUnit.m_channelAlias);
        m_veinActValueList.at(i)->setUnit(aliasUnit.m_channelUnit);
        m_DCValueList.at(i)->setChannelName(aliasUnit.m_channelAlias);
        m_DCValueList.at(i)->setUnit(aliasUnit.m_channelUnit);
    }
}

void cFftModuleMeasProgram::setInterfaceActualValues(QVector<float> *actualValues)
{
    if (m_bActive) { // maybe we are deactivating !!!!
        for (int channel = 0; channel < m_veinActValueList.count(); channel++) {
            int maxOrder = getConfData()->m_nFftOrder;
            int offs = channel * maxOrder * 2;
            QList<double> fftList;
            for (int order = 0; order < maxOrder; order++) {
                fftList.append(actualValues->at(offs + order*2));
                fftList.append(actualValues->at(offs + order*2 +1));
            }
            QVariant list;
            list = QVariant::fromValue<QList<double> >(fftList);
            m_veinActValueList.at(channel)->setValue(list);
            m_DCValueList.at(channel)->setValue(fftList[0]);
        }
    }
}

void cFftModuleMeasProgram::resourceManagerConnect()
{
    // as this is our entry point when activating the module, we do some initialization first
    m_measChannelInfoHash.clear(); // we build up a new channel info hash
    cMeasChannelInfo mi;
    for (int i = 0; i < m_veinActValueList.count(); i++)
    {
        QStringList sl = getConfData()->m_valueChannelList.at(i).split('-');
        for (int j = 0; j < sl.count(); j++)
        {
            QString s = sl.at(j);
            if (!m_measChannelInfoHash.contains(s))
                m_measChannelInfoHash[s] = mi;
        }
    }

    // we have to instantiate a working resource manager interface
    // so first we try to get a connection to resource manager over proxy
    m_rmClient = Zera::Proxy::getInstance()->getConnectionSmart(m_pModule->getNetworkConfig()->m_rmServiceConnectionInfo,
                                                                m_pModule->getNetworkConfig()->m_tcpNetworkFactory);
    m_resourceManagerConnectState.addTransition(m_rmClient.get(), &Zera::ProxyClient::connected, &m_IdentifyState);
    // todo insert timer for timeout and/or connect error conditions.....
    // and then we set resource manager interface's connection
    m_rmInterface.setClientSmart(m_rmClient); //
    connect(&m_rmInterface, &Zera::cRMInterface::serverAnswer, this, &cFftModuleMeasProgram::catchInterfaceAnswer);
    Zera::Proxy::getInstance()->startConnectionSmart(m_rmClient);
}


void cFftModuleMeasProgram::sendRMIdent()
{
    m_MsgNrCmdList[m_rmInterface.rmIdent(QString("FftModule%1").arg(m_pModule->getModuleNr()))] = sendrmident;
}

void cFftModuleMeasProgram::pcbserverConnect()
{
    m_pcbClient = Zera::Proxy::getInstance()->getConnectionSmart(m_pModule->getNetworkConfig()->m_pcbServiceConnectionInfo,
                                                                 m_pModule->getNetworkConfig()->m_tcpNetworkFactory);
    m_pcbInterface->setClientSmart(m_pcbClient);
    connect(m_pcbClient.get(), &Zera::ProxyClient::connected, this, &cBaseMeasProgram::activationContinue);
    connect(m_pcbInterface.get(), &AbstractServerInterface::serverAnswer, this, &cFftModuleMeasProgram::catchInterfaceAnswer);
    Zera::Proxy::getInstance()->startConnectionSmart(m_pcbClient);
}

void cFftModuleMeasProgram::dspserverConnect()
{
    m_dspClient = Zera::Proxy::getInstance()->getConnectionSmart(m_pModule->getNetworkConfig()->m_dspServiceConnectionInfo,
                                                                 m_pModule->getNetworkConfig()->m_tcpNetworkFactory);
    m_dspInterface->setClientSmart(m_dspClient);
    m_dspserverConnectState.addTransition(m_dspClient.get(), &Zera::ProxyClient::connected, &m_claimPGRMemState);
    connect(m_dspInterface.get(), &Zera::cDSPInterface::serverAnswer, this, &cFftModuleMeasProgram::catchInterfaceAnswer);
    Zera::Proxy::getInstance()->startConnectionSmart(m_dspClient);
}

void cFftModuleMeasProgram::claimPGRMem()
{
    // if we've got dsp server connection we set up our measure program and claim the resources
    setDspVarList(); // first we set the var list for our dsp
    setDspCmdList(); // and the cmd list he has to work on
    m_MsgNrCmdList[m_rmInterface.setResource("DSP1", "PGRMEMC", m_dspInterface->cmdListCount())] = claimpgrmem;
}

void cFftModuleMeasProgram::claimUSERMem()
{
   m_MsgNrCmdList[m_rmInterface.setResource("DSP1", "USERMEM", m_nDspMemUsed)] = claimusermem;
}

void cFftModuleMeasProgram::varList2DSP()
{
    m_MsgNrCmdList[m_dspInterface->varList2Dsp()] = varlist2dsp;
}

void cFftModuleMeasProgram::cmdList2DSP()
{
    m_MsgNrCmdList[m_dspInterface->cmdList2Dsp()] = cmdlist2dsp;
}

void cFftModuleMeasProgram::activateDSP()
{
    m_MsgNrCmdList[m_dspInterface->activateInterface()] = activatedsp; // aktiviert die var- und cmd-listen im dsp
}

void cFftModuleMeasProgram::activateDSPdone()
{
    m_bActive = true;

    setActualValuesNames();
    setSCPIMeasInfo();

    m_pMeasureSignal->setValue(QVariant(1));
    connect(m_pIntegrationTimeParameter, &VfModuleParameter::sigValueChanged, this, &cFftModuleMeasProgram::newIntegrationtime);
    connect(m_pRefChannelParameter, &VfModuleParameter::sigValueChanged, this, &cFftModuleMeasProgram::newRefChannel);
    emit activated();
}

void cFftModuleMeasProgram::deactivateDSP()
{
    m_bActive = false;
    m_MsgNrCmdList[m_dspInterface->deactivateInterface()] = deactivatedsp; // wat wohl
}

void cFftModuleMeasProgram::freePGRMem()
{
    Zera::Proxy::getInstance()->releaseConnection(m_dspClient.get());
    deleteDspVarList();
    deleteDspCmdList();

    m_MsgNrCmdList[m_rmInterface.freeResource("DSP1", "PGRMEMC")] = freepgrmem;
}

void cFftModuleMeasProgram::freeUSERMem()
{
    m_MsgNrCmdList[m_rmInterface.freeResource("DSP1", "USERMEM")] = freeusermem;
}

void cFftModuleMeasProgram::deactivateDSPdone()
{
    disconnect(&m_rmInterface, 0, this, 0);
    disconnect(m_dspInterface.get(), 0, this, 0);
    disconnect(m_pcbInterface.get(), 0, this, 0);
    emit deactivated();
}

void cFftModuleMeasProgram::dataAcquisitionDSP()
{
    m_pMeasureSignal->setValue(QVariant(0));
    m_MsgNrCmdList[m_dspInterface->dataAcquisition(m_pActualValuesDSP)] = dataaquistion; // we start our data aquisition now
}

void cFftModuleMeasProgram::dataReadDSP()
{
    if (m_bActive) {
        m_ModuleActualValues = m_pActualValuesDSP->getData();

        int nChannels = m_veinActValueList.count();
        int nHarmonic = getConfData()->m_nFftOrder;
        int resultOffs = nHarmonic * 2;

        double scale = 1.0/m_nfftLen;
        int middle = m_nfftLen; // the fft results are sym. ordered with pos. and neg. frequencies
        int sourceDataSize = 2*m_nfftLen;

        for (int channelNo = 0; channelNo < nChannels; channelNo++)
        {
            int sourceOffset = channelNo * sourceDataSize;

            // deduced per channel dsp data layout:
            //                                v- middle                   v- 2*middle
            // re0|         im-values         |         re-values         |
            //     123..                 ..321 123...                ..321
            m_FFTModuleActualValues.replace(channelNo * resultOffs, m_ModuleActualValues.at(sourceOffset) * scale); // special case dc
            m_FFTModuleActualValues.replace(channelNo * resultOffs + 1, 0.0);

            for (int harmonicNo = 1; harmonicNo < nHarmonic; harmonicNo++)
            {
                double re, im;
                // as our Fft produces math positive values, we correct them to technical positive values (*-1.0)
                // also we change real and imag. parts because we are interested in sine rather than cosine

                re = -scale * (m_ModuleActualValues.at(sourceOffset + middle + harmonicNo) - m_ModuleActualValues.at(sourceOffset + middle * 2 - harmonicNo) );
                im = -scale * (m_ModuleActualValues.at(sourceOffset + middle - harmonicNo) + m_ModuleActualValues.at(sourceOffset + harmonicNo));

                m_FFTModuleActualValues.replace(channelNo * resultOffs + (harmonicNo*2), re);
                m_FFTModuleActualValues.replace(channelNo * resultOffs + (harmonicNo*2) + 1, im);
            }
        }

        emit actualValues(&m_FFTModuleActualValues); // and send them
        m_pMeasureSignal->setValue(QVariant(1)); // signal measuring
    }
}

void cFftModuleMeasProgram::newIntegrationtime(QVariant ti)
{
    bool ok;
    getConfData()->m_fMeasInterval.m_fValue = ti.toDouble(&ok);

    if (getConfData()->m_bmovingWindow)
        m_movingwindowFilter.setIntegrationtime(getConfData()->m_fMeasInterval.m_fValue);
    else {
        m_pParameterDSP->setVarData(QString("TIPAR:%1;TISTART:0;").arg(getConfData()->m_fMeasInterval.m_fValue*1000));
        m_MsgNrCmdList[m_dspInterface->dspMemoryWrite(m_pParameterDSP)] = writeparameter;
    }

    emit m_pModule->parameterChanged();
}

void cFftModuleMeasProgram::newRefChannel(QVariant chn)
{
    QString channelMName = chn.toString();
    getConfData()->m_RefChannel.m_sPar = channelMName;
    ChannelRangeObserver::SystemObserverPtr observer = m_pModule->getSharedChannelRangeObserver();
    ChannelRangeObserver::ChannelPtr channel = observer->getChannel(channelMName);
    int dspChannel = channel->getDspChannel();
    m_pParameterDSP->setVarData(QString("REFCHN:%1;").arg(dspChannel));
    m_MsgNrCmdList[m_dspInterface->dspMemoryWrite(m_pParameterDSP)] = writeparameter;
    emit m_pModule->parameterChanged();
}
}
