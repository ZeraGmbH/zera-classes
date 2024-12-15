#include "oscimodulemeasprogram.h"
#include "oscimodule.h"
#include "oscimoduleconfiguration.h"
#include "servicechannelnamehelper.h"
#include <stringvalidator.h>
#include <intvalidator.h>
#include <errormessages.h>
#include <dspinterfacecmddecoder.h>
#include <reply.h>
#include <proxy.h>
#include <timerfactoryqt.h>
#include <cmath>

namespace OSCIMODULE
{

cOsciModuleMeasProgram::cOsciModuleMeasProgram(cOsciModule* module, std::shared_ptr<BaseModuleConfiguration> pConfiguration) :
    cBaseDspMeasProgram(pConfiguration, module->getVeinModuleName()),
    m_pModule(module)
{
    m_dspInterface = m_pModule->getServiceInterfaceFactory()->createDspInterfaceOsci(
        getConfData()->m_valueChannelList,
        getConfData()->m_nInterpolation);

    // As long as there are no tasks - ignore error
    m_channelRangeObserverScanState.addTransition(
        m_pModule->getSharedChannelRangeObserver().get(), &ChannelRangeObserver::SystemObserver::sigFullScanFinished,&m_resourceManagerConnectState);
    m_IdentifyState.addTransition(this, &cOsciModuleMeasProgram::activationContinue, &m_pcbserverConnectState);
    m_pcbserverConnectState.addTransition(this, &cOsciModuleMeasProgram::activationContinue, &m_dspserverConnectState);

    m_claimPGRMemState.addTransition(this, &cOsciModuleMeasProgram::activationContinue, &m_claimUSERMemState);
    m_claimUSERMemState.addTransition(this, &cOsciModuleMeasProgram::activationContinue, &m_var2DSPState);
    m_var2DSPState.addTransition(this, &cOsciModuleMeasProgram::activationContinue, &m_cmd2DSPState);
    m_cmd2DSPState.addTransition(this, &cOsciModuleMeasProgram::activationContinue, &m_activateDSPState);
    m_activateDSPState.addTransition(this, &cOsciModuleMeasProgram::activationContinue, &m_loadDSPDoneState);

    m_activationMachine.addState(&m_channelRangeObserverScanState);
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

    m_activationMachine.setInitialState(&m_channelRangeObserverScanState);

    connect(&m_channelRangeObserverScanState, &QState::entered, this, &cOsciModuleMeasProgram::startFetchCommonRanges);
    connect(&m_resourceManagerConnectState, &QState::entered, this, &cOsciModuleMeasProgram::resourceManagerConnect);
    connect(&m_IdentifyState, &QState::entered, this, &cOsciModuleMeasProgram::sendRMIdent);
    connect(&m_pcbserverConnectState, &QState::entered, this, &cOsciModuleMeasProgram::pcbserverConnect);
    connect(&m_dspserverConnectState, &QState::entered, this, &cOsciModuleMeasProgram::dspserverConnect);
    connect(&m_claimPGRMemState, &QState::entered, this, &cOsciModuleMeasProgram::claimPGRMem);
    connect(&m_claimUSERMemState, &QState::entered, this, &cOsciModuleMeasProgram::claimUSERMem);
    connect(&m_var2DSPState, &QState::entered, this, &cOsciModuleMeasProgram::varList2DSP);
    connect(&m_cmd2DSPState, &QState::entered, this, &cOsciModuleMeasProgram::cmdList2DSP);
    connect(&m_activateDSPState, &QState::entered, this, &cOsciModuleMeasProgram::activateDSP);
    connect(&m_loadDSPDoneState, &QState::entered, this, &cOsciModuleMeasProgram::activateDSPdone);

    // setting up statemachine for unloading dsp and setting resources free
    m_deactivateDSPState.addTransition(this, &cOsciModuleMeasProgram::deactivationContinue, &m_freePGRMemState);
    m_freePGRMemState.addTransition(this, &cOsciModuleMeasProgram::deactivationContinue, &m_freeUSERMemState);
    m_freeUSERMemState.addTransition(this, &cOsciModuleMeasProgram::deactivationContinue, &m_unloadDSPDoneState);
    m_deactivationMachine.addState(&m_deactivateDSPState);
    m_deactivationMachine.addState(&m_freePGRMemState);
    m_deactivationMachine.addState(&m_freeUSERMemState);
    m_deactivationMachine.addState(&m_unloadDSPDoneState);

    m_deactivationMachine.setInitialState(&m_deactivateDSPState);

    connect(&m_deactivateDSPState, &QState::entered, this, &cOsciModuleMeasProgram::deactivateDSP);
    connect(&m_freePGRMemState, &QState::entered, this, &cOsciModuleMeasProgram::freePGRMem);
    connect(&m_freeUSERMemState, &QState::entered, this, &cOsciModuleMeasProgram::freeUSERMem);
    connect(&m_unloadDSPDoneState, &QState::entered, this, &cOsciModuleMeasProgram::deactivateDSPdone);

    // setting up statemachine for data acquisition
    m_dataAcquisitionState.addTransition(this, &cOsciModuleMeasProgram::dataAquisitionContinue, &m_dataAcquisitionDoneState);
    m_dataAcquisitionMachine.addState(&m_dataAcquisitionState);
    m_dataAcquisitionMachine.addState(&m_dataAcquisitionDoneState);
    m_dataAcquisitionMachine.setInitialState(&m_dataAcquisitionState);
    connect(&m_dataAcquisitionState, &QState::entered, this, &cOsciModuleMeasProgram::dataAcquisitionDSP);
    connect(&m_dataAcquisitionDoneState, &QState::entered, this, &cOsciModuleMeasProgram::dataReadDSP);

    connect(this, &cOsciModuleMeasProgram::actualValues,
            &m_startStopHandler, &ActualValueStartStopHandler::onNewActualValues);
    connect(&m_startStopHandler, &ActualValueStartStopHandler::sigNewActualValues,
            this, &cOsciModuleMeasProgram::setInterfaceActualValues);
}

void cOsciModuleMeasProgram::start()
{
    m_startStopHandler.start();
}

void cOsciModuleMeasProgram::stop()
{
    m_startStopHandler.stop();
}

void cOsciModuleMeasProgram::generateVeinInterface()
{
    QString key;

    VfModuleActvalue *pActvalue;
    int n;
    n = getConfData()->m_valueChannelList.count();

    for (int i = 0; i < n; i++)
    {
        pActvalue = new VfModuleActvalue(m_pModule->getEntityId(), m_pModule->m_pModuleValidator,
                                            QString("ACT_OSCI%1").arg(i+1),
                                            QString("Measures samples"));
        m_veinActValueList.append(pActvalue); // we add the component for our measurement
        m_pModule->veinModuleActvalueList.append(pActvalue); // and for the modules interface
    }

    m_pOsciCountInfo = new VfModuleMetaData(QString("OSCICount"), QVariant(n));
    m_pModule->veinModuleMetaDataList.append(m_pOsciCountInfo);

    QString refChannelMNameConfigured = getConfData()->m_RefChannel.m_sPar;
    m_pRefChannelParameter = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->m_pModuleValidator,
                                                   key = QString("PAR_RefChannel"),
                                                   QString("Reference channel"),
                                                   refChannelMNameConfigured);

    m_pRefChannelParameter->setSCPIInfo(new cSCPIInfo("CONFIGURATION","REFCHANNEL", "10", "PAR_RefChannel", "0", ""));

    m_pModule->m_veinModuleParameterMap[key] = m_pRefChannelParameter; // for modules use

    cStringValidator *sValidator;
    sValidator = new cStringValidator(getConfData()->m_valueChannelList);
    m_pRefChannelParameter->setValidator(sValidator);

    m_pMeasureSignal = new VfModuleComponent(m_pModule->getEntityId(), m_pModule->m_pModuleValidator,
                                                QString("SIG_Measuring"),
                                                QString("Signal indicating measurement activity"),
                                                QVariant(0));

    m_pModule->veinModuleComponentList.append(m_pMeasureSignal);
}

void cOsciModuleMeasProgram::setDspVarList()
{
    // we fetch a handle for sampled data and other temporary values
    int samples = m_pModule->getSharedChannelRangeObserver()->getSampleRate();
    m_pTmpDataDsp = m_dspInterface->getMemHandle("TmpData");
    m_pTmpDataDsp->addVarItem( new cDspVar("MEASSIGNAL", m_veinActValueList.count() * samples, DSPDATA::vDspTemp));
    m_pTmpDataDsp->addVarItem(new cDspVar("WORKSPACE", 2 * samples, DSPDATA::vDspTemp));
    m_pTmpDataDsp->addVarItem( new cDspVar("GAPCOUNT", 1, DSPDATA::vDspTemp, DSPDATA::dInt));
    m_pTmpDataDsp->addVarItem( new cDspVar("GAPPAR",1, DSPDATA::vDspTemp, DSPDATA::dInt));
    m_pTmpDataDsp->addVarItem( new cDspVar("IPOLADR", 1, DSPDATA::vDspTemp, DSPDATA::dInt));
    m_pTmpDataDsp->addVarItem(new cDspVar("DEBUGCOUNT",1,DSPDATA::vDspTemp, DSPDATA::dInt));
    m_pTmpDataDsp->addVarItem( new cDspVar("DFTREF", 2, DSPDATA::vDspTemp));

    // a handle for parameter
    m_pParameterDSP =  m_dspInterface->getMemHandle("Parameter");
    m_pParameterDSP->addVarItem( new cDspVar("REFCHN",1, DSPDATA::vDspParam, DSPDATA::dInt));

    // and one for actual values
    m_pActualValuesDSP = m_dspInterface->getMemHandle("ActualValues");
    m_pActualValuesDSP->addVarItem( new cDspVar("VALXOSCI",m_veinActValueList.count() * getConfData()->m_nInterpolation, DSPDATA::vDspResult));

    m_ModuleActualValues.resize(m_pActualValuesDSP->getSize()); // we provide a vector for generated actual values
    m_nDspMemUsed = m_pTmpDataDsp->getSize() + m_pParameterDSP->getSize() + m_pActualValuesDSP->getSize();
}

void cOsciModuleMeasProgram::deleteDspVarList()
{
    m_dspInterface->deleteMemHandle(m_pTmpDataDsp);
    m_dspInterface->deleteMemHandle(m_pParameterDSP);
    m_dspInterface->deleteMemHandle(m_pActualValuesDSP);
}

void cOsciModuleMeasProgram::setDspCmdList()
{
    QString s;
    ChannelRangeObserver::SystemObserverPtr observer = m_pModule->getSharedChannelRangeObserver();
    int samples = observer->getSampleRate();
    QString referenceChannel = getConfData()->m_RefChannel.m_sPar;
    int referenceDspChannel = observer->getChannel(referenceChannel)->m_dspChannel;
    m_dspInterface->addCycListItem( s = "STARTCHAIN(1,1,0x0101)"); // aktiv, prozessnr. (dummy),hauptkette 1 subkette 1 start
        m_dspInterface->addCycListItem( s = QString("CLEARN(%1,MEASSIGNAL)").arg(m_veinActValueList.count() * samples) ); // clear meassignal
        m_dspInterface->addCycListItem( s = QString("SETVAL(GAPCOUNT,%1)").arg(getConfData()->m_nGap)); // we start with the first period
        m_dspInterface->addCycListItem( s = QString("SETVAL(GAPPAR,%1)").arg(getConfData()->m_nGap+1)); // our value to reload gap
        m_dspInterface->addCycListItem( s = QString("SETVAL(REFCHN,%1)").arg(referenceDspChannel));
        m_dspInterface->addCycListItem( s = QString("SETVAL(DEBUGCOUNT,0)"));
        m_dspInterface->addCycListItem( s = "DEACTIVATECHAIN(1,0x0101)"); // ende prozessnr., hauptkette 1 subkette 1
    m_dspInterface->addCycListItem( s = "STOPCHAIN(1,0x0101)"); // ende prozessnr., hauptkette 1 subkette 1

    // now lets do our sampling job if necessary


    // next 3 commands for debug purpose , will be removed later
    // m_dspInterface->addCycListItem( s = "INC(DEBUGCOUNT)");
    // m_dspInterface->addCycListItem( s = "TESTVCSKIPLT(DEBUGCOUNT,1000)");
    // m_dspInterface->addCycListItem( s = "BREAK(1)");

    m_dspInterface->addCycListItem( s = "INC(GAPCOUNT)");
    m_dspInterface->addCycListItem( s = "ACTIVATECHAIN(1,0x0102)");
    m_dspInterface->addCycListItem( s = "TESTVVSKIPEQ(GAPCOUNT,GAPPAR)");
    m_dspInterface->addCycListItem( s = "DEACTIVATECHAIN(1,0x0102)");

    m_dspInterface->addCycListItem( s = "STARTCHAIN(0,1,0x0102)");
        m_dspInterface->addCycListItem( s = "SETVAL((GAPCOUNT,0)"); // next gap

        // we compute the phase of our reference channel first
        m_dspInterface->addCycListItem( s = QString("COPYDATAIND(REFCHN,0,WORKSPACE)"));
        m_dspInterface->addCycListItem( s = QString("DFT(1,WORKSPACE,DFTREF)"));
        m_dspInterface->addCycListItem( s = QString("GENADR(WORKSPACE,DFTREF,IPOLADR)"));

        // now we do all necessary for each channel we work on
        for (int i = 0; i < m_veinActValueList.count(); i++)
        {
            QString channelMName = getConfData()->m_valueChannelList[i];
            int dspChannel = observer->getChannel(channelMName)->m_dspChannel;
            m_dspInterface->addCycListItem( s = QString("COPYMEM(%1,MEASSIGNAL+%2,WORKSPACE)").arg(samples).arg(i * samples));
            m_dspInterface->addCycListItem( s = QString("COPYDATA(CH%1,0,WORKSPACE+%2)").arg(dspChannel).arg(samples));

            m_dspInterface->addCycListItem( s = QString("INTERPOLATIONIND(%1,IPOLADR,VALXOSCI+%2)")
                                             .arg(getConfData()->m_nInterpolation)
                                             .arg(i * getConfData()->m_nInterpolation));
            m_dspInterface->addCycListItem( s = QString("COPYMEM(%1,WORKSPACE+%2,MEASSIGNAL+%3)").arg(samples).arg(samples).arg(i * samples));

        }

        m_dspInterface->addCycListItem( s = QString("DSPINTTRIGGER(0x0,0x%1)").arg(irqNr)); // send interrupt to module
        m_dspInterface->addCycListItem( s = "DEACTIVATECHAIN(1,0x0102)");

    m_dspInterface->addCycListItem( s = "STOPCHAIN(1,0x0102)"); // end processnr., mainchain 1 subchain 2

}


void cOsciModuleMeasProgram::deleteDspCmdList()
{
    m_dspInterface->clearCmdList();
}


void cOsciModuleMeasProgram::catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer)
{
    bool ok;

    if (msgnr == 0) // 0 was reserved for async. messages
    {
        QString sintnr;
        sintnr = answer.toString().section(':', 1, 1);
        int service = sintnr.toInt(&ok);
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

cOsciModuleConfigData *cOsciModuleMeasProgram::getConfData()
{
    return qobject_cast<cOsciModuleConfiguration*>(m_pConfiguration.get())->getConfigurationData();
}

void cOsciModuleMeasProgram::setActualValuesNames()
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

void cOsciModuleMeasProgram::setSCPIMeasInfo()
{
    cSCPIInfo* pSCPIInfo;

    for (int i = 0; i < getConfData()->m_valueChannelList.count(); i++)
    {
        pSCPIInfo = new cSCPIInfo("MEASURE", m_veinActValueList.at(i)->getChannelName(), "8", m_veinActValueList.at(i)->getName(), "0", m_veinActValueList.at(i)->getUnit());
        m_veinActValueList.at(i)->setSCPIInfo(pSCPIInfo);
    }
}

void cOsciModuleMeasProgram::setInterfaceActualValues(QVector<float> *actualValues)
{
    if (m_bActive) // maybe we are deactivating !!!!
    {

        for (int i = 0; i < m_veinActValueList.count(); i++)
        {
            QList<double> osciList;
            int offs = i * getConfData()->m_nInterpolation;

            for (int j = 0; j < getConfData()->m_nInterpolation; j++)
                osciList.append(actualValues->at(offs + j));

            QVariant list;
            list = QVariant::fromValue<QList<double> >(osciList);
            m_veinActValueList.at(i)->setValue(list); // and set entities
        }
    }
}

void cOsciModuleMeasProgram::startFetchCommonRanges()
{
    m_pModule->getSharedChannelRangeObserver()->startFullScan();
}

void cOsciModuleMeasProgram::resourceManagerConnect()
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
    // and then we set resource manager interface's connection
    m_rmInterface.setClientSmart(m_rmClient);
    m_resourceManagerConnectState.addTransition(m_rmClient.get(), &Zera::ProxyClient::connected, &m_IdentifyState);
    connect(&m_rmInterface, &Zera::cRMInterface::serverAnswer, this, &cOsciModuleMeasProgram::catchInterfaceAnswer);
    Zera::Proxy::getInstance()->startConnectionSmart(m_rmClient);
}

void cOsciModuleMeasProgram::sendRMIdent()
{
    m_MsgNrCmdList[m_rmInterface.rmIdent(QString("OsciModule%1").arg(m_pModule->getModuleNr()))] = sendrmident;
}

void cOsciModuleMeasProgram::pcbserverConnect()
{
    m_pcbClient = Zera::Proxy::getInstance()->getConnectionSmart(m_pModule->getNetworkConfig()->m_pcbServiceConnectionInfo,
                                                                 m_pModule->getNetworkConfig()->m_tcpNetworkFactory);
    m_pcbInterface->setClientSmart(m_pcbClient);
    connect(m_pcbClient.get(), &Zera::ProxyClient::connected, this, &cBaseMeasProgram::activationContinue);
    connect(m_pcbInterface.get(), &AbstractServerInterface::serverAnswer, this, &cOsciModuleMeasProgram::catchInterfaceAnswer);
    Zera::Proxy::getInstance()->startConnectionSmart(m_pcbClient);
}

void cOsciModuleMeasProgram::dspserverConnect()
{
    m_dspClient = Zera::Proxy::getInstance()->getConnectionSmart(m_pModule->getNetworkConfig()->m_dspServiceConnectionInfo,
                                                                 m_pModule->getNetworkConfig()->m_tcpNetworkFactory);
    m_dspInterface->setClientSmart(m_dspClient);
    m_dspserverConnectState.addTransition(m_dspClient.get(), &Zera::ProxyClient::connected, &m_claimPGRMemState);
    connect(m_dspInterface.get(), &Zera::cDSPInterface::serverAnswer, this, &cOsciModuleMeasProgram::catchInterfaceAnswer);
    Zera::Proxy::getInstance()->startConnectionSmart(m_dspClient);
}

void cOsciModuleMeasProgram::claimPGRMem()
{
    setDspVarList(); // first we set the var list for our dsp
    setDspCmdList(); // and the cmd list he has to work on
    m_MsgNrCmdList[m_rmInterface.setResource("DSP1", "PGRMEMC", m_dspInterface->cmdListCount())] = claimpgrmem;
}

void cOsciModuleMeasProgram::claimUSERMem()
{
   m_MsgNrCmdList[m_rmInterface.setResource("DSP1", "USERMEM", m_nDspMemUsed)] = claimusermem;
}

void cOsciModuleMeasProgram::varList2DSP()
{
    m_MsgNrCmdList[m_dspInterface->varList2Dsp()] = varlist2dsp;
}

void cOsciModuleMeasProgram::cmdList2DSP()
{
    m_MsgNrCmdList[m_dspInterface->cmdList2Dsp()] = cmdlist2dsp;
}

void cOsciModuleMeasProgram::activateDSP()
{
    m_MsgNrCmdList[m_dspInterface->activateInterface()] = activatedsp; // aktiviert die var- und cmd-listen im dsp
}

void cOsciModuleMeasProgram::activateDSPdone()
{
    m_bActive = true;

    setActualValuesNames();
    setSCPIMeasInfo();

    m_pMeasureSignal->setValue(QVariant(1));
    connect(m_pRefChannelParameter, &VfModuleParameter::sigValueChanged, this, &cOsciModuleMeasProgram::newRefChannel);
    emit activated();
}

void cOsciModuleMeasProgram::deactivateDSP()
{
    m_bActive = false;
    m_MsgNrCmdList[m_dspInterface->deactivateInterface()] = deactivatedsp; // wat wohl
}

void cOsciModuleMeasProgram::freePGRMem()
{
    Zera::Proxy::getInstance()->releaseConnection(m_dspClient.get());
    deleteDspVarList();
    deleteDspCmdList();

    m_MsgNrCmdList[m_rmInterface.freeResource("DSP1", "PGRMEMC")] = freepgrmem;
}

void cOsciModuleMeasProgram::freeUSERMem()
{
    m_MsgNrCmdList[m_rmInterface.freeResource("DSP1", "USERMEM")] = freeusermem;
}

void cOsciModuleMeasProgram::deactivateDSPdone()
{
    disconnect(&m_rmInterface, 0, this, 0);
    disconnect(m_dspInterface.get(), 0, this, 0);
    disconnect(m_pcbInterface.get(), 0, this, 0);
    emit deactivated();
}

void cOsciModuleMeasProgram::dataAcquisitionDSP()
{
    m_pMeasureSignal->setValue(QVariant(0));
    m_MsgNrCmdList[m_dspInterface->dataAcquisition(m_pActualValuesDSP)] = dataaquistion; // we start our data aquisition now
}

void cOsciModuleMeasProgram::dataReadDSP()
{
    if (m_bActive) {
        m_ModuleActualValues = m_pActualValuesDSP->getData();
        emit actualValues(&m_ModuleActualValues); // and send them
        m_pMeasureSignal->setValue(QVariant(1)); // signal measuring
    }
}

void cOsciModuleMeasProgram::newRefChannel(QVariant chn)
{
    QString channelMName = chn.toString();
    getConfData()->m_RefChannel.m_sPar = channelMName;
    ChannelRangeObserver::SystemObserverPtr observer = m_pModule->getSharedChannelRangeObserver();
    ChannelRangeObserver::ChannelPtr channel = observer->getChannel(channelMName);
    int dspChannel = channel->m_dspChannel;
    DspInterfaceCmdDecoder::setVarData(m_pParameterDSP, QString("REFCHN:%1;").arg(dspChannel));
    m_MsgNrCmdList[m_dspInterface->dspMemoryWrite(m_pParameterDSP)] = writeparameter;
    emit m_pModule->parameterChanged();
}

}
