#include <QString>
#include <QStateMachine>
#include <rminterface.h>
#include <dspinterface.h>
#include <pcbinterface.h>
#include <movingwindowfilter.h>
#include <proxy.h>
#include <proxyclient.h>
#include <veinpeer.h>
#include <veinentity.h>
#include <QPointF>
#include <math.h>

#include "debug.h"
#include "errormessages.h"
#include "reply.h"
#include "modulesignal.h"
#include "moduleinfo.h"
#include "moduleparameter.h"
#include "fftmodule.h"
#include "fftmoduleconfigdata.h"
#include "fftmodulemeasprogram.h"

namespace FFTMODULE
{

cFftModuleMeasProgram::cFftModuleMeasProgram(cFftModule* module, Zera::Proxy::cProxy* proxy, VeinPeer* peer, cFftModuleConfigData& configdata)
    :cBaseDspMeasProgram(proxy, peer), m_pModule(module), m_ConfigData(configdata)
{
    m_pRMInterface = new Zera::Server::cRMInterface();
    m_pDSPInterFace = new Zera::Server::cDSPInterface();
    m_pMovingwindowFilter = new cMovingwindowFilter(1.0);

    m_ActValueList = m_ConfigData.m_valueChannelList;

    m_IdentifyState.addTransition(this, SIGNAL(activationContinue()), &m_readResourceTypesState);
    m_readResourceTypesState.addTransition(this, SIGNAL(activationContinue()), &m_readResourceState);
    m_readResourceState.addTransition(this, SIGNAL(activationContinue()), &m_readResourceInfosState);
    m_readResourceInfosState.addTransition(this, SIGNAL(activationContinue()), &m_readResourceInfoState);
    m_readResourceInfoState.addTransition(this, SIGNAL(activationContinue()), &m_readResourceInfoDoneState);
    m_readResourceInfoDoneState.addTransition(this, SIGNAL(activationContinue()), &m_pcbserverConnectState);
    m_readResourceInfoDoneState.addTransition(this, SIGNAL(activationLoop()), &m_readResourceInfoState);
    m_pcbserverConnectState.addTransition(this, SIGNAL(activationContinue()), &m_readSampleRateState);
    m_readSampleRateState.addTransition(this, SIGNAL(activationContinue()), &m_readChannelInformationState);
    m_readChannelInformationState.addTransition(this, SIGNAL(activationContinue()), &m_readChannelAliasState);
    m_readChannelAliasState.addTransition(this, SIGNAL(activationContinue()), &m_readChannelUnitState);
    m_readChannelUnitState.addTransition(this, SIGNAL(activationContinue()), &m_readDspChannelState);
    m_readDspChannelState.addTransition(this, SIGNAL(activationContinue()), &m_readDspChannelDoneState);
    m_readDspChannelDoneState.addTransition(this, SIGNAL(activationContinue()), &m_dspserverConnectState);
    m_readDspChannelDoneState.addTransition(this, SIGNAL(activationLoop()), &m_readChannelAliasState);

    m_claimPGRMemState.addTransition(this, SIGNAL(activationContinue()), &m_claimUSERMemState);
    m_claimUSERMemState.addTransition(this, SIGNAL(activationContinue()), &m_var2DSPState);
    m_var2DSPState.addTransition(this, SIGNAL(activationContinue()), &m_cmd2DSPState);
    m_cmd2DSPState.addTransition(this, SIGNAL(activationContinue()), &m_activateDSPState);
    m_activateDSPState.addTransition(this, SIGNAL(activationContinue()), &m_loadDSPDoneState);

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

    connect(&m_resourceManagerConnectState, SIGNAL(entered()), SLOT(resourceManagerConnect()));
    connect(&m_IdentifyState, SIGNAL(entered()), SLOT(sendRMIdent()));
    connect(&m_readResourceTypesState, SIGNAL(entered()), SLOT(readResourceTypes()));
    connect(&m_readResourceState, SIGNAL(entered()), SLOT(readResource()));
    connect(&m_readResourceInfosState, SIGNAL(entered()), SLOT(readResourceInfos()));
    connect(&m_readResourceInfoState, SIGNAL(entered()), SLOT(readResourceInfo()));
    connect(&m_readResourceInfoDoneState, SIGNAL(entered()), SLOT(readResourceInfoDone()));
    connect(&m_pcbserverConnectState, SIGNAL(entered()), SLOT(pcbserverConnect()));
    connect(&m_readSampleRateState, SIGNAL(entered()), SLOT(readSampleRate()));
    connect(&m_readChannelInformationState, SIGNAL(entered()), SLOT(readChannelInformation()));
    connect(&m_readChannelAliasState, SIGNAL(entered()), SLOT(readChannelAlias()));
    connect(&m_readChannelUnitState, SIGNAL(entered()), SLOT(readChannelUnit()));
    connect(&m_readDspChannelState, SIGNAL(entered()), SLOT(readDspChannel()));
    connect(&m_readDspChannelDoneState, SIGNAL(entered()), SLOT(readDspChannelDone()));
    connect(&m_dspserverConnectState, SIGNAL(entered()), SLOT(dspserverConnect()));
    connect(&m_claimPGRMemState, SIGNAL(entered()), SLOT(claimPGRMem()));
    connect(&m_claimUSERMemState, SIGNAL(entered()), SLOT(claimUSERMem()));
    connect(&m_var2DSPState, SIGNAL(entered()), SLOT(varList2DSP()));
    connect(&m_cmd2DSPState, SIGNAL(entered()), SLOT(cmdList2DSP()));
    connect(&m_activateDSPState, SIGNAL(entered()), SLOT(activateDSP()));
    connect(&m_loadDSPDoneState, SIGNAL(entered()), SLOT(activateDSPdone()));

    // setting up statemachine for unloading dsp and setting resources free
    m_deactivateDSPState.addTransition(this, SIGNAL(deactivationContinue()), &m_freePGRMemState);
    m_freePGRMemState.addTransition(this, SIGNAL(deactivationContinue()), &m_freeUSERMemState);
    m_freeUSERMemState.addTransition(this, SIGNAL(deactivationContinue()), &m_unloadDSPDoneState);
    m_deactivationMachine.addState(&m_deactivateDSPState);
    m_deactivationMachine.addState(&m_freePGRMemState);
    m_deactivationMachine.addState(&m_freeUSERMemState);
    m_deactivationMachine.addState(&m_unloadDSPDoneState);

    m_deactivationMachine.setInitialState(&m_deactivateDSPState);

    connect(&m_deactivateDSPState, SIGNAL(entered()), SLOT(deactivateDSP()));
    connect(&m_freePGRMemState, SIGNAL(entered()), SLOT(freePGRMem()));
    connect(&m_freeUSERMemState, SIGNAL(entered()), SLOT(freeUSERMem()));
    connect(&m_unloadDSPDoneState, SIGNAL(entered()), SLOT(deactivateDSPdone()));

    // setting up statemachine for data acquisition
    m_dataAcquisitionState.addTransition(this, SIGNAL(dataAquisitionContinue()), &m_dataAcquisitionDoneState);
    m_dataAcquisitionMachine.addState(&m_dataAcquisitionState);
    m_dataAcquisitionMachine.addState(&m_dataAcquisitionDoneState);
    m_dataAcquisitionMachine.setInitialState(&m_dataAcquisitionState);
    connect(&m_dataAcquisitionState, SIGNAL(entered()), SLOT(dataAcquisitionDSP()));
    connect(&m_dataAcquisitionDoneState, SIGNAL(entered()), SLOT(dataReadDSP()));
}


cFftModuleMeasProgram::~cFftModuleMeasProgram()
{
    delete m_pRMInterface;
    delete m_pDSPInterFace;
    delete m_pMovingwindowFilter;
}


void cFftModuleMeasProgram::start()
{
    if (m_ConfigData.m_bmovingWindow)
    {
        m_pMovingwindowFilter->setIntegrationtime(m_ConfigData.m_fMeasInterval.m_fValue);
        connect(this, SIGNAL(actualValues(QVector<float>*)), m_pMovingwindowFilter, SLOT(receiveActualValues(QVector<float>*)));
        connect(m_pMovingwindowFilter, SIGNAL(actualValues(QVector<float>*)), this, SLOT(setInterfaceActualValues(QVector<float>*)));
    }
    else
        connect(this, SIGNAL(actualValues(QVector<float>*)), this, SLOT(setInterfaceActualValues(QVector<float>*)));
}


void cFftModuleMeasProgram::stop()
{
    disconnect(this, SIGNAL(actualValues(QVector<float>*)), this, 0);
    disconnect(m_pMovingwindowFilter, 0, 0, 0);
}


void cFftModuleMeasProgram::generateInterface()
{
    VeinEntity* p_entity;
    QString s;

    // this here is for translation purpose
    s = tr("UL%1;[V]");
    s = tr("IL%1;[A]");
    s = tr("REF%1;[V]");

    int n = m_ActValueList.count();

    for (int i = 0; i < n; i++)
    {

        s = QString("TRA_FFT%1Name").arg(i+1);
        p_entity = m_pPeer->dataAdd(s);
        p_entity->modifiersAdd(VeinEntity::MOD_READONLY);
        p_entity->modifiersAdd(VeinEntity::MOD_NOECHO);
        p_entity->setValue(QVariant("Unknown"), m_pPeer);
        m_EntityNameList.append(p_entity);

        s = QString("ACT_FFT%1").arg(i+1);
        p_entity = m_pPeer->dataAdd(s);
        p_entity->modifiersAdd(VeinEntity::MOD_READONLY);
        p_entity->modifiersAdd(VeinEntity::MOD_NOECHO);
        p_entity->setValue(QVariant((double) 0.0), m_pPeer);
        m_EntityActValueList.append(p_entity);
    }

    m_pRefChannelParameter = new cModuleParameter(m_pPeer, "PAR_REFCHANNEL", m_ConfigData.m_RefChannel.m_sPar);
    m_pRefChannelInfo = new cModuleInfo(m_pPeer, "INF_REFCHANNELLIST",QVariant(m_ConfigData.m_valueChannelList));
    m_pFftOrderInfo = new cModuleInfo(m_pPeer, "INF_FFTOrder", QVariant(m_ConfigData.m_nFftOrder));
    m_pIntegrationTimeParameter = new cModuleParameter(m_pPeer, "PAR_INTEGRATIONTIME", QVariant((double) m_ConfigData.m_fMeasInterval.m_fValue));
    m_pInfIntegrationTimeLimits = new cModuleInfo(m_pPeer, "PAR_INTEGRATIONTIME_LIMITS", QVariant(QString("%1;%2").arg(0.1).arg(100.0)));
    m_pMeasureSignal = new cModuleSignal(m_pPeer, "SIG_MEASURING", QVariant(0));
}


void cFftModuleMeasProgram::deleteInterface()
{
    for (int i = 0; i < m_EntityNameList.count(); i++)
        m_pPeer->dataRemove(m_EntityNameList.at(i));
    for (int i = 0; i < m_EntityActValueList.count(); i++)
        m_pPeer->dataRemove(m_EntityActValueList.at(i));

    m_EntityNameList.clear();
    m_EntityActValueList.clear();

    delete m_pFftOrderInfo;
    delete m_pIntegrationTimeParameter;
    delete m_pInfIntegrationTimeLimits;
    delete m_pMeasureSignal;
    delete m_pRefChannelParameter;
    delete m_pRefChannelInfo;
}


void cFftModuleMeasProgram::setDspVarList()
{
    m_nfftLen = 2 << (int)(floor(log((m_ConfigData.m_nFftOrder << 1) -1)/log(2.0))); // our fft length
    if (m_nfftLen < 32)
        m_nfftLen = m_nfftLen << 1; // minimum fftlen is 32 !!!!
    // we fetch a handle for sampled data and other temporary values
    // global data segment is 1k words and lies on 1k boundary, so we put fftinput and fftouptut
    // at the beginning of that page because bitreversal adressing of fft only works properly if so
    m_pTmpDataDsp = m_pDSPInterFace->getMemHandle("TmpData");
    m_pTmpDataDsp->addVarItem( new cDspVar("FFTINPUT", 2 * m_nfftLen, DSPDATA::vDspTempGlobal));
    m_pTmpDataDsp->addVarItem( new cDspVar("FFTOUTPUT", 2 * m_nfftLen, DSPDATA::vDspTempGlobal));
    // meassignal will also still fit in global mem ... so we save memory
    m_pTmpDataDsp->addVarItem( new cDspVar("MEASSIGNAL", 2 * m_nSRate, DSPDATA::vDspTemp));
    m_pTmpDataDsp->addVarItem( new cDspVar("FFTXOUTPUT", 2 * m_nfftLen * m_ActValueList.count(), DSPDATA::vDspTemp));
    m_pTmpDataDsp->addVarItem( new cDspVar("FILTER", 2 * 2 * m_nfftLen * m_ActValueList.count(),DSPDATA::vDspTemp));
    m_pTmpDataDsp->addVarItem( new cDspVar("N",1,DSPDATA::vDspTemp));
    m_pTmpDataDsp->addVarItem( new cDspVar("IPOLADR", 1, DSPDATA::vDspTemp, DSPDATA::dInt));
    m_pTmpDataDsp->addVarItem( new cDspVar("DFTREF", 2, DSPDATA::vDspTemp));
    m_pTmpDataDsp->addVarItem(new cDspVar("DEBUGCOUNT",1,DSPDATA::vDspTemp, DSPDATA::dInt));

    // a handle for parameter
    m_pParameterDSP =  m_pDSPInterFace->getMemHandle("Parameter");
    m_pParameterDSP->addVarItem( new cDspVar("TIPAR",1, DSPDATA::vDspParam, DSPDATA::dInt)); // integrationtime res = 1ms
    // we use tistart as parameter, so we can finish actual measuring interval bei setting 0
    m_pParameterDSP->addVarItem( new cDspVar("TISTART",1, DSPDATA::vDspTemp, DSPDATA::dInt));
    m_pParameterDSP->addVarItem( new cDspVar("REFCHN",1, DSPDATA::vDspParam, DSPDATA::dInt));

    // and one for filtered actual values
    m_pActualValuesDSP = m_pDSPInterFace->getMemHandle("ActualValues");
    m_pActualValuesDSP->addVarItem( new cDspVar("VALXFFTF", 2 * m_nfftLen * m_ActValueList.count(), DSPDATA::vDspResult));

    m_ModuleActualValues.resize(m_pActualValuesDSP->getSize()); // we provide a vector for generated actual values
    m_FFTModuleActualValues.resize(m_ActValueList.count() * m_ConfigData.m_nFftOrder * 2);
    m_nDspMemUsed = m_pTmpDataDsp->getumemSize() + m_pParameterDSP->getSize() + m_pActualValuesDSP->getSize();
}


void cFftModuleMeasProgram::deleteDspVarList()
{
    m_pDSPInterFace->deleteMemHandle(m_pTmpDataDsp);
    m_pDSPInterFace->deleteMemHandle(m_pParameterDSP);
    m_pDSPInterFace->deleteMemHandle(m_pActualValuesDSP);
}


void cFftModuleMeasProgram::setDspCmdList()
{
    QString s;

    m_pDSPInterFace->addCycListItem( s = "STARTCHAIN(1,1,0x0101)"); // aktiv, prozessnr. (dummy),hauptkette 1 subkette 1 start
        m_pDSPInterFace->addCycListItem( s = QString("CLEARN(%1,MEASSIGNAL)").arg(2*m_nSRate) ); // clear meassignal
        m_pDSPInterFace->addCycListItem( s = QString("CLEARN(%1,FILTER)").arg(2 * 2 * m_nfftLen * m_ActValueList.count()+1) ); // clear the whole filter incl. count
        if (m_ConfigData.m_bmovingWindow)
            m_pDSPInterFace->addCycListItem( s = QString("SETVAL(TIPAR,%1)").arg(m_ConfigData.m_fmovingwindowInterval*1000.0)); // initial ti time
        else
            m_pDSPInterFace->addCycListItem( s = QString("SETVAL(TIPAR,%1)").arg(m_ConfigData.m_fMeasInterval.m_fValue*1000.0)); // initial ti time
        m_pDSPInterFace->addCycListItem( s = QString("SETVAL(REFCHN,%1)").arg(m_measChannelInfoHash.value(m_ConfigData.m_RefChannel.m_sPar).dspChannelNr));
        m_pDSPInterFace->addCycListItem( s = "GETSTIME(TISTART)"); // einmal ti start setzen
        m_pDSPInterFace->addCycListItem( s = "DEACTIVATECHAIN(1,0x0101)"); // ende prozessnr., hauptkette 1 subkette 1
        m_pDSPInterFace->addCycListItem( s = QString("SETVAL(DEBUGCOUNT,0)"));
    m_pDSPInterFace->addCycListItem( s = "STOPCHAIN(1,0x0101)"); // ende prozessnr., hauptkette 1 subkette 1

    // we compute the phase of our reference channel first
    m_pDSPInterFace->addCycListItem( s = QString("COPYDATAIND(REFCHN,0,MEASSIGNAL)"));
    m_pDSPInterFace->addCycListItem( s = QString("DFT(1,MEASSIGNAL,DFTREF)"));
    m_pDSPInterFace->addCycListItem( s = QString("GENADR(MEASSIGNAL,DFTREF,IPOLADR)"));


    // next 3 commands for debug purpose , will be removed later
    //m_pDSPInterFace->addCycListItem( s = "INC(DEBUGCOUNT)");
    //m_pDSPInterFace->addCycListItem( s = "TESTVCSKIPLT(DEBUGCOUNT,1000)");
    //m_pDSPInterFace->addCycListItem( s = "BREAK(1)");


    // we compute or copy our wanted actual values
    for (int i = 0; i < m_ActValueList.count(); i++)
    {
        m_pDSPInterFace->addCycListItem( s = QString("COPYDATA(CH%1,0,MEASSIGNAL)").arg(m_measChannelInfoHash.value(m_ActValueList.at(i)).dspChannelNr));
        m_pDSPInterFace->addCycListItem( s = QString("COPYDATA(CH%1,0,MEASSIGNAL+%2)").arg(m_measChannelInfoHash.value(m_ActValueList.at(i)).dspChannelNr).arg(m_nSRate));
        m_pDSPInterFace->addCycListItem( s = QString("INTERPOLATIONIND(%1,IPOLADR,FFTINPUT)").arg(m_nfftLen));
        m_pDSPInterFace->addCycListItem( s = QString("FFTREAL(%1,FFTINPUT,FFTOUTPUT)").arg(m_nfftLen));
        m_pDSPInterFace->addCycListItem( s = QString("COPYMEM(%1,FFTOUTPUT,FFTXOUTPUT+%2)").arg(2 * m_nfftLen).arg(2 * m_nfftLen * i));
    }

    // and filter them
    m_pDSPInterFace->addCycListItem( s = QString("AVERAGE1(%1,FFTXOUTPUT,FILTER)").arg(2 * m_nfftLen * m_ActValueList.count())); // we add results to filter

    m_pDSPInterFace->addCycListItem( s = "TESTTIMESKIPNEX(TISTART,TIPAR)");
    m_pDSPInterFace->addCycListItem( s = "ACTIVATECHAIN(1,0x0102)");

    m_pDSPInterFace->addCycListItem( s = "STARTCHAIN(0,1,0x0102)");
        m_pDSPInterFace->addCycListItem( s = "GETSTIME(TISTART)"); // set new system time
        m_pDSPInterFace->addCycListItem( s = QString("CMPAVERAGE1(%1,FILTER,VALXFFTF)").arg(2 * m_nfftLen * m_ActValueList.count()));
        m_pDSPInterFace->addCycListItem( s = QString("CLEARN(%1,FILTER)").arg(2 * 2 * m_nfftLen * m_ActValueList.count()+1) );
        m_pDSPInterFace->addCycListItem( s = QString("DSPINTTRIGGER(0x0,0x%1)").arg(irqNr)); // send interrupt to module
        m_pDSPInterFace->addCycListItem( s = "DEACTIVATECHAIN(1,0x0102)");
    m_pDSPInterFace->addCycListItem( s = "STOPCHAIN(1,0x0102)"); // end processnr., mainchain 1 subchain 2
}


void cFftModuleMeasProgram::deleteDspCmdList()
{
    m_pDSPInterFace->clearCmdList();
}


void cFftModuleMeasProgram::catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer)
{
    bool ok;

    if (msgnr == 0) // 0 was reserved for async. messages
    {
        QString sintnr;
        // qDebug() << "meas program interrupt";
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
                if (reply == ack) // we only continue if resource manager acknowledges
                    emit activationContinue();
                else
                {
                    emit errMsg(tr(rmidentErrMSG));
#ifdef DEBUG
                    qDebug() << rmidentErrMSG;
#endif
                    emit activationError();
                }
                break;
            case claimpgrmem:
                if (reply == ack) // we only continue if resource manager acknowledges
                    emit activationContinue();
                else
                {
                    emit errMsg((tr(claimresourceErrMsg)));
#ifdef DEBUG
                    qDebug() << claimresourceErrMsg;
#endif
                    emit activationError();
                }
                break;
            case claimusermem:
                if (reply == ack) // we only continue if resource manager acknowledges
                    emit activationContinue();
                else
                {
                    emit errMsg((tr(claimresourceErrMsg)));
#ifdef DEBUG
                    qDebug() << claimresourceErrMsg;
#endif
                    emit activationError();
                }
                break;
            case varlist2dsp:
                if (reply == ack) // we only continue if resource manager acknowledges
                    emit activationContinue();
                else
                {
                    emit errMsg((tr(dspvarlistwriteErrMsg)));
#ifdef DEBUG
                    qDebug() << dspvarlistwriteErrMsg;
#endif
                    emit activationError();
                }
                break;
            case cmdlist2dsp:
                if (reply == ack) // we only continue if resource manager acknowledges
                    emit activationContinue();
                else
                {
                    emit errMsg((tr(dspcmdlistwriteErrMsg)));
#ifdef DEBUG
                    qDebug() << dspcmdlistwriteErrMsg;
#endif
                    emit activationError();
                }
                break;
            case activatedsp:
                if (reply == ack) // we only continue if resource manager acknowledges
                    emit activationContinue();
                else
                {
                    emit errMsg((tr(dspactiveErrMsg)));
#ifdef DEBUG
                    qDebug() << dspactiveErrMsg;
#endif
                    emit activationError();
                }
                break;

            case readresourcetypes:
                if ((reply == ack) && (answer.toString().contains("SENSE")))
                    emit activationContinue();
                else
                {
                    emit errMsg((tr(resourcetypeErrMsg)));
#ifdef DEBUG
                    qDebug() << resourcetypeErrMsg;
#endif
                    emit activationError();
                }
                break;

            case readresource:
            {
                if (reply == ack)
                {
                    bool allfound = true;
                    QList<QString> sl = m_measChannelInfoHash.keys();
                    QString s = answer.toString();
                    for (int i = 0; i < sl.count(); i++)
                    {
                        if (!s.contains(sl.at(i)))
                            allfound = false;
                    }

                    if (allfound)
                        emit activationContinue();
                    else
                    {
                        emit errMsg((tr(resourceErrMsg)));
#ifdef DEBUG
                        qDebug() << resourceErrMsg;
#endif
                        emit activationError();
                    }
                }
                else
                {
                    emit errMsg((tr(resourceErrMsg)));
#ifdef DEBUG
                    qDebug() << resourceErrMsg;
#endif
                    emit activationError();
                }
                break;
            }

            case readresourceinfo:

            {
                int port;
                QStringList sl;
                cMeasChannelInfo mi;

                sl = answer.toString().split(';');
                if ((reply == ack) && (sl.length() >= 4))
                {
                    port = sl.at(3).toInt(&ok); // we have to set the port where we can find our meas channel
                    if (ok)
                    {
                        mi = m_measChannelInfoHash.take(channelInfoRead);
                        mi.pcbServersocket.m_nPort = port;
                        m_measChannelInfoHash[channelInfoRead] = mi;
                        emit activationContinue();
                    }
                    else
                    {
                        emit errMsg((tr(resourceInfoErrMsg)));
#ifdef DEBUG
                        qDebug() << resourceInfoErrMsg;
#endif
                        emit activationError();
                    }
                }
                else
                {
                    emit errMsg((tr(resourceInfoErrMsg)));
#ifdef DEBUG
                    qDebug() << resourceInfoErrMsg;
#endif
                    emit activationError();
                }
                break;
            }

            case readsamplerate:
                if (reply == ack)
                {
                    m_nSRate = answer.toInt(&ok);
                    emit activationContinue();
                }
                else
                {
                    emit errMsg((tr(readsamplerateErrMsg)));
        #ifdef DEBUG
                    qDebug() << readsamplerateErrMsg;
        #endif
                    emit activationError();
                }
                break;

            case readalias:
            {
                QString alias;
                cMeasChannelInfo mi;

                if (reply == ack)
                {
                    alias = answer.toString();
                    mi = m_measChannelInfoHash.take(channelInfoRead);
                    mi.alias = alias;
                    m_measChannelInfoHash[channelInfoRead] = mi;
                    emit activationContinue();
                }
                else
                {
                    emit errMsg((tr(readaliasErrMsg)));
#ifdef DEBUG
                    qDebug() << readaliasErrMsg;
#endif
                    emit activationError();
                }
                break;
            }

            case readunit:
            {
                QString unit;
                cMeasChannelInfo mi;

                if (reply == ack)
                {
                    unit = answer.toString();
                    mi = m_measChannelInfoHash.take(channelInfoRead);
                    mi.unit = unit;
                    m_measChannelInfoHash[channelInfoRead] = mi;
                    emit activationContinue();
                }
                else
                {
                    emit errMsg((tr(readunitErrMsg)));
#ifdef DEBUG
                    qDebug() << readunitErrMsg;
#endif
                    emit activationError();
                }
                break;
            }

            case readdspchannel:
            {
                int chnnr;
                cMeasChannelInfo mi;

                if (reply == ack)
                {
                    chnnr = answer.toInt(&ok);
                    mi = m_measChannelInfoHash.take(channelInfoRead);
                    mi.dspChannelNr = chnnr;
                    m_measChannelInfoHash[channelInfoRead] = mi;
                    emit activationContinue();
                }
                else
                {
                    emit errMsg((tr(readdspchannelErrMsg)));
#ifdef DEBUG
                    qDebug() << readdspchannelErrMsg;
#endif
                    emit activationError();
                }
                break;
            break;
            }


            case writeparameter:
                if (reply == ack) // we ignore ack
                    ;
                else
                {
                    emit errMsg((tr(writedspmemoryErrMsg)));
#ifdef DEBUG
                    qDebug() << writedspmemoryErrMsg;
#endif
                    emit executionError();
                }
                break;

            case deactivatedsp:
                if (reply == ack) // we only continue if resource manager acknowledges
                    emit deactivationContinue();
                else
                {
                    emit errMsg((tr(dspdeactiveErrMsg)));
#ifdef DEBUG
                    qDebug() << dspdeactiveErrMsg;
#endif
                    emit deactivationError();
                }
                break;
            case freepgrmem:
                if (reply == ack) // we only continue if resource manager acknowledges
                    emit deactivationContinue();
                else
                {
                    emit errMsg((tr(freeresourceErrMsg)));
#ifdef DEBUG
                    qDebug() << freeresourceErrMsg;
#endif
                    emit deactivationError();
                }
                break;
            case freeusermem:
                if (reply == ack) // we only continue if resource manager acknowledges
                    emit deactivationContinue();
                else
                {
                    emit errMsg((tr(freeresourceErrMsg)));
#ifdef DEBUG
                    qDebug() << freeresourceErrMsg;
#endif
                    emit deactivationError();
                }
                break;

            case dataaquistion:
                if (reply == ack)
                    emit dataAquisitionContinue();
                else
                {
                    m_dataAcquisitionMachine.stop();
                    emit errMsg((tr(dataaquisitionErrMsg)));
#ifdef DEBUG
                    qDebug() << dataaquisitionErrMsg;
#endif
                    emit executionError(); // but we send error message
                }
                break;
            }
        }
    }
}


void cFftModuleMeasProgram::setActualValuesNames()
{
    for (int i = 0; i < m_ActValueList.count(); i++)
    {
        QStringList sl = m_ActValueList.at(i).split('-');
        QString s;
        QString s1,s2,s3,s4;
        // we have 1 or 2 entries for each value
        s1 = s2 = m_measChannelInfoHash.value(sl.at(0)).alias;
        s1.remove(QRegExp("[1-9][0-9]?"));
        s2.remove(s1);

        if (sl.count() == 1)
        {
            s = s1 + "%1" + QString(";%1;[%2]").arg(s2).arg(m_measChannelInfoHash.value(sl.at(0)).unit);
        }
        else
        {
            s3 = s4 = m_measChannelInfoHash.value(sl.at(1)).alias;
            s3.remove(QRegExp("[1-9][0-9]?"));
            s4.remove(s3);
            s = s1 + "%1-" + s3 + "%2" + QString(";%1;%2;[%3]").arg(s2).arg(s4).arg(m_measChannelInfoHash.value(sl.at(0)).unit);
        }

        m_EntityNameList.at(i)->setValue(s, m_pPeer);
    }
}


void cFftModuleMeasProgram::setInterfaceActualValues(QVector<float> *actualValues)
{
    int i;
    if (m_bActive) // maybe we are deactivating !!!!
    {
        int n = m_EntityActValueList.count();
        for (i = 0; i < n; i++) // we set n fft "values"
        {
            QList<double> fftList;
            int m = m_ConfigData.m_nFftOrder;
            int offs = i * m * 2;

            for (int j = 0; j < m; j++)
            {
                fftList.append(actualValues->at(offs + j*2));
                fftList.append(actualValues->at(offs + j*2 +1));
            }

            QVariant list;
            list = QVariant::fromValue<QList<double> >(fftList);
            m_EntityActValueList.at(i)->setValue( list, m_pPeer); // and set entities
        }
    }
}


void cFftModuleMeasProgram::resourceManagerConnect()
{
    // as this is our entry point when activating the module, we do some initialization first
    m_measChannelInfoHash.clear(); // we build up a new channel info hash
    cMeasChannelInfo mi;
    mi.pcbServersocket = m_ConfigData.m_PCBServerSocket; // the default from configuration file
    for (int i = 0; i < m_ActValueList.count(); i++)
    {
        QStringList sl = m_ActValueList.at(i).split('-');
        for (int j = 0; j < sl.count(); j++)
        {
            QString s = sl.at(j);
            if (!m_measChannelInfoHash.contains(s))
                m_measChannelInfoHash[s] = mi;
        }
    }

    // we have to instantiate a working resource manager interface
    // so first we try to get a connection to resource manager over proxy
    m_pRMClient = m_pProxy->getConnection(m_ConfigData.m_RMSocket.m_sIP, m_ConfigData.m_RMSocket.m_nPort);
    m_resourceManagerConnectState.addTransition(m_pRMClient, SIGNAL(connected()), &m_IdentifyState);
    // todo insert timer for timeout and/or connect error conditions.....
    // and then we set resource manager interface's connection
    m_pRMInterface->setClient(m_pRMClient); //
    connect(m_pRMInterface, SIGNAL(serverAnswer(quint32, quint8, QVariant)), this, SLOT(catchInterfaceAnswer(quint32, quint8, QVariant)));
    m_pProxy->startConnection(m_pRMClient);
}


void cFftModuleMeasProgram::sendRMIdent()
{
    m_MsgNrCmdList[m_pRMInterface->rmIdent(QString("FftModule%1").arg(m_pModule->getModuleNr()))] = sendrmident;
}


void cFftModuleMeasProgram::readResourceTypes()
{
    m_MsgNrCmdList[m_pRMInterface->getResourceTypes()] = readresourcetypes;
}


void cFftModuleMeasProgram::readResource()
{
    m_MsgNrCmdList[m_pRMInterface->getResources("SENSE")] = readresource;
}


void cFftModuleMeasProgram::readResourceInfos()
{
    channelInfoReadList = m_measChannelInfoHash.keys(); // we have to read information for all channels in this list
    emit activationContinue();
}


void cFftModuleMeasProgram::readResourceInfo()
{
    channelInfoRead = channelInfoReadList.takeLast();
    m_MsgNrCmdList[m_pRMInterface->getResourceInfo("SENSE", channelInfoRead)] = readresourceinfo;
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
    // we have to connect to all ports....
    channelInfoReadList = m_measChannelInfoHash.keys(); // so first we look for our different pcb sockets
    m_nConnectionCount = channelInfoReadList.count();
    for (int i = 0; i < channelInfoReadList.count(); i++)
    {
        QString key = channelInfoReadList.at(i);
        cMeasChannelInfo mi = m_measChannelInfoHash.take(key);
        cSocket sock = mi.pcbServersocket;
        Zera::Proxy::cProxyClient* pcbClient = m_pProxy->getConnection(sock.m_sIP, sock.m_nPort);
        m_pcbClientList.append(pcbClient);
        Zera::Server::cPCBInterface* pcbIFace = new Zera::Server::cPCBInterface();
        m_pcbIFaceList.append(pcbIFace);
        pcbIFace->setClient(pcbClient);
        mi.pcbIFace = pcbIFace;
        m_measChannelInfoHash[key] = mi;
        connect(pcbClient, SIGNAL(connected()), this, SLOT(monitorConnection())); // here we wait until all connections are established
        connect(pcbIFace, SIGNAL(serverAnswer(quint32, quint8, QVariant)), this, SLOT(catchInterfaceAnswer(quint32, quint8, QVariant)));
        m_pProxy->startConnection(pcbClient);
    }
}


void cFftModuleMeasProgram::readSampleRate()
{
    // we always take the sample count from the first channels pcb server
    m_MsgNrCmdList[m_pcbIFaceList.at(0)->getSampleRate()] = readsamplerate;
}


void cFftModuleMeasProgram::readChannelInformation()
{
    channelInfoReadList = m_measChannelInfoHash.keys(); // we have to read information for all channels in this list
    emit activationContinue();
}


void cFftModuleMeasProgram::readChannelAlias()
{
    channelInfoRead = channelInfoReadList.takeFirst();
    m_MsgNrCmdList[m_measChannelInfoHash[channelInfoRead].pcbIFace->getAlias(channelInfoRead)] = readalias;
}


void cFftModuleMeasProgram::readChannelUnit()
{
   m_MsgNrCmdList[m_measChannelInfoHash[channelInfoRead].pcbIFace->getUnit(channelInfoRead)] = readunit;
}


void cFftModuleMeasProgram::readDspChannel()
{
    m_MsgNrCmdList[m_measChannelInfoHash[channelInfoRead].pcbIFace->getDSPChannel(channelInfoRead)] = readdspchannel;
}


void cFftModuleMeasProgram::readDspChannelDone()
{
    if (channelInfoReadList.isEmpty())
    {
        emit activationContinue();
    }
    else
        emit activationLoop();
}


void cFftModuleMeasProgram::dspserverConnect()
{
    m_pDspClient = m_pProxy->getConnection(m_ConfigData.m_DSPServerSocket.m_sIP, m_ConfigData.m_DSPServerSocket.m_nPort);
    m_pDSPInterFace->setClient(m_pDspClient);
    m_dspserverConnectState.addTransition(m_pDspClient, SIGNAL(connected()), &m_claimPGRMemState);
    connect(m_pDSPInterFace, SIGNAL(serverAnswer(quint32, quint8, QVariant)), this, SLOT(catchInterfaceAnswer(quint32, quint8, QVariant)));
    m_pProxy->startConnection(m_pDspClient);
}


void cFftModuleMeasProgram::claimPGRMem()
{
    // if we've got dsp server connection we set up our measure program and claim the resources
    setDspVarList(); // first we set the var list for our dsp
    setDspCmdList(); // and the cmd list he has to work on
    m_MsgNrCmdList[m_pRMInterface->setResource("DSP1", "PGRMEMC", m_pDSPInterFace->cmdListCount())] = claimpgrmem;
}


void cFftModuleMeasProgram::claimUSERMem()
{
   m_MsgNrCmdList[m_pRMInterface->setResource("DSP1", "USERMEM", m_nDspMemUsed)] = claimusermem;
}


void cFftModuleMeasProgram::varList2DSP()
{
    m_MsgNrCmdList[m_pDSPInterFace->varList2Dsp()] = varlist2dsp;
}


void cFftModuleMeasProgram::cmdList2DSP()
{
    m_MsgNrCmdList[m_pDSPInterFace->cmdList2Dsp()] = cmdlist2dsp;
}


void cFftModuleMeasProgram::activateDSP()
{
    m_MsgNrCmdList[m_pDSPInterFace->activateInterface()] = activatedsp; // aktiviert die var- und cmd-listen im dsp
}


void cFftModuleMeasProgram::activateDSPdone()
{
    m_bActive = true;

    setActualValuesNames();
    m_pMeasureSignal->m_pParEntity->setValue(QVariant(1), m_pPeer);
    connect(m_pIntegrationTimeParameter, SIGNAL(updated(QVariant)), this, SLOT(newIntegrationtime(QVariant)));
    connect(m_pRefChannelParameter, SIGNAL(updated(QVariant)), SLOT(newRefChannel(QVariant)));
    emit activated();
}


void cFftModuleMeasProgram::deactivateDSP()
{
    m_bActive = false;
    m_MsgNrCmdList[m_pDSPInterFace->deactivateInterface()] = deactivatedsp; // wat wohl
}


void cFftModuleMeasProgram::freePGRMem()
{
    m_pProxy->releaseConnection(m_pDspClient);
    deleteDspVarList();
    deleteDspCmdList();

    m_MsgNrCmdList[m_pRMInterface->freeResource("DSP1", "PGRMEMC")] = freepgrmem;
}


void cFftModuleMeasProgram::freeUSERMem()
{
    m_MsgNrCmdList[m_pRMInterface->freeResource("DSP1", "USERMEM")] = freeusermem;
}


void cFftModuleMeasProgram::deactivateDSPdone()
{
    m_pProxy->releaseConnection(m_pRMClient);

    if (m_pcbIFaceList.count() > 0)
    {
        for (int i = 0; i < m_pcbIFaceList.count(); i++)
        {
            m_pProxy->releaseConnection(m_pcbClientList.at(i));
            delete m_pcbIFaceList.at(i);
        }
        m_pcbClientList.clear();
        m_pcbIFaceList.clear();
    }

    disconnect(m_pRMInterface, 0, this, 0);
    disconnect(m_pDSPInterFace, 0, this, 0);

    emit deactivated();
}


void cFftModuleMeasProgram::dataAcquisitionDSP()
{
    m_pMeasureSignal->m_pParEntity->setValue(QVariant(0), m_pPeer);
    m_MsgNrCmdList[m_pDSPInterFace->dataAcquisition(m_pActualValuesDSP)] = dataaquistion; // we start our data aquisition now
}


void cFftModuleMeasProgram::dataReadDSP()
{
    if (m_bActive)
    {
        m_pDSPInterFace->getData(m_pActualValuesDSP, m_ModuleActualValues); // we fetch our actual values

        int nChannels, nHarmonic;
        int middle, offs, resultOffs;
        double scale;

        nChannels = m_ActValueList.count();
        nHarmonic = m_ConfigData.m_nFftOrder;
        resultOffs = nHarmonic << 1;

        scale = 1.0/m_nfftLen;
        middle = m_nfftLen >> 1; // the fft results are sym. ordered with pos. and neg. frequencies
        offs = m_nfftLen << 1;

        for (int i = 0; i < nChannels; i++)
        {

            m_FFTModuleActualValues.replace(i * resultOffs, m_ModuleActualValues.at(i * offs) * scale); // special case dc
            m_FFTModuleActualValues.replace(i * resultOffs + 1, 0.0);

            for (int j = 1; j < nHarmonic; j++)
            {
                double re, im;
                // as our Fft produces math positive values, we correct them to technical positive values (*-1.0)
                // also we change real and imag. parts because we are interested in sine rather than cosine

                re = (m_ModuleActualValues.at((i * offs) + (middle << 1) - j) + m_ModuleActualValues.at((i * offs) + j)) * scale;
                im = -1.0 * (m_ModuleActualValues.at((i * offs) + (middle << 1) +j) - m_ModuleActualValues.at((i * offs) + (middle << 2) - j) ) * scale;
                m_FFTModuleActualValues.replace((i * resultOffs) + (j << 1), re);
                m_FFTModuleActualValues.replace((i * resultOffs) + (j << 1) + 1, im);
            }
        }

        emit actualValues(&m_FFTModuleActualValues); // and send them
        m_pMeasureSignal->m_pParEntity->setValue(QVariant(1), m_pPeer); // signal measuring

#ifdef DEBUG
        QString s, ts;
        for (int i = 0; i < m_ActValueList.count(); i++)
        {
            for (int j = 0; j < m_ConfigData.m_nFftOrder; j++)
            {
                ts = QString("FFT(%1(%2)):(%3,%4)[%5];").arg(m_measChannelInfoHash.value(m_ActValueList.at(i)).alias)
                        .arg(j)
                        .arg(m_FFTModuleActualValues.at((i*m_ConfigData.m_nFftOrder+j)*2))
                        .arg(m_FFTModuleActualValues.at((i*m_ConfigData.m_nFftOrder+j)*2+1))
                        .arg(m_measChannelInfoHash.value(m_ActValueList.at(i)).unit);
                s += ts;
            }
            qDebug() << s;
        }
#endif
    }
}


void cFftModuleMeasProgram::newIntegrationtime(QVariant ti)
{
    bool ok;
    m_ConfigData.m_fMeasInterval.m_fValue = ti.toDouble(&ok);

    if (m_ConfigData.m_bmovingWindow)
        m_pMovingwindowFilter->setIntegrationtime(m_ConfigData.m_fMeasInterval.m_fValue);
    else
    {
        m_pDSPInterFace->setVarData(m_pParameterDSP, QString("TIPAR:%1;TISTART:%2;").arg(m_ConfigData.m_fMeasInterval.m_fValue*1000)
                                                                                .arg(0), DSPDATA::dInt);
        m_MsgNrCmdList[m_pDSPInterFace->dspMemoryWrite(m_pParameterDSP)] = writeparameter;
    }
}


void cFftModuleMeasProgram::newRefChannel(QVariant chn)
{
    m_ConfigData.m_RefChannel.m_sPar = chn.toString();
    m_pDSPInterFace->setVarData(m_pParameterDSP, QString("REFCHN:%1;").arg(m_measChannelInfoHash.value(m_ConfigData.m_RefChannel.m_sPar).dspChannelNr));
    m_MsgNrCmdList[m_pDSPInterFace->dspMemoryWrite(m_pParameterDSP)] = writeparameter;
}
}





