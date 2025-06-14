#include "dftmodulemeasprogram.h"
#include "dftmodule.h"
#include "dftmoduleconfiguration.h"
#include "dftmoduleconfigdata.h"
#include "servicechannelnamehelper.h"
#include "vfmodulecomponent.h"
#include <errormessages.h>
#include <movingwindowfilter.h>
#include <scpi.h>
#include <timerfactoryqt.h>
#include <reply.h>
#include <proxy.h>
#include <stringvalidator.h>
#include <doublevalidator.h>
#include <math.h>
#include <complex>
#include <useratan.h>

namespace DFTMODULE
{

cDftModuleMeasProgram::cDftModuleMeasProgram(cDftModule* module, std::shared_ptr<BaseModuleConfiguration> pConfiguration) :
    cBaseDspMeasProgram(pConfiguration, module->getVeinModuleName()),
    m_pModule(module)
{
    m_dspInterface = m_pModule->getServiceInterfaceFactory()->createDspInterfaceDft(
        m_pModule->getEntityId(),
        getConfData()->m_valueChannelList,
        getConfData()->m_nDftOrder);

    m_IdentifyState.addTransition(this, &cDftModuleMeasProgram::activationContinue, &m_dspserverConnectState);

    m_claimPGRMemState.addTransition(this, &cDftModuleMeasProgram::activationContinue, &m_claimUSERMemState);
    m_claimUSERMemState.addTransition(this, &cDftModuleMeasProgram::activationContinue, &m_var2DSPState);
    m_var2DSPState.addTransition(this, &cDftModuleMeasProgram::activationContinue, &m_cmd2DSPState);
    m_cmd2DSPState.addTransition(this, &cDftModuleMeasProgram::activationContinue, &m_activateDSPState);
    m_activateDSPState.addTransition(this, &cDftModuleMeasProgram::activationContinue, &m_loadDSPDoneState);

    m_activationMachine.addState(&m_resourceManagerConnectState);
    m_activationMachine.addState(&m_IdentifyState);
    m_activationMachine.addState(&m_dspserverConnectState);
    m_activationMachine.addState(&m_claimPGRMemState);
    m_activationMachine.addState(&m_claimUSERMemState);
    m_activationMachine.addState(&m_var2DSPState);
    m_activationMachine.addState(&m_cmd2DSPState);
    m_activationMachine.addState(&m_activateDSPState);
    m_activationMachine.addState(&m_loadDSPDoneState);

    m_activationMachine.setInitialState(&m_resourceManagerConnectState);

    connect(&m_resourceManagerConnectState, &QState::entered, this, &cDftModuleMeasProgram::resourceManagerConnect);
    connect(&m_IdentifyState, &QState::entered, this, &cDftModuleMeasProgram::sendRMIdent);
    connect(&m_dspserverConnectState, &QState::entered, this, &cDftModuleMeasProgram::dspserverConnect);
    connect(&m_claimPGRMemState, &QState::entered, this, &cDftModuleMeasProgram::claimPGRMem);
    connect(&m_claimUSERMemState, &QState::entered, this, &cDftModuleMeasProgram::claimUSERMem);
    connect(&m_var2DSPState, &QState::entered, this, &cDftModuleMeasProgram::varList2DSP);
    connect(&m_cmd2DSPState, &QState::entered, this, &cDftModuleMeasProgram::cmdList2DSP);
    connect(&m_activateDSPState, &QState::entered, this, &cDftModuleMeasProgram::activateDSP);
    connect(&m_loadDSPDoneState, &QState::entered, this, &cDftModuleMeasProgram::activateDSPdone);

    // setting up statemachine for unloading dsp and setting resources free
    m_freePGRMemState.addTransition(this, &cDftModuleMeasProgram::deactivationContinue, &m_freeUSERMemState);
    m_freeUSERMemState.addTransition(this, &cDftModuleMeasProgram::deactivationContinue, &m_unloadDSPDoneState);
    m_deactivationMachine.addState(&m_freePGRMemState);
    m_deactivationMachine.addState(&m_freeUSERMemState);
    m_deactivationMachine.addState(&m_unloadDSPDoneState);

    m_deactivationMachine.setInitialState(&m_freePGRMemState);

    connect(&m_freePGRMemState, &QState::entered, this, &cDftModuleMeasProgram::freePGRMem);
    connect(&m_freeUSERMemState, &QState::entered, this, &cDftModuleMeasProgram::freeUSERMem);
    connect(&m_unloadDSPDoneState, &QState::entered, this, &cDftModuleMeasProgram::deactivateDSPdone);

    // setting up statemachine for data acquisition
    m_dataAcquisitionState.addTransition(this, &cDftModuleMeasProgram::dataAquisitionContinue, &m_dataAcquisitionDoneState);
    m_dataAcquisitionMachine.addState(&m_dataAcquisitionState);
    m_dataAcquisitionMachine.addState(&m_dataAcquisitionDoneState);
    m_dataAcquisitionMachine.setInitialState(&m_dataAcquisitionState);
    connect(&m_dataAcquisitionState, &QState::entered, this, &cDftModuleMeasProgram::dataAcquisitionDSP);
    connect(&m_dataAcquisitionDoneState, &QState::entered, this, &cDftModuleMeasProgram::dataReadDSP);

    connect(this, &cDftModuleMeasProgram::actualValues,
            &m_startStopHandler, &ActualValueStartStopHandler::onNewActualValues);
    if (getConfData()->m_bmovingWindow) {
        m_movingwindowFilter.setIntegrationtime(getConfData()->m_fMeasInterval.m_fValue);
        connect(&m_startStopHandler, &ActualValueStartStopHandler::sigNewActualValues,
                &m_movingwindowFilter, &cMovingwindowFilter::receiveActualValues);
        connect(&m_movingwindowFilter, &cMovingwindowFilter::actualValues,
                this, &cDftModuleMeasProgram::setInterfaceActualValues);
    }
    else
        connect(&m_startStopHandler, &ActualValueStartStopHandler::sigNewActualValues,
                this, &cDftModuleMeasProgram::setInterfaceActualValues);
}

void cDftModuleMeasProgram::start()
{
    m_startStopHandler.start();
}

void cDftModuleMeasProgram::stop()
{
    m_startStopHandler.stop();
}

void cDftModuleMeasProgram::generateVeinInterface()
{
    VfModuleComponent *pActvalue;
    int n,p;
    n = p = 0;
    for (int i = 0; i < getConfData()->m_valueChannelList.count(); i++) {
        QStringList sl = getConfData()->m_valueChannelList.at(i).split('-');
        // we have 1 or 2 entries for each value
        if (sl.count() == 1) { // in this case we have phase,neutral value
            QString channelDescriptionCartesian;
            QString channelDescriptionPolar;
            if(sl.contains("m0") || sl.contains("m1") || sl.contains("m2") || sl.contains("m6")) { //voltage channels
                channelDescriptionCartesian = QString("Actual value phase/neutral / cartesian format: re,im");
                channelDescriptionPolar = QString("Actual value phase/neutral / polar format: abs,rad[0,2π],deg[0,360]");
            }
            else { //current channels
                channelDescriptionCartesian = QString("Actual value / cartesian format: re,im");
                channelDescriptionPolar = QString("Actual value / polar format: abs,rad[0,2π],deg[0,360]");
            }
            pActvalue = new VfModuleComponent(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                                QString("ACT_DFTPN%1").arg(n+1),
                                                channelDescriptionCartesian);
            m_veinActValueList.append(pActvalue); // we add the component for our measurement
            m_pModule->veinModuleActvalueList.append(pActvalue); // and for the modules interface

            pActvalue = new VfModuleComponent(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                             QString("ACT_POL_DFTPN%1").arg(n+1),
                                             channelDescriptionPolar);
            m_veinPolarValue.append(pActvalue); // we add the component for our measurement
            m_pModule->veinModuleActvalueList.append(pActvalue); // and for the modules interface

            n++;
        }
        else {
            pActvalue = new VfModuleComponent(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                                QString("ACT_DFTPP%1").arg(p+1),
                                                QString("Actual value phase/phase / cartesian format: re,im"));
            m_veinActValueList.append(pActvalue); // we add the component for our measurement
            m_pModule->veinModuleActvalueList.append(pActvalue); // and for the modules interface

            pActvalue = new VfModuleComponent(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                             QString("ACT_POL_DFTPP%1").arg(p+1),
                                             QString("Actual value phase/phase / polar format: abs,rad[0,2π],deg[0,360]"));
            m_veinPolarValue.append(pActvalue); // we add the component for our measurement
            m_pModule->veinModuleActvalueList.append(pActvalue); // and for the modules interface

            p++;
        }
    }

    m_pRFieldActualValue = new VfModuleComponent(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                                   QString("ACT_RFIELD"),
                                                   QString("Phase sequence"));

    m_pModule->veinModuleActvalueList.append(m_pRFieldActualValue); // we add the component for the modules interface

    m_pDFTPNCountInfo = new VfModuleMetaData(QString("DFTPNCount"), QVariant(n));
    m_pModule->veinModuleMetaDataList.append(m_pDFTPNCountInfo);
    m_pDFTPPCountInfo = new VfModuleMetaData(QString("DFTPPCount"), QVariant(p));
    m_pModule->veinModuleMetaDataList.append(m_pDFTPPCountInfo);
    m_pDFTOrderInfo = new VfModuleMetaData(QString("DFTOrder"), QVariant(getConfData()->m_nDftOrder));
    m_pModule->veinModuleMetaDataList.append(m_pDFTOrderInfo);

    QString key;
    m_pIntegrationTimeParameter = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                                           key = QString("PAR_Interval"),
                                                           QString("Integration time"),
                                                           QVariant(getConfData()->m_fMeasInterval.m_fValue));
    m_pIntegrationTimeParameter->setUnit("s");
    m_pIntegrationTimeParameter->setScpiInfo("CONFIGURATION","TINTEGRATION", SCPI::isQuery|SCPI::isCmdwP, "PAR_Interval");
    m_pIntegrationTimeParameter->setValidator(new cDoubleValidator(1.0, 100.0, 0.5));
    m_pModule->m_veinModuleParameterMap[key] = m_pIntegrationTimeParameter; // for modules use


    QString refChannelAliasConfigured = getConfData()->m_sRefChannel.m_sPar;
    m_pRefChannelParameter = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                                   key = QString("PAR_RefChannel"),
                                                   QString("Reference channel"),
                                                   refChannelAliasConfigured);

    m_pRefChannelParameter->setScpiInfo("CONFIGURATION","REFCHANNEL", SCPI::isQuery|SCPI::isCmdwP, "PAR_RefChannel");

    m_pModule->m_veinModuleParameterMap[key] = m_pRefChannelParameter; // for modules use
    // we must set validator after activation because we don't know the channel names here

    m_pMeasureSignal = new VfModuleComponent(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                                QString("SIG_Measuring"),
                                                QString("Signal indicating measurement activity"),
                                                QVariant(0));

    m_pModule->veinModuleComponentList.append(m_pMeasureSignal);
}


void cDftModuleMeasProgram::setDspVarList()
{
    int samples = m_pModule->getSharedChannelRangeObserver()->getSamplesPerPeriod();
    // we fetch a handle for sampled data and other temporary values
    m_pTmpDataDsp = m_dspInterface->getMemHandle("TmpData");
    m_pTmpDataDsp->addDspVar("MEASSIGNAL", samples, DSPDATA::vDspTemp);
    m_pTmpDataDsp->addDspVar("VALXDFT",2*m_veinActValueList.count(), DSPDATA::vDspTemp);
    m_pTmpDataDsp->addDspVar("FILTER",2*2*m_veinActValueList.count(), DSPDATA::vDspTemp);
    m_pTmpDataDsp->addDspVar("N",1,DSPDATA::vDspTemp);

    // a handle for parameter
    m_pParameterDSP =  m_dspInterface->getMemHandle("Parameter");
    m_pParameterDSP->addDspVar("TIPAR",1, DSPDATA::vDspParam, DSPDATA::dInt); // integrationtime res = 1ms
    // we use tistart as parameter, so we can finish actual measuring interval bei setting 0
    m_pParameterDSP->addDspVar("TISTART",1, DSPDATA::vDspParam, DSPDATA::dInt);

    // and one for filtered actual values
    m_pActualValuesDSP = m_dspInterface->getMemHandle("ActualValues");
    m_pActualValuesDSP->addDspVar("VALXDFTF",2*m_veinActValueList.count(), DSPDATA::vDspResult);

    m_ModuleActualValues.resize(m_pActualValuesDSP->getSize()); // we provide a vector for generated actual values
    m_nDspMemUsed = m_pTmpDataDsp->getSize() + m_pParameterDSP->getSize() + m_pActualValuesDSP->getSize();
}

void cDftModuleMeasProgram::setDspCmdList()
{
    ChannelRangeObserver::SystemObserverPtr observer = m_pModule->getSharedChannelRangeObserver();
    int samples = observer->getSamplesPerPeriod();
    m_dspInterface->addCycListItem("STARTCHAIN(1,1,0x0101)"); // aktiv, prozessnr. (dummy),hauptkette 1 subkette 1 start
        m_dspInterface->addCycListItem(QString("CLEARN(%1,MEASSIGNAL)").arg(samples) ); // clear meassignal
        m_dspInterface->addCycListItem(QString("CLEARN(%1,FILTER)").arg(2*2*m_veinActValueList.count()+1) ); // clear the whole filter incl. count

        if (getConfData()->m_bmovingWindow)
            m_dspInterface->addCycListItem(QString("SETVAL(TIPAR,%1)").arg(getConfData()->m_fmovingwindowInterval*1000.0)); // initial ti time
        else
            m_dspInterface->addCycListItem(QString("SETVAL(TIPAR,%1)").arg(getConfData()->m_fMeasInterval.m_fValue*1000.0)); // initial ti time

        m_dspInterface->addCycListItem("GETSTIME(TISTART)"); // einmal ti start setzen
        m_dspInterface->addCycListItem("DEACTIVATECHAIN(1,0x0101)"); // ende prozessnr., hauptkette 1 subkette 1
    m_dspInterface->addCycListItem("STOPCHAIN(1,0x0101)"); // ende prozessnr., hauptkette 1 subkette 1

    // we compute or copy our wanted actual values
    for (int i = 0; i < getConfData()->m_valueChannelList.count(); i++)
    {
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
        m_dspInterface->addCycListItem(QString("DFT(%1,MEASSIGNAL,VALXDFT+%2)").arg(getConfData()->m_nDftOrder).arg(2*i));
    }

    // and filter them
    m_dspInterface->addCycListItem(QString("AVERAGE1(%1,VALXDFT,FILTER)").arg(2*m_veinActValueList.count())); // we add results to filter

    m_dspInterface->addCycListItem("TESTTIMESKIPNEX(TISTART,TIPAR)");
    m_dspInterface->addCycListItem("ACTIVATECHAIN(1,0x0102)");

    m_dspInterface->addCycListItem("STARTCHAIN(0,1,0x0102)");
        m_dspInterface->addCycListItem("GETSTIME(TISTART)"); // set new system time
        m_dspInterface->addCycListItem(QString("CMPAVERAGE1(%1,FILTER,VALXDFTF)").arg(2*m_veinActValueList.count()));
        m_dspInterface->addCycListItem(QString("CLEARN(%1,FILTER)").arg(2*2*m_veinActValueList.count()+1) );
        m_dspInterface->addCycListItem(QString("DSPINTTRIGGER(0x0,0x%1)").arg(0)); // send interrupt to module
        m_dspInterface->addCycListItem("DEACTIVATECHAIN(1,0x0102)");
    m_dspInterface->addCycListItem("STOPCHAIN(1,0x0102)"); // end processnr., mainchain 1 subchain 2
}


void cDftModuleMeasProgram::deleteDspCmdList()
{
    m_dspInterface->clearCmdList();
}


void cDftModuleMeasProgram::catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer)
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

cDftModuleConfigData *cDftModuleMeasProgram::getConfData()
{
    return qobject_cast<cDftModuleConfiguration*>(m_pConfiguration.get())->getConfigurationData();
}

void cDftModuleMeasProgram::setActualValuesNames()
{
    ChannelRangeObserver::SystemObserverPtr observer = m_pModule->getSharedChannelRangeObserver();
    const QStringList &channelList = getConfData()->m_valueChannelList;
    for(int i = 0; i < channelList.count(); i++) {
        const QString &channelMNamesEntry = getConfData()->m_valueChannelList.at(i);
        ServiceChannelNameHelper::TChannelAliasUnit aliasUnit =
            ServiceChannelNameHelper::getChannelAndUnit(channelMNamesEntry, observer);
        m_veinActValueList.at(i)->setChannelName(aliasUnit.m_channelAlias);
        m_veinActValueList.at(i)->setUnit(aliasUnit.m_channelUnit);
        m_veinPolarValue.at(i)->setChannelName(aliasUnit.m_channelAlias);
        m_veinPolarValue.at(i)->setUnit(aliasUnit.m_channelUnit);
    }
}

void cDftModuleMeasProgram::setSCPIMeasInfo()
{
    for (int i = 0; i < getConfData()->m_valueChannelList.count(); i++) {
        QString channelName = m_veinPolarValue.at(i)->getChannelName();
        m_veinActValueList.at(i)->setScpiInfo("MEASURE", channelName, SCPI::isCmdwP, m_veinActValueList.at(i)->getName());

        // Try hard to find unique names with four letters...
        QString polarChannelName = channelName.replace("L", "").replace("-", "").replace("AUX", "4");
        m_veinPolarValue.at(i)->setScpiInfo("MEASURE", polarChannelName, SCPI::isCmdwP, m_veinPolarValue.at(i)->getName());
    }
    m_pRFieldActualValue->setScpiInfo("MEASURE", "RFIELD", SCPI::isCmdwP, m_pRFieldActualValue->getName());
}


void cDftModuleMeasProgram::setRefChannelValidator()
{
    ChannelRangeObserver::SystemObserverPtr observer =  m_pModule->getSharedChannelRangeObserver();
    const QStringList channelMNames = observer->getChannelMNames();
    QStringList channelAliases;
    for(const QString &channelMName : channelMNames) {
        if(getConfData()->m_valueChannelList.contains(channelMName)) {
            ChannelRangeObserver::ChannelPtr channel = observer->getChannel(channelMName);
            const QString alias = channel->getAlias();
            channelAliases.append(alias);
            m_ChannelSystemNameHash[alias] = channelMName;
        }
    }
    m_pRefChannelParameter->setValidator(new cStringValidator(channelAliases));
}


void cDftModuleMeasProgram::initRFieldMeasurement()
{
    QString s;
    for (int i = 0; i < getConfData()->m_rfieldChannelList.length(); i++)
        rfieldActvalueIndexList.append(getConfData()->m_valueChannelList.indexOf(s = getConfData()->m_rfieldChannelList.at(i)));
}


void cDftModuleMeasProgram::setInterfaceActualValues(QVector<float> *actualValues)
{
    if (m_bActive) { // maybe we are deactivating !!!!
        for (int i = 0; i < m_veinActValueList.count(); i++) {
            double re = actualValues->at(2*i);
            double im = actualValues->at(2*i+1);

            QList<double> dftResult;
            dftResult.append(re);
            dftResult.append(im);
            QVariant list = QVariant::fromValue<QList<double>>(dftResult);
            m_veinActValueList.at(i)->setValue(list); // and set entities

            QList<double> dftResultPolar;
            double abs = sqrt(re*re + im*im);
            dftResultPolar.append(abs);
            double rad = atan2(im, re); // y=im / x=re
            if(rad < 0)
                rad += 2*M_PI;
            dftResultPolar.append(rad);
            double deg = rad / M_PI * 180;
            dftResultPolar.append(deg);

            QVariant listPolar = QVariant::fromValue<QList<double>>(dftResultPolar);
            m_veinPolarValue.at(i)->setValue(listPolar);
        }

        // rfield computation
        double angle[3];
        for (int j = 0; j < 3; j++)
            angle[j] = userAtan(actualValues->at(2*rfieldActvalueIndexList.at(j)+1), actualValues->at(2*rfieldActvalueIndexList.at(j)));
        if ((angle[0] < angle[1]) && (angle[1] < angle[2]))
            m_pRFieldActualValue->setValue("123");
        else
            m_pRFieldActualValue->setValue("132");
    }
}

void cDftModuleMeasProgram::resourceManagerConnect()
{
    // as this is our entry point when activating the module, we do some initialization first
    m_measChannelInfoHash.clear(); // we build up a new channel info hash
    cMeasChannelInfo mi;
    for (int i = 0; i < getConfData()->m_valueChannelList.count(); i++)
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
    connect(&m_rmInterface, &AbstractServerInterface::serverAnswer, this, &cDftModuleMeasProgram::catchInterfaceAnswer);
    Zera::Proxy::getInstance()->startConnectionSmart(m_rmClient);
}


void cDftModuleMeasProgram::sendRMIdent()
{
    m_MsgNrCmdList[m_rmInterface.rmIdent(QString("DftModule%1").arg(m_pModule->getModuleNr()))] = sendrmident;
}

void cDftModuleMeasProgram::dspserverConnect()
{
    m_dspClient = Zera::Proxy::getInstance()->getConnectionSmart(m_pModule->getNetworkConfig()->m_dspServiceConnectionInfo,
                                                                 m_pModule->getNetworkConfig()->m_tcpNetworkFactory);
    m_dspInterface->setClientSmart(m_dspClient);
    m_dspserverConnectState.addTransition(m_dspClient.get(), &Zera::ProxyClient::connected, &m_claimPGRMemState);
    connect(m_dspInterface.get(), &AbstractServerInterface::serverAnswer, this, &cDftModuleMeasProgram::catchInterfaceAnswer);
    Zera::Proxy::getInstance()->startConnectionSmart(m_dspClient);
}

void cDftModuleMeasProgram::claimPGRMem()
{
    // if we've got dsp server connection we set up our measure program and claim the resources
    setDspVarList(); // first we set the var list for our dsp
    setDspCmdList(); // and the cmd list he has to work on
    m_MsgNrCmdList[m_rmInterface.setResource("DSP1", "PGRMEMC", m_dspInterface->cmdListCount())] = claimpgrmem;
}


void cDftModuleMeasProgram::claimUSERMem()
{
   m_MsgNrCmdList[m_rmInterface.setResource("DSP1", "USERMEM", m_nDspMemUsed)] = claimusermem;
}


void cDftModuleMeasProgram::varList2DSP()
{
    m_MsgNrCmdList[m_dspInterface->varList2Dsp()] = varlist2dsp;
}


void cDftModuleMeasProgram::cmdList2DSP()
{
    m_MsgNrCmdList[m_dspInterface->cmdList2Dsp()] = cmdlist2dsp;
}


void cDftModuleMeasProgram::activateDSP()
{
    m_MsgNrCmdList[m_dspInterface->activateInterface()] = activatedsp; // aktiviert die var- und cmd-listen im dsp
}


void cDftModuleMeasProgram::activateDSPdone()
{
    m_bActive = true;

    setActualValuesNames();
    setSCPIMeasInfo();
    setRefChannelValidator();
    initRFieldMeasurement();

    m_pMeasureSignal->setValue(QVariant(1));
    connect(m_pIntegrationTimeParameter, &VfModuleParameter::sigValueChanged, this, &cDftModuleMeasProgram::newIntegrationtime);
    connect(m_pRefChannelParameter, &VfModuleParameter::sigValueChanged, this, &cDftModuleMeasProgram::newRefChannel);

    emit activated();
}

void cDftModuleMeasProgram::freePGRMem()
{
    m_dataAcquisitionMachine.stop();
    m_bActive = false;
    Zera::Proxy::getInstance()->releaseConnectionSmart(m_dspClient); // no async. messages anymore
    deleteDspCmdList();

    m_MsgNrCmdList[m_rmInterface.freeResource("DSP1", "PGRMEMC")] = freepgrmem;
}

void cDftModuleMeasProgram::freeUSERMem()
{
    m_MsgNrCmdList[m_rmInterface.freeResource("DSP1", "USERMEM")] = freeusermem;
}

void cDftModuleMeasProgram::deactivateDSPdone()
{
    disconnect(&m_rmInterface, 0, this, 0); // but we must disconnect this 2 manually
    disconnect(m_dspInterface.get(), 0, this, 0);
    emit deactivated();
}

void cDftModuleMeasProgram::dataAcquisitionDSP()
{
    m_pMeasureSignal->setValue(QVariant(0));
    if(m_bActive)
        m_MsgNrCmdList[m_dspInterface->dataAcquisition(m_pActualValuesDSP)] = dataaquistion; // we start our data aquisition now
}

void cDftModuleMeasProgram::turnVectorsToRefChannel()
{
    QHash<QString, std::complex<double>> dftActComplexValuesChannelHash;
    for (int i = 0; i < getConfData()->m_valueChannelList.count(); i++)
        dftActComplexValuesChannelHash[getConfData()->m_valueChannelList.at(i)] = std::complex<double>(m_ModuleActualValues[i*2], m_ModuleActualValues[i*2+1]);

    QString referenceChannelName = m_ChannelSystemNameHash.value(getConfData()->m_sRefChannel.m_sPar);
    std::complex<double> complexReferenceVector = dftActComplexValuesChannelHash.value(referenceChannelName);

    double tanRef = complexReferenceVector.imag() / complexReferenceVector.real();
    double divisor = sqrt(1.0+(tanRef * tanRef));
    // the turnvector has the negative reference angle
    // computing in complex is more acurate, but we have to keep in mind the
    // point of discontinuity of the arctan function
    std::complex<double> turnVector;
    if (complexReferenceVector.real() < 0)
        turnVector = std::complex<double>(-(1.0 / divisor), (tanRef / divisor));
    else
        turnVector = std::complex<double>((1.0 / divisor), -(tanRef / divisor));

    // this method is alternative ... but it is not so accurate as the above one
    //double phiRef = userAtan(complexReferenceVector.im(), complexReferenceVector.re());
    //complex turnVector = complex(cos(-phiRef*0.017453292), sin(-phiRef*0.017453292));

    for (int i = 0; i < getConfData()->m_valueChannelList.count(); i++) {
        QString key;
        key = getConfData()->m_valueChannelList.at(i);
        std::complex<double> newDft = dftActComplexValuesChannelHash.take(key);
        newDft *= turnVector;
        if (fabs(newDft.imag()) < 1e-8)
            newDft = std::complex<double>(newDft.real(), 0.0);
        dftActComplexValuesChannelHash[key] = newDft;
    }

    // now we have to compute the difference vectors and store all new values
    for (int i = 0; i < getConfData()->m_valueChannelList.count(); i++) {
        QString key = getConfData()->m_valueChannelList.at(i);
        QStringList sl = key.split('-');
        // we have 2 entries
        if (sl.count() == 2)
            dftActComplexValuesChannelHash[key] = dftActComplexValuesChannelHash[sl.at(0)] - dftActComplexValuesChannelHash[sl.at(1)];
        m_ModuleActualValues.replace(i*2, dftActComplexValuesChannelHash[key].real());
        m_ModuleActualValues.replace(i*2+1, dftActComplexValuesChannelHash[key].imag());
    }
}

void cDftModuleMeasProgram::dataReadDSP()
{
    if (m_bActive) {
        m_ModuleActualValues = m_pActualValuesDSP->getData();
        // dft(0) is a speciality. sin and cos in dsp are set so that we get amplitude rather than energy.
        // so dc is multiplied  by sqrt(2) * sqrt(2) = 2
        // used in COM5003 ref session
        if (getConfData()->m_nDftOrder == 0) {
            for (int i = 0; i < m_veinActValueList.count(); i++) {
                double re = m_ModuleActualValues[i*2] * 0.5;
                m_ModuleActualValues.replace(i*2, re);
            }
        }
        else {
            // as our dft produces math positive values, we correct them to technical positive values (im * -1)
            for (int i = 0; i < m_veinActValueList.count(); i++) {
                double im = m_ModuleActualValues[i*2+1] * -1.0;
                m_ModuleActualValues.replace(i*2+1, im);
            }
            if (getConfData()->m_bRefChannelOn)
                turnVectorsToRefChannel();
        }
        emit actualValues(&m_ModuleActualValues); // and send them
        m_pMeasureSignal->setValue(QVariant(1)); // signal measuring
    }
}

void cDftModuleMeasProgram::newIntegrationtime(QVariant ti)
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

void cDftModuleMeasProgram::newRefChannel(QVariant refchn)
{
    getConfData()->m_sRefChannel.m_sPar = refchn.toString();
    emit m_pModule->parameterChanged();
}

}
