#include "fftmodulemeasprogram.h"
#include "fftmoduleconfiguration.h"
#include <errormessages.h>
#include <reply.h>
#include <proxy.h>
#include <dspinterfacecmddecoder.h>
#include <doublevalidator.h>
#include <stringvalidator.h>
#include <math.h>
#include <timerfactoryqt.h>

namespace FFTMODULE
{

cFftModuleMeasProgram::cFftModuleMeasProgram(cFftModule* module, std::shared_ptr<cBaseModuleConfiguration> pConfiguration)
    :cBaseDspMeasProgram(pConfiguration), m_pModule(module)
{
    m_dspInterface = m_pModule->getServiceInterfaceFactory()->createDspInterfaceFft(getConfData()->m_valueChannelList, getConfData()->m_nFftOrder);

    m_IdentifyState.addTransition(this, &cFftModuleMeasProgram::activationContinue, &m_readResourceTypesState);
    m_readResourceTypesState.addTransition(this, &cFftModuleMeasProgram::activationContinue, &m_readResourceState);
    m_readResourceState.addTransition(this, &cFftModuleMeasProgram::activationContinue, &m_readResourceInfosState);
    m_readResourceInfosState.addTransition(this, &cFftModuleMeasProgram::activationContinue, &m_readResourceInfoState);
    m_readResourceInfoState.addTransition(this, &cFftModuleMeasProgram::activationContinue, &m_readResourceInfoDoneState);
    m_readResourceInfoDoneState.addTransition(this, &cFftModuleMeasProgram::activationContinue, &m_pcbserverConnectState);
    m_readResourceInfoDoneState.addTransition(this, &cFftModuleMeasProgram::activationLoop, &m_readResourceInfoState);
    m_pcbserverConnectState.addTransition(this, &cFftModuleMeasProgram::activationContinue, &m_readSampleRateState);
    m_readSampleRateState.addTransition(this, &cFftModuleMeasProgram::activationContinue, &m_readChannelInformationState);
    m_readChannelInformationState.addTransition(this, &cFftModuleMeasProgram::activationContinue, &m_readChannelAliasState);
    m_readChannelAliasState.addTransition(this, &cFftModuleMeasProgram::activationContinue, &m_readChannelUnitState);
    m_readChannelUnitState.addTransition(this, &cFftModuleMeasProgram::activationContinue, &m_readDspChannelState);
    m_readDspChannelState.addTransition(this, &cFftModuleMeasProgram::activationContinue, &m_readDspChannelDoneState);
    m_readDspChannelDoneState.addTransition(this, &cFftModuleMeasProgram::activationContinue, &m_dspserverConnectState);
    m_readDspChannelDoneState.addTransition(this, &cFftModuleMeasProgram::activationLoop, &m_readChannelAliasState);

    m_claimPGRMemState.addTransition(this, &cFftModuleMeasProgram::activationContinue, &m_claimUSERMemState);
    m_claimUSERMemState.addTransition(this, &cFftModuleMeasProgram::activationContinue, &m_var2DSPState);
    m_var2DSPState.addTransition(this, &cFftModuleMeasProgram::activationContinue, &m_cmd2DSPState);
    m_cmd2DSPState.addTransition(this, &cFftModuleMeasProgram::activationContinue, &m_activateDSPState);
    m_activateDSPState.addTransition(this, &cFftModuleMeasProgram::activationContinue, &m_loadDSPDoneState);

    m_activationMachine.addState(&m_resourceManagerConnectState);
    m_activationMachine.addState(&m_IdentifyState);
    m_activationMachine.addState(&m_readResourceTypesState);
    m_activationMachine.addState(&m_readResourceState);
    m_activationMachine.addState(&m_readResourceInfosState);
    m_activationMachine.addState(&m_readResourceInfoState);
    m_activationMachine.addState(&m_readResourceInfoDoneState);
    m_activationMachine.addState(&m_pcbserverConnectState);
    m_activationMachine.addState(&m_readSampleRateState);
    m_activationMachine.addState(&m_readChannelInformationState);
    m_activationMachine.addState(&m_readChannelAliasState);
    m_activationMachine.addState(&m_readChannelUnitState);
    m_activationMachine.addState(&m_readDspChannelState);
    m_activationMachine.addState(&m_readDspChannelDoneState);
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
    connect(&m_readResourceTypesState, &QState::entered, this, &cFftModuleMeasProgram::readResourceTypes);
    connect(&m_readResourceState, &QState::entered, this, &cFftModuleMeasProgram::readResource);
    connect(&m_readResourceInfosState, &QState::entered, this, &cFftModuleMeasProgram::readResourceInfos);
    connect(&m_readResourceInfoState, &QState::entered, this, &cFftModuleMeasProgram::readResourceInfo);
    connect(&m_readResourceInfoDoneState, &QState::entered, this, &cFftModuleMeasProgram::readResourceInfoDone);
    connect(&m_pcbserverConnectState, &QState::entered, this, &cFftModuleMeasProgram::pcbserverConnect);
    connect(&m_readSampleRateState, &QState::entered, this, &cFftModuleMeasProgram::readSampleRate);
    connect(&m_readChannelInformationState, &QState::entered, this, &cFftModuleMeasProgram::readChannelInformation);
    connect(&m_readChannelAliasState, &QState::entered, this, &cFftModuleMeasProgram::readChannelAlias);
    connect(&m_readChannelUnitState, &QState::entered, this, &cFftModuleMeasProgram::readChannelUnit);
    connect(&m_readDspChannelState, &QState::entered, this, &cFftModuleMeasProgram::readDspChannel);
    connect(&m_readDspChannelDoneState, &QState::entered, this, &cFftModuleMeasProgram::readDspChannelDone);
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

void cFftModuleMeasProgram::generateInterface()
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
    m_pIntegrationTimeParameter->setSCPIInfo(new cSCPIInfo("CONFIGURATION","TINTEGRATION", "10", "PAR_Interval", "0", "s"));

    m_pModule->m_veinModuleParameterMap[key] = m_pIntegrationTimeParameter; // for modules use

    cDoubleValidator *dValidator;
    dValidator = new cDoubleValidator(1.0, 100.0, 0.5);
    m_pIntegrationTimeParameter->setValidator(dValidator);

    m_pRefChannelParameter = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->m_pModuleValidator,
                                                      key = QString("PAR_RefChannel"),
                                                      QString("Reference channel"),
                                                      QVariant(getConfData()->m_RefChannel.m_sPar));

    cStringValidator *sValidator;
    sValidator = new cStringValidator(getConfData()->m_valueChannelList);
    m_pRefChannelParameter->setValidator(sValidator);

    m_pRefChannelParameter->setSCPIInfo(new cSCPIInfo("CONFIGURATION","REFCHANNEL", "10", "PAR_RefChannel", "0", ""));
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
    m_nfftLen = calcFftResultLenHalf(getConfData()->m_nFftOrder);
    // we fetch a handle for sampled data and other temporary values
    // global data segment is 1k words and lies on 1k boundary, so we put fftinput and fftouptut
    // at the beginning of that page because bitreversal adressing of fft only works properly if so
    m_pTmpDataDsp = m_dspInterface->getMemHandle("TmpData");
    m_pTmpDataDsp->addVarItem( new cDspVar("FFTINPUT", 2 * m_nfftLen, DSPDATA::vDspTempGlobal));
    m_pTmpDataDsp->addVarItem( new cDspVar("FFTOUTPUT", 2 * m_nfftLen, DSPDATA::vDspTempGlobal));
    // meassignal will also still fit in global mem ... so we save memory
    m_pTmpDataDsp->addVarItem( new cDspVar("MEASSIGNAL", 2 * m_nSRate, DSPDATA::vDspTemp));
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
    m_nDspMemUsed = m_pTmpDataDsp->getumemSize() + m_pParameterDSP->getSize() + m_pActualValuesDSP->getSize();
}


void cFftModuleMeasProgram::deleteDspVarList()
{
    m_dspInterface->deleteMemHandle(m_pTmpDataDsp);
    m_dspInterface->deleteMemHandle(m_pParameterDSP);
    m_dspInterface->deleteMemHandle(m_pActualValuesDSP);
}


void cFftModuleMeasProgram::setDspCmdList()
{
    QString s;

    m_dspInterface->addCycListItem( s = "STARTCHAIN(1,1,0x0101)"); // aktiv, prozessnr. (dummy),hauptkette 1 subkette 1 start
        m_dspInterface->addCycListItem( s = QString("CLEARN(%1,MEASSIGNAL)").arg(2*m_nSRate) ); // clear meassignal
        m_dspInterface->addCycListItem( s = QString("CLEARN(%1,FILTER)").arg(2 * 2 * m_nfftLen * m_veinActValueList.count()+1) ); // clear the whole filter incl. count
        if (getConfData()->m_bmovingWindow)
            m_dspInterface->addCycListItem( s = QString("SETVAL(TIPAR,%1)").arg(getConfData()->m_fmovingwindowInterval*1000.0)); // initial ti time
        else
            m_dspInterface->addCycListItem( s = QString("SETVAL(TIPAR,%1)").arg(getConfData()->m_fMeasInterval.m_fValue*1000.0)); // initial ti time
        m_dspInterface->addCycListItem( s = QString("SETVAL(REFCHN,%1)").arg(m_measChannelInfoHash.value(getConfData()->m_RefChannel.m_sPar).dspChannelNr));
        m_dspInterface->addCycListItem( s = "GETSTIME(TISTART)"); // einmal ti start setzen
        m_dspInterface->addCycListItem( s = "DEACTIVATECHAIN(1,0x0101)"); // ende prozessnr., hauptkette 1 subkette 1
        m_dspInterface->addCycListItem( s = QString("SETVAL(DEBUGCOUNT,0)"));
    m_dspInterface->addCycListItem( s = "STOPCHAIN(1,0x0101)"); // ende prozessnr., hauptkette 1 subkette 1

    // we compute the phase of our reference channel first
    m_dspInterface->addCycListItem( s = QString("COPYDATAIND(REFCHN,0,MEASSIGNAL)"));
    m_dspInterface->addCycListItem( s = QString("DFT(1,MEASSIGNAL,DFTREF)"));
    m_dspInterface->addCycListItem( s = QString("GENADR(MEASSIGNAL,DFTREF,IPOLADR)"));


    // next 3 commands for debug purpose , will be removed later
    //m_dspInterface->addCycListItem( s = "INC(DEBUGCOUNT)");
    //m_dspInterface->addCycListItem( s = "TESTVCSKIPLT(DEBUGCOUNT,1000)");
    //m_dspInterface->addCycListItem( s = "BREAK(1)");


    // we compute or copy our wanted actual values
    for (int i = 0; i < getConfData()->m_valueChannelList.count(); i++)
    {
        m_dspInterface->addCycListItem( s = QString("COPYDATA(CH%1,0,MEASSIGNAL)").arg(m_measChannelInfoHash.value(getConfData()->m_valueChannelList.at(i)).dspChannelNr) );
        m_dspInterface->addCycListItem( s = QString("COPYDATA(CH%1,0,MEASSIGNAL+%2)").arg(m_measChannelInfoHash.value(getConfData()->m_valueChannelList.at(i)).dspChannelNr).arg(m_nSRate));
        m_dspInterface->addCycListItem( s = QString("INTERPOLATIONIND(%1,IPOLADR,FFTINPUT)").arg(m_nfftLen));
        m_dspInterface->addCycListItem( s = QString("FFTREAL(%1,FFTINPUT,FFTOUTPUT)").arg(m_nfftLen));
        m_dspInterface->addCycListItem( s = QString("COPYMEM(%1,FFTOUTPUT,FFTXOUTPUT+%2)").arg(2 * m_nfftLen).arg(2 * m_nfftLen * i));
    }

    // and filter them
    m_dspInterface->addCycListItem( s = QString("AVERAGE1(%1,FFTXOUTPUT,FILTER)").arg(2 * m_nfftLen * m_veinActValueList.count())); // we add results to filter

    m_dspInterface->addCycListItem( s = "TESTTIMESKIPNEX(TISTART,TIPAR)");
    m_dspInterface->addCycListItem( s = "ACTIVATECHAIN(1,0x0102)");

    m_dspInterface->addCycListItem( s = "STARTCHAIN(0,1,0x0102)");
        m_dspInterface->addCycListItem( s = "GETSTIME(TISTART)"); // set new system time
        m_dspInterface->addCycListItem( s = QString("CMPAVERAGE1(%1,FILTER,VALXFFTF)").arg(2 * m_nfftLen * m_veinActValueList.count()));
        m_dspInterface->addCycListItem( s = QString("CLEARN(%1,FILTER)").arg(2 * 2 * m_nfftLen * m_veinActValueList.count()+1) );
        m_dspInterface->addCycListItem( s = QString("DSPINTTRIGGER(0x0,0x%1)").arg(0)); // send interrupt to module
        m_dspInterface->addCycListItem( s = "DEACTIVATECHAIN(1,0x0102)");
    m_dspInterface->addCycListItem( s = "STOPCHAIN(1,0x0102)"); // end processnr., mainchain 1 subchain 2
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
                if (reply == ack) // we only continue if resource manager acknowledges
                    emit activationContinue();
                else {
                    emit errMsg(tr(rmidentErrMSG));
                    emit activationError();
                }
                break;
            case claimpgrmem:
                if (reply == ack) // we only continue if resource manager acknowledges
                    emit activationContinue();
                else {
                    emit errMsg((tr(claimresourceErrMsg)));
                    emit activationError();
                }
                break;
            case claimusermem:
                if (reply == ack) // we only continue if resource manager acknowledges
                    emit activationContinue();
                else {
                    emit errMsg((tr(claimresourceErrMsg)));
                    emit activationError();
                }
                break;
            case varlist2dsp:
                if (reply == ack) // we only continue if resource manager acknowledges
                    emit activationContinue();
                else {
                    emit errMsg((tr(dspvarlistwriteErrMsg)));
                    emit activationError();
                }
                break;
            case cmdlist2dsp:
                if (reply == ack) // we only continue if resource manager acknowledges
                    emit activationContinue();
                else {
                    emit errMsg((tr(dspcmdlistwriteErrMsg)));
                    emit activationError();
                }
                break;
            case activatedsp:
                if (reply == ack) // we only continue if resource manager acknowledges
                    emit activationContinue();
                else {
                    emit errMsg((tr(dspactiveErrMsg)));
                    emit activationError();
                }
                break;

            case readresourcetypes:
                if ((reply == ack) && (answer.toString().contains("SENSE")))
                    emit activationContinue();
                else {
                    emit errMsg((tr(resourcetypeErrMsg)));
                    emit activationError();
                }
                break;

            case readresource:
                if (reply == ack) {
                    QList<QString> sl = m_measChannelInfoHash.keys();
                    QString s = answer.toString();
                    bool allfound = true;
                    for (int i = 0; i < sl.count(); i++) {
                        if (!s.contains(sl.at(i)))
                            allfound = false;
                    }
                    if (allfound)
                        emit activationContinue();
                    else {
                        emit errMsg((tr(resourceErrMsg)));
                        emit activationError();
                    }
                }
                else {
                    emit errMsg((tr(resourceErrMsg)));
                    emit activationError();
                }
                break;

            case readresourceinfo:
            {
                QStringList sl = answer.toString().split(';');
                if ((reply == ack) && (sl.length() >= 4)) {
                    bool ok;
                    int port = sl.at(3).toInt(&ok); // we have to set the port where we can find our meas channel
                    if (ok) {
                        cMeasChannelInfo mi = m_measChannelInfoHash.take(channelInfoRead);
                        mi.pcbServersocket.m_nPort = port;
                        m_measChannelInfoHash[channelInfoRead] = mi;
                        emit activationContinue();
                    }
                    else {
                        emit errMsg((tr(resourceInfoErrMsg)));
                        emit activationError();
                    }
                }
                else {
                    emit errMsg((tr(resourceInfoErrMsg)));
                    emit activationError();
                }
                break;
            }

            case readsamplerate:
                if (reply == ack) {
                    m_nSRate = answer.toInt();
                    emit activationContinue();
                }
                else {
                    emit errMsg((tr(readsamplerateErrMsg)));
                    emit activationError();
                }
                break;

            case readalias:
                if (reply == ack) {
                    QString alias = answer.toString();
                    cMeasChannelInfo mi = m_measChannelInfoHash.take(channelInfoRead);
                    mi.alias = alias;
                    m_measChannelInfoHash[channelInfoRead] = mi;
                    emit activationContinue();
                }
                else {
                    emit errMsg((tr(readaliasErrMsg)));
                    emit activationError();
                }
                break;

            case readunit:
                if (reply == ack) {
                    QString unit = answer.toString();
                    cMeasChannelInfo mi = m_measChannelInfoHash.take(channelInfoRead);
                    mi.unit = unit;
                    m_measChannelInfoHash[channelInfoRead] = mi;
                    emit activationContinue();
                }
                else
                {
                    emit errMsg((tr(readunitErrMsg)));
                    emit activationError();
                }
                break;

            case readdspchannel:
                if (reply == ack) {
                    int chnnr = answer.toInt();
                    cMeasChannelInfo mi = m_measChannelInfoHash.take(channelInfoRead);
                    mi.dspChannelNr = chnnr;
                    m_measChannelInfoHash[channelInfoRead] = mi;
                    emit activationContinue();
                }
                else {
                    emit errMsg((tr(readdspchannelErrMsg)));
                    emit activationError();
                }
                break;

            case writeparameter:
                if (reply == ack) // we ignore ack
                    ;
                else {
                    emit errMsg((tr(writedspmemoryErrMsg)));
                    emit executionError();
                }
                break;

            case deactivatedsp:
                if (reply == ack) // we only continue if resource manager acknowledges
                    emit deactivationContinue();
                else {
                    emit errMsg((tr(dspdeactiveErrMsg)));
                    emit deactivationError();
                }
                break;
            case freepgrmem:
                if (reply == ack) // we only continue if resource manager acknowledges
                    emit deactivationContinue();
                else {
                    emit errMsg((tr(freeresourceErrMsg)));
                    emit deactivationError();
                }
                break;
            case freeusermem:
                if (reply == ack) // we only continue if resource manager acknowledges
                    emit deactivationContinue();
                else {
                    emit errMsg((tr(freeresourceErrMsg)));
                    emit deactivationError();
                }
                break;

            case dataaquistion:
                if (reply == ack)
                    emit dataAquisitionContinue();
                else {
                    m_dataAcquisitionMachine.stop();
                    emit errMsg((tr(dataaquisitionErrMsg)));
                    emit executionError(); // but we send error message
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
    cSCPIInfo* pSCPIInfo;

    for (int i = 0; i < getConfData()->m_valueChannelList.count(); i++)
    {
        pSCPIInfo = new cSCPIInfo("MEASURE", m_veinActValueList.at(i)->getChannelName(), "8", m_veinActValueList.at(i)->getName(), "0", m_veinActValueList.at(i)->getUnit());
        m_veinActValueList.at(i)->setSCPIInfo(pSCPIInfo);
        pSCPIInfo = new cSCPIInfo("MEASURE", m_DCValueList.at(i)->getChannelName() + "_DC", "8", m_DCValueList.at(i)->getName(), "0", m_DCValueList.at(i)->getUnit());
        m_DCValueList.at(i)->setSCPIInfo(pSCPIInfo);
    }
}


void cFftModuleMeasProgram::setActualValuesNames()
{
    for (int i = 0; i < getConfData()->m_valueChannelList.count(); i++)
    {
        QStringList sl = getConfData()->m_valueChannelList.at(i).split('-');
        QString s, name;
        QString s1,s2,s3,s4;
        // we have 1 or 2 entries for each value
        s1 = s2 = m_measChannelInfoHash.value(sl.at(0)).alias;
        s1.remove(QRegExp("[1-9][0-9]?"));
        s2.remove(s1);

        if (sl.count() == 1)
        {
            s = s1 + "%1" + QString(";%1;[%2]").arg(s2).arg(m_measChannelInfoHash.value(sl.at(0)).unit);
            name = s1 + s2; // we still have to clarify translation solution
        }
        else
        {
            s3 = s4 = m_measChannelInfoHash.value(sl.at(1)).alias;
            s3.remove(QRegExp("[1-9][0-9]?"));
            s4.remove(s3);
            s = s1 + "%1-" + s3 + "%2" + QString(";%1;%2;[%3]").arg(s2).arg(s4).arg(m_measChannelInfoHash.value(sl.at(0)).unit);
            name = s1 + s2 + "-" + s3 +s4; // dito
        }

        m_veinActValueList.at(i)->setChannelName(name);
        m_veinActValueList.at(i)->setUnit(m_measChannelInfoHash.value(sl.at(0)).unit);
        m_DCValueList.at(i)->setChannelName(name);
        m_DCValueList.at(i)->setUnit(m_measChannelInfoHash.value(sl.at(0)).unit);
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
    mi.pcbServersocket = getConfData()->m_PCBServerSocket; // the default from configuration file
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
    m_rmClient = Zera::Proxy::getInstance()->getConnectionSmart(getConfData()->m_RMSocket.m_sIP, getConfData()->m_RMSocket.m_nPort);
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


void cFftModuleMeasProgram::readResourceTypes()
{
    m_MsgNrCmdList[m_rmInterface.getResourceTypes()] = readresourcetypes;
}


void cFftModuleMeasProgram::readResource()
{
    m_MsgNrCmdList[m_rmInterface.getResources("SENSE")] = readresource;
}


void cFftModuleMeasProgram::readResourceInfos()
{
    channelInfoReadList = m_measChannelInfoHash.keys(); // we have to read information for all channels in this list
    emit activationContinue();
}


void cFftModuleMeasProgram::readResourceInfo()
{
    channelInfoRead = channelInfoReadList.takeLast();
    m_MsgNrCmdList[m_rmInterface.getResourceInfo("SENSE", channelInfoRead)] = readresourceinfo;
}


void cFftModuleMeasProgram::readResourceInfoDone()
{
    if (channelInfoReadList.isEmpty())
        emit activationContinue();
    else
        emit activationLoop();
}


void cFftModuleMeasProgram::pcbserverConnect()
{
    m_pcbClient = Zera::Proxy::getInstance()->getConnectionSmart(getConfData()->m_PCBServerSocket.m_sIP, getConfData()->m_PCBServerSocket.m_nPort);
    m_pcbInterface->setClientSmart(m_pcbClient);
    connect(m_pcbClient.get(), &Zera::ProxyClient::connected, this, &cBaseMeasProgram::activationContinue);
    connect(m_pcbInterface.get(), &AbstractServerInterface::serverAnswer, this, &cFftModuleMeasProgram::catchInterfaceAnswer);
    Zera::Proxy::getInstance()->startConnectionSmart(m_pcbClient);
}


void cFftModuleMeasProgram::readSampleRate()
{
    m_MsgNrCmdList[m_pcbInterface->getSampleRate()] = readsamplerate;
}


void cFftModuleMeasProgram::readChannelInformation()
{
    channelInfoReadList = m_measChannelInfoHash.keys(); // we have to read information for all channels in this list
    emit activationContinue();
}


void cFftModuleMeasProgram::readChannelAlias()
{
    channelInfoRead = channelInfoReadList.takeFirst();
    m_MsgNrCmdList[m_pcbInterface->getAlias(channelInfoRead)] = readalias;
}


void cFftModuleMeasProgram::readChannelUnit()
{
    m_MsgNrCmdList[m_pcbInterface->getUnit(channelInfoRead)] = readunit;
}


void cFftModuleMeasProgram::readDspChannel()
{
    m_MsgNrCmdList[m_pcbInterface->getDSPChannel(channelInfoRead)] = readdspchannel;
}


void cFftModuleMeasProgram::readDspChannelDone()
{
    if (channelInfoReadList.isEmpty())
        emit activationContinue();
    else
        emit activationLoop();
}


void cFftModuleMeasProgram::dspserverConnect()
{
    m_dspClient = Zera::Proxy::getInstance()->getConnectionSmart(getConfData()->m_DSPServerSocket.m_sIP, getConfData()->m_DSPServerSocket.m_nPort);
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
        DspInterfaceCmdDecoder::setVarData(m_pParameterDSP, QString("TIPAR:%1;TISTART:%2;").arg(getConfData()->m_fMeasInterval.m_fValue*1000)
                                                                            .arg(0), DSPDATA::dInt);
        m_MsgNrCmdList[m_dspInterface->dspMemoryWrite(m_pParameterDSP)] = writeparameter;
    }

    emit m_pModule->parameterChanged();
}


void cFftModuleMeasProgram::newRefChannel(QVariant chn)
{
    getConfData()->m_RefChannel.m_sPar = chn.toString();
    DspInterfaceCmdDecoder::setVarData(m_pParameterDSP, QString("REFCHN:%1;").arg(m_measChannelInfoHash.value(getConfData()->m_RefChannel.m_sPar).dspChannelNr));
    m_MsgNrCmdList[m_dspInterface->dspMemoryWrite(m_pParameterDSP)] = writeparameter;
    emit m_pModule->parameterChanged();
}
}
