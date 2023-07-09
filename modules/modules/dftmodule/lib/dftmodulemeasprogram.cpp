#include "dftmodulemeasprogram.h"
#include "dftmodule.h"
#include "dftmoduleconfiguration.h"
#include "dftmoduleconfigdata.h"
#include <errormessages.h>
#include <movingwindowfilter.h>
#include <timerfactoryqt.h>
#include <reply.h>
#include <proxy.h>
#include <scpiinfo.h>
#include <stringvalidator.h>
#include <doublevalidator.h>
#include <math.h>
#include <complex.h>
#include <useratan.h>

namespace DFTMODULE
{


cDftModuleMeasProgram::cDftModuleMeasProgram(cDftModule* module, std::shared_ptr<cBaseModuleConfiguration> pConfiguration)
    :cBaseDspMeasProgram(pConfiguration), m_pModule(module)
{
    m_pDSPInterFace = new Zera::cDSPInterface();

    m_IdentifyState.addTransition(this, &cDftModuleMeasProgram::activationContinue, &m_readResourceTypesState);
    m_readResourceTypesState.addTransition(this, &cDftModuleMeasProgram::activationContinue, &m_readResourceState);
    m_readResourceState.addTransition(this, &cDftModuleMeasProgram::activationContinue, &m_readResourceInfosState);
    m_readResourceInfosState.addTransition(this, &cDftModuleMeasProgram::activationContinue, &m_readResourceInfoState);
    m_readResourceInfoState.addTransition(this, &cDftModuleMeasProgram::activationContinue, &m_readResourceInfoDoneState);
    m_readResourceInfoDoneState.addTransition(this, &cDftModuleMeasProgram::activationContinue, &m_pcbserverConnectState);
    m_readResourceInfoDoneState.addTransition(this, &cDftModuleMeasProgram::activationLoop, &m_readResourceInfoState);
    m_pcbserverConnectState.addTransition(this, &cDftModuleMeasProgram::activationContinue, &m_readSampleRateState);
    m_readSampleRateState.addTransition(this, &cDftModuleMeasProgram::activationContinue, &m_readChannelInformationState);
    m_readChannelInformationState.addTransition(this, &cDftModuleMeasProgram::activationContinue, &m_readChannelAliasState);
    m_readChannelAliasState.addTransition(this, &cDftModuleMeasProgram::activationContinue, &m_readChannelUnitState);
    m_readChannelUnitState.addTransition(this, &cDftModuleMeasProgram::activationContinue, &m_readDspChannelState);
    m_readDspChannelState.addTransition(this, &cDftModuleMeasProgram::activationContinue, &m_readDspChannelDoneState);
    m_readDspChannelDoneState.addTransition(this, &cDftModuleMeasProgram::activationContinue, &m_dspserverConnectState);
    m_readDspChannelDoneState.addTransition(this, &cDftModuleMeasProgram::activationLoop, &m_readChannelAliasState);

    m_claimPGRMemState.addTransition(this, &cDftModuleMeasProgram::activationContinue, &m_claimUSERMemState);
    m_claimUSERMemState.addTransition(this, &cDftModuleMeasProgram::activationContinue, &m_var2DSPState);
    m_var2DSPState.addTransition(this, &cDftModuleMeasProgram::activationContinue, &m_cmd2DSPState);
    m_cmd2DSPState.addTransition(this, &cDftModuleMeasProgram::activationContinue, &m_activateDSPState);
    m_activateDSPState.addTransition(this, &cDftModuleMeasProgram::activationContinue, &m_loadDSPDoneState);

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

    if(m_pModule->m_demo)
        m_activationMachine.setInitialState(&m_loadDSPDoneState);
    else
        m_activationMachine.setInitialState(&m_resourceManagerConnectState);

    connect(&m_resourceManagerConnectState, &QState::entered, this, &cDftModuleMeasProgram::resourceManagerConnect);
    connect(&m_IdentifyState, &QState::entered, this, &cDftModuleMeasProgram::sendRMIdent);
    connect(&m_readResourceTypesState, &QState::entered, this, &cDftModuleMeasProgram::readResourceTypes);
    connect(&m_readResourceState, &QState::entered, this, &cDftModuleMeasProgram::readResource);
    connect(&m_readResourceInfosState, &QState::entered, this, &cDftModuleMeasProgram::readResourceInfos);
    connect(&m_readResourceInfoState, &QState::entered, this, &cDftModuleMeasProgram::readResourceInfo);
    connect(&m_readResourceInfoDoneState, &QState::entered, this, &cDftModuleMeasProgram::readResourceInfoDone);
    connect(&m_pcbserverConnectState, &QState::entered, this, &cDftModuleMeasProgram::pcbserverConnect);
    connect(&m_readSampleRateState, &QState::entered, this, &cDftModuleMeasProgram::readSampleRate);
    connect(&m_readChannelInformationState, &QState::entered, this, &cDftModuleMeasProgram::readChannelInformation);
    connect(&m_readChannelAliasState, &QState::entered, this, &cDftModuleMeasProgram::readChannelAlias);
    connect(&m_readChannelUnitState, &QState::entered, this, &cDftModuleMeasProgram::readChannelUnit);
    connect(&m_readDspChannelState, &QState::entered, this, &cDftModuleMeasProgram::readDspChannel);
    connect(&m_readDspChannelDoneState, &QState::entered, this, &cDftModuleMeasProgram::readDspChannelDone);
    connect(&m_dspserverConnectState, &QState::entered, this, &cDftModuleMeasProgram::dspserverConnect);
    connect(&m_claimPGRMemState, &QState::entered, this, &cDftModuleMeasProgram::claimPGRMem);
    connect(&m_claimUSERMemState, &QState::entered, this, &cDftModuleMeasProgram::claimUSERMem);
    connect(&m_var2DSPState, &QState::entered, this, &cDftModuleMeasProgram::varList2DSP);
    connect(&m_cmd2DSPState, &QState::entered, this, &cDftModuleMeasProgram::cmdList2DSP);
    connect(&m_activateDSPState, &QState::entered, this, &cDftModuleMeasProgram::activateDSP);
    connect(&m_loadDSPDoneState, &QState::entered, this, &cDftModuleMeasProgram::activateDSPdone);

    // setting up statemachine for unloading dsp and setting resources free
    m_deactivateDSPState.addTransition(this, &cDftModuleMeasProgram::deactivationContinue, &m_freePGRMemState);
    m_freePGRMemState.addTransition(this, &cDftModuleMeasProgram::deactivationContinue, &m_freeUSERMemState);
    m_freeUSERMemState.addTransition(this, &cDftModuleMeasProgram::deactivationContinue, &m_unloadDSPDoneState);
    m_deactivationMachine.addState(&m_deactivateDSPState);
    m_deactivationMachine.addState(&m_freePGRMemState);
    m_deactivationMachine.addState(&m_freeUSERMemState);
    m_deactivationMachine.addState(&m_unloadDSPDoneState);

    if(m_pModule->m_demo)
        m_deactivationMachine.setInitialState(&m_unloadDSPDoneState);
    else
        m_deactivationMachine.setInitialState(&m_deactivateDSPState);

    connect(&m_deactivateDSPState, &QState::entered, this, &cDftModuleMeasProgram::deactivateDSP);
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

    if(m_pModule->m_demo){
        m_demoPeriodicTimer = TimerFactoryQt::createPeriodic(500);
        connect(m_demoPeriodicTimer.get(), &TimerTemplateQt::sigExpired,this, &cDftModuleMeasProgram::handleDemoActualValues);
    }
}


cDftModuleMeasProgram::~cDftModuleMeasProgram()
{
    delete m_pDSPInterFace;
}


void cDftModuleMeasProgram::start()
{
    if (getConfData()->m_bmovingWindow) {
        m_movingwindowFilter.setIntegrationtime(getConfData()->m_fMeasInterval.m_fValue);
        connect(this, &cDftModuleMeasProgram::actualValues, &m_movingwindowFilter, &cMovingwindowFilter::receiveActualValues);
        connect(&m_movingwindowFilter, &cMovingwindowFilter::actualValues, this, &cDftModuleMeasProgram::setInterfaceActualValues);
    }
    else
        connect(this, &cDftModuleMeasProgram::actualValues, this, &cDftModuleMeasProgram::setInterfaceActualValues);
    if(m_pModule->m_demo)
        m_demoPeriodicTimer->start();
}


void cDftModuleMeasProgram::stop()
{
    disconnect(this, &cDftModuleMeasProgram::actualValues, 0, 0);
    disconnect(&m_movingwindowFilter, &cMovingwindowFilter::actualValues, 0, 0);
    if(m_pModule->m_demo)
        m_demoPeriodicTimer->stop();
}


void cDftModuleMeasProgram::generateInterface()
{
    QString key;

    VfModuleActvalue *pActvalue;
    int n,p;
    n = p = 0; //
    for (int i = 0; i < getConfData()->m_valueChannelList.count(); i++)
    {
        QStringList sl = getConfData()->m_valueChannelList.at(i).split('-');
        // we have 1 or 2 entries for each value
        if (sl.count() == 1) // in this case we have phase,neutral value
        {
            pActvalue = new VfModuleActvalue(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                                QString("ACT_DFTPN%1").arg(n+1),
                                                QString("Actual value phase/neutral"),
                                                QVariant(0.0) );
            m_veinActValueList.append(pActvalue); // we add the component for our measurement
            m_pModule->veinModuleActvalueList.append(pActvalue); // and for the modules interface

            n++;
        }

        else

        {
            pActvalue = new VfModuleActvalue(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                                QString("ACT_DFTPP%1").arg(p+1),
                                                QString("Actual value phase/phase"),
                                                QVariant(0.0) );
            m_veinActValueList.append(pActvalue); // we add the component for our measurement
            m_pModule->veinModuleActvalueList.append(pActvalue); // and for the modules interface

            p++;
        }
    }

    m_pRFieldActualValue = new VfModuleActvalue(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                                   QString("ACT_RFIELD"),
                                                   QString("Phase sequence"),
                                                   QVariant("") );

    m_pModule->veinModuleActvalueList.append(m_pRFieldActualValue); // we add the component for the modules interface

    m_pDFTPNCountInfo = new VfModuleMetaData(QString("DFTPNCount"), QVariant(n));
    m_pModule->veinModuleMetaDataList.append(m_pDFTPNCountInfo);
    m_pDFTPPCountInfo = new VfModuleMetaData(QString("DFTPPCount"), QVariant(p));
    m_pModule->veinModuleMetaDataList.append(m_pDFTPPCountInfo);
    m_pDFTOrderInfo = new VfModuleMetaData(QString("DFTOrder"), QVariant(getConfData()->m_nDftOrder));



    m_pIntegrationTimeParameter = new VfModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                                           key = QString("PAR_Interval"),
                                                           QString("Integration time"),
                                                           QVariant(getConfData()->m_fMeasInterval.m_fValue));
    m_pIntegrationTimeParameter->setUnit("s");
    m_pIntegrationTimeParameter->setSCPIInfo(new cSCPIInfo("CONFIGURATION","TINTEGRATION", "10", "PAR_Interval", "0", "s"));

    m_pModule->veinModuleParameterHash[key] = m_pIntegrationTimeParameter; // for modules use

    cDoubleValidator *dValidator;
    dValidator = new cDoubleValidator(1.0, 100.0, 0.5);
    m_pIntegrationTimeParameter->setValidator(dValidator);

    m_pRefChannelParameter = new VfModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                                      key = QString("PAR_RefChannel"),
                                                      QString("Reference channel"),
                                                      QVariant(getConfData()->m_sRefChannel.m_sPar));

    m_pRefChannelParameter->setSCPIInfo(new cSCPIInfo("CONFIGURATION","REFCHANNEL", "10", "PAR_RefChannel", "0", ""));

    m_pModule->veinModuleParameterHash[key] = m_pRefChannelParameter; // for modules use
    // we must set validator after activation because we don't know the channel names here

    m_pMeasureSignal = new VfModuleComponent(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                                QString("SIG_Measuring"),
                                                QString("Signal indicating measurement activity"),
                                                QVariant(0));

    m_pModule->veinModuleComponentList.append(m_pMeasureSignal);

    if(m_pModule->m_demo)
        setDspVarList();
}


void cDftModuleMeasProgram::setDspVarList()
{
    // we fetch a handle for sampled data and other temporary values
    m_pTmpDataDsp = m_pDSPInterFace->getMemHandle("TmpData");
    m_pTmpDataDsp->addVarItem( new cDspVar("MEASSIGNAL", m_nSRate, DSPDATA::vDspTemp));
    m_pTmpDataDsp->addVarItem( new cDspVar("VALXDFT",2*m_veinActValueList.count(), DSPDATA::vDspTemp));
    m_pTmpDataDsp->addVarItem( new cDspVar("FILTER",2*2*m_veinActValueList.count(),DSPDATA::vDspTemp));
    m_pTmpDataDsp->addVarItem( new cDspVar("N",1,DSPDATA::vDspTemp));

    // a handle for parameter
    m_pParameterDSP =  m_pDSPInterFace->getMemHandle("Parameter");
    m_pParameterDSP->addVarItem( new cDspVar("TIPAR",1, DSPDATA::vDspParam, DSPDATA::dInt)); // integrationtime res = 1ms
    // we use tistart as parameter, so we can finish actual measuring interval bei setting 0
    m_pParameterDSP->addVarItem( new cDspVar("TISTART",1, DSPDATA::vDspParam, DSPDATA::dInt));

    // and one for filtered actual values
    m_pActualValuesDSP = m_pDSPInterFace->getMemHandle("ActualValues");
    m_pActualValuesDSP->addVarItem( new cDspVar("VALXDFTF",2*m_veinActValueList.count(), DSPDATA::vDspResult));

    m_ModuleActualValues.resize(m_pActualValuesDSP->getSize()); // we provide a vector for generated actual values
    m_nDspMemUsed = m_pTmpDataDsp->getSize() + m_pParameterDSP->getSize() + m_pActualValuesDSP->getSize();
}


void cDftModuleMeasProgram::deleteDspVarList()
{
    m_pDSPInterFace->deleteMemHandle(m_pTmpDataDsp);
    m_pDSPInterFace->deleteMemHandle(m_pParameterDSP);
    m_pDSPInterFace->deleteMemHandle(m_pActualValuesDSP);
}


void cDftModuleMeasProgram::setDspCmdList()
{
    QString s;

    m_pDSPInterFace->addCycListItem( s = "STARTCHAIN(1,1,0x0101)"); // aktiv, prozessnr. (dummy),hauptkette 1 subkette 1 start
        m_pDSPInterFace->addCycListItem( s = QString("CLEARN(%1,MEASSIGNAL)").arg(m_nSRate) ); // clear meassignal
        m_pDSPInterFace->addCycListItem( s = QString("CLEARN(%1,FILTER)").arg(2*2*m_veinActValueList.count()+1) ); // clear the whole filter incl. count

        if (getConfData()->m_bmovingWindow)
            m_pDSPInterFace->addCycListItem( s = QString("SETVAL(TIPAR,%1)").arg(getConfData()->m_fmovingwindowInterval*1000.0)); // initial ti time
        else
            m_pDSPInterFace->addCycListItem( s = QString("SETVAL(TIPAR,%1)").arg(getConfData()->m_fMeasInterval.m_fValue*1000.0)); // initial ti time

        m_pDSPInterFace->addCycListItem( s = "GETSTIME(TISTART)"); // einmal ti start setzen
        m_pDSPInterFace->addCycListItem( s = "DEACTIVATECHAIN(1,0x0101)"); // ende prozessnr., hauptkette 1 subkette 1
    m_pDSPInterFace->addCycListItem( s = "STOPCHAIN(1,0x0101)"); // ende prozessnr., hauptkette 1 subkette 1

    // we compute or copy our wanted actual values
    for (int i = 0; i < getConfData()->m_valueChannelList.count(); i++)
    {
        QStringList sl = getConfData()->m_valueChannelList.at(i).split('-');
        // we have 1 or 2 entries for each value
        if (sl.count() == 1)
            m_pDSPInterFace->addCycListItem( s = QString("COPYDATA(CH%1,0,MEASSIGNAL)").arg(m_measChannelInfoHash.value(sl.at(0)).dspChannelNr));
        else
            m_pDSPInterFace->addCycListItem( s = QString("COPYDIFF(CH%1,CH%2,MEASSIGNAL)").arg(m_measChannelInfoHash.value(sl.at(0)).dspChannelNr)
                                                                                      .arg(m_measChannelInfoHash.value(sl.at(1)).dspChannelNr));
        m_pDSPInterFace->addCycListItem( s = QString("DFT(%1,MEASSIGNAL,VALXDFT+%2)").arg(getConfData()->m_nDftOrder).arg(2*i));
    }

    // and filter them
    m_pDSPInterFace->addCycListItem( s = QString("AVERAGE1(%1,VALXDFT,FILTER)").arg(2*m_veinActValueList.count())); // we add results to filter

    m_pDSPInterFace->addCycListItem( s = "TESTTIMESKIPNEX(TISTART,TIPAR)");
    m_pDSPInterFace->addCycListItem( s = "ACTIVATECHAIN(1,0x0102)");

    m_pDSPInterFace->addCycListItem( s = "STARTCHAIN(0,1,0x0102)");
        m_pDSPInterFace->addCycListItem( s = "GETSTIME(TISTART)"); // set new system time
        m_pDSPInterFace->addCycListItem( s = QString("CMPAVERAGE1(%1,FILTER,VALXDFTF)").arg(2*m_veinActValueList.count()));
        m_pDSPInterFace->addCycListItem( s = QString("CLEARN(%1,FILTER)").arg(2*2*m_veinActValueList.count()+1) );
        m_pDSPInterFace->addCycListItem( s = QString("DSPINTTRIGGER(0x0,0x%1)").arg(irqNr)); // send interrupt to module
        m_pDSPInterFace->addCycListItem( s = "DEACTIVATECHAIN(1,0x0102)");
    m_pDSPInterFace->addCycListItem( s = "STOPCHAIN(1,0x0102)"); // end processnr., mainchain 1 subchain 2
}


void cDftModuleMeasProgram::deleteDspCmdList()
{
    m_pDSPInterFace->clearCmdList();
}


void cDftModuleMeasProgram::catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer)
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
                if (reply == ack) // we only continue if resource manager acknowledges
                    emit activationContinue();
                else
                {
                    emit errMsg(tr(rmidentErrMSG));
                    emit activationError();
                }
                break;
            case claimpgrmem:
                if (reply == ack) // we only continue if resource manager acknowledges
                    emit activationContinue();
                else
                {
                    emit errMsg((tr(claimresourceErrMsg)));
                    emit activationError();
                }
                break;
            case claimusermem:
                if (reply == ack) // we only continue if resource manager acknowledges
                    emit activationContinue();
                else
                {
                    emit errMsg((tr(claimresourceErrMsg)));
                    emit activationError();
                }
                break;
            case varlist2dsp:
                if (reply == ack) // we only continue if resource manager acknowledges
                    emit activationContinue();
                else
                {
                    emit errMsg((tr(dspvarlistwriteErrMsg)));
                    emit activationError();
                }
                break;
            case cmdlist2dsp:
                if (reply == ack) // we only continue if resource manager acknowledges
                    emit activationContinue();
                else
                {
                    emit errMsg((tr(dspcmdlistwriteErrMsg)));
                    emit activationError();
                }
                break;
            case activatedsp:
                if (reply == ack) // we only continue if resource manager acknowledges
                    emit activationContinue();
                else
                {
                    emit errMsg((tr(dspactiveErrMsg)));
                    emit activationError();
                }
                break;

            case readresourcetypes:
                if ((reply == ack) && (answer.toString().contains("SENSE")))
                    emit activationContinue();
                else
                {
                    emit errMsg((tr(resourcetypeErrMsg)));
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
                        emit activationError();
                    }
                }
                else
                {
                    emit errMsg((tr(resourceErrMsg)));
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
                        emit activationError();
                    }
                }
                else
                {
                    emit errMsg((tr(resourceInfoErrMsg)));
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
                    emit executionError();
                }
                break;

            case deactivatedsp:
                if (reply == ack) // we only continue if resource manager acknowledges
                    emit deactivationContinue();
                else
                {
                    emit errMsg((tr(dspdeactiveErrMsg)));
                    emit deactivationError();
                }
                break;
            case freepgrmem:
                if (reply == ack) // we only continue if resource manager acknowledges
                    emit deactivationContinue();
                else
                {
                    emit errMsg((tr(freeresourceErrMsg)));
                    emit deactivationError();
                }
                break;
            case freeusermem:
                if (reply == ack) // we only continue if resource manager acknowledges
                    emit deactivationContinue();
                else
                {
                    emit errMsg((tr(freeresourceErrMsg)));
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
                    emit executionError(); // but we send error message
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
    }
}


void cDftModuleMeasProgram::setSCPIMeasInfo()
{
    cSCPIInfo* pSCPIInfo;

    for (int i = 0; i < getConfData()->m_valueChannelList.count(); i++)
    {
        pSCPIInfo = new cSCPIInfo("MEASURE", m_veinActValueList.at(i)->getChannelName(), "8", m_veinActValueList.at(i)->getName(), "0", m_veinActValueList.at(i)->getUnit());
        m_veinActValueList.at(i)->setSCPIInfo(pSCPIInfo);
    }

    pSCPIInfo = pSCPIInfo = new cSCPIInfo("MEASURE", "RFIELD", "8", m_pRFieldActualValue->getName(), "0", "");
    m_pRFieldActualValue->setSCPIInfo(pSCPIInfo);
}


void cDftModuleMeasProgram::setRefChannelValidator()
{
    int i, n;
    QList<QString> keylist;
    QStringList sl;
    QString key, alias;
    cStringValidator *sValidator;

    keylist = m_measChannelInfoHash.keys();
    n = keylist.count();

    for (i = 0; i < n; i++)
    {
        key = keylist.at(i);
        alias = m_measChannelInfoHash.value(key).alias;
        sl.append(alias);
        m_ChannelSystemNameHash[alias] = key;
    }

    sValidator = new cStringValidator(sl);
    m_pRefChannelParameter->setValidator(sValidator);
}


void cDftModuleMeasProgram::initRFieldMeasurement()
{
    QString s;
    for (int i = 0; i < getConfData()->m_rfieldChannelList.length(); i++)
        rfieldActvalueIndexList.append(getConfData()->m_valueChannelList.indexOf(s = getConfData()->m_rfieldChannelList.at(i)));
}


void cDftModuleMeasProgram::setInterfaceActualValues(QVector<float> *actualValues)
{
    if (m_bActive) // maybe we are deactivating !!!!
    {
        for (int i = 0; i < m_veinActValueList.count(); i++)
        {
            QList<double> dftResult;
            dftResult.append(actualValues->at(2*i));
            dftResult.append(actualValues->at(2*i+1));
            QVariant list;
            list = QVariant::fromValue<QList<double> >(dftResult);
            m_veinActValueList.at(i)->setValue(list); // and set entities
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

void cDftModuleMeasProgram::handleDemoActualValues()
{
    int current = 10;
    int voltage = 230;
    double angle = 30;
    QVector<float> valuesDemo;
    QStringList listEntitiyName;
    for(const auto veinAct : qAsConst(m_veinActValueList)) {
        QString name = veinAct->getName();
        listEntitiyName.append(name);
    }

    valuesDemo.resize(m_ModuleActualValues.size());
    //Q_ASSERT(valuesDemo.size() == m_ModuleActualValues.size());
    m_ModuleActualValues = valuesDemo;
    emit actualValues(&m_ModuleActualValues);
}


void cDftModuleMeasProgram::resourceManagerConnect()
{
    // as this is our entry point when activating the module, we do some initialization first
    m_measChannelInfoHash.clear(); // we build up a new channel info hash
    cMeasChannelInfo mi;
    mi.pcbServersocket = getConfData()->m_PCBServerSocket; // the default from configuration file
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
    m_rmClient = Zera::Proxy::getInstance()->getConnectionSmart(getConfData()->m_RMSocket.m_sIP, getConfData()->m_RMSocket.m_nPort);
    m_resourceManagerConnectState.addTransition(m_rmClient.get(), &Zera::ProxyClient::connected, &m_IdentifyState);
    // todo insert timer for timeout and/or connect error conditions.....
    // and then we set resource manager interface's connection
    m_rmInterface.setClientSmart(m_rmClient); //
    connect(&m_rmInterface, &Zera::cRMInterface::serverAnswer, this, &cDftModuleMeasProgram::catchInterfaceAnswer);
    Zera::Proxy::getInstance()->startConnectionSmart(m_rmClient);
}


void cDftModuleMeasProgram::sendRMIdent()
{
    m_MsgNrCmdList[m_rmInterface.rmIdent(QString("DftModule%1").arg(m_pModule->getModuleNr()))] = sendrmident;
}


void cDftModuleMeasProgram::readResourceTypes()
{
    m_MsgNrCmdList[m_rmInterface.getResourceTypes()] = readresourcetypes;
}


void cDftModuleMeasProgram::readResource()
{
    m_MsgNrCmdList[m_rmInterface.getResources("SENSE")] = readresource;
}


void cDftModuleMeasProgram::readResourceInfos()
{
    channelInfoReadList = m_measChannelInfoHash.keys(); // we have to read information for all channels in this list
    emit activationContinue();
}


void cDftModuleMeasProgram::readResourceInfo()
{
    channelInfoRead = channelInfoReadList.takeLast();
    m_MsgNrCmdList[m_rmInterface.getResourceInfo("SENSE", channelInfoRead)] = readresourceinfo;
}


void cDftModuleMeasProgram::readResourceInfoDone()
{
    if (channelInfoReadList.isEmpty())
        emit activationContinue();
    else
        emit activationLoop();
}


void cDftModuleMeasProgram::pcbserverConnect()
{
    // we have to connect to all ports....
    channelInfoReadList = m_measChannelInfoHash.keys(); // so first we look for our different pcb sockets
    m_nConnectionCount = channelInfoReadList.count();
    for (int i = 0; i < channelInfoReadList.count(); i++)
    {
        QString key = channelInfoReadList.at(i);
        cMeasChannelInfo mi = m_measChannelInfoHash.take(key);
        cSocket sock = mi.pcbServersocket;
        Zera::ProxyClient* pcbClient = Zera::Proxy::getInstance()->getConnection(sock.m_sIP, sock.m_nPort);
        m_pcbClientList.append(pcbClient);
        Zera::cPCBInterface* pcbIFace = new Zera::cPCBInterface();
        m_pcbIFaceList.append(pcbIFace);
        pcbIFace->setClient(pcbClient);
        mi.pcbIFace = pcbIFace;
        m_measChannelInfoHash[key] = mi;
        connect(pcbClient, &Zera::ProxyClient::connected, this, &cDftModuleMeasProgram::monitorConnection); // here we wait until all connections are established
        connect(pcbIFace, &Zera::cPCBInterface::serverAnswer, this, &cDftModuleMeasProgram::catchInterfaceAnswer);
        Zera::Proxy::getInstance()->startConnection(pcbClient);
    }
}


void cDftModuleMeasProgram::readSampleRate()
{
    // we always take the sample count from the first channels pcb server
    m_MsgNrCmdList[m_pcbIFaceList.at(0)->getSampleRate()] = readsamplerate;
}


void cDftModuleMeasProgram::readChannelInformation()
{
    channelInfoReadList = m_measChannelInfoHash.keys(); // we have to read information for all channels in this list
    emit activationContinue();
}


void cDftModuleMeasProgram::readChannelAlias()
{
    channelInfoRead = channelInfoReadList.takeFirst();
    m_MsgNrCmdList[m_measChannelInfoHash[channelInfoRead].pcbIFace->getAlias(channelInfoRead)] = readalias;
}


void cDftModuleMeasProgram::readChannelUnit()
{
   m_MsgNrCmdList[m_measChannelInfoHash[channelInfoRead].pcbIFace->getUnit(channelInfoRead)] = readunit;
}


void cDftModuleMeasProgram::readDspChannel()
{
    m_MsgNrCmdList[m_measChannelInfoHash[channelInfoRead].pcbIFace->getDSPChannel(channelInfoRead)] = readdspchannel;
}


void cDftModuleMeasProgram::readDspChannelDone()
{
    if (channelInfoReadList.isEmpty())
    {
        emit activationContinue();
    }
    else
        emit activationLoop();
}


void cDftModuleMeasProgram::dspserverConnect()
{
    m_pDspClient = Zera::Proxy::getInstance()->getConnection(getConfData()->m_DSPServerSocket.m_sIP, getConfData()->m_DSPServerSocket.m_nPort);
    m_pDSPInterFace->setClient(m_pDspClient);
    m_dspserverConnectState.addTransition(m_pDspClient, &Zera::ProxyClient::connected, &m_claimPGRMemState);
    connect(m_pDSPInterFace, &Zera::cDSPInterface::serverAnswer, this, &cDftModuleMeasProgram::catchInterfaceAnswer);
    Zera::Proxy::getInstance()->startConnection(m_pDspClient);
}


void cDftModuleMeasProgram::claimPGRMem()
{
    // if we've got dsp server connection we set up our measure program and claim the resources
    setDspVarList(); // first we set the var list for our dsp
    setDspCmdList(); // and the cmd list he has to work on
    m_MsgNrCmdList[m_rmInterface.setResource("DSP1", "PGRMEMC", m_pDSPInterFace->cmdListCount())] = claimpgrmem;
}


void cDftModuleMeasProgram::claimUSERMem()
{
   m_MsgNrCmdList[m_rmInterface.setResource("DSP1", "USERMEM", m_nDspMemUsed)] = claimusermem;
}


void cDftModuleMeasProgram::varList2DSP()
{
    m_MsgNrCmdList[m_pDSPInterFace->varList2Dsp()] = varlist2dsp;
}


void cDftModuleMeasProgram::cmdList2DSP()
{
    m_MsgNrCmdList[m_pDSPInterFace->cmdList2Dsp()] = cmdlist2dsp;
}


void cDftModuleMeasProgram::activateDSP()
{
    m_MsgNrCmdList[m_pDSPInterFace->activateInterface()] = activatedsp; // aktiviert die var- und cmd-listen im dsp
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


void cDftModuleMeasProgram::deactivateDSP()
{
    m_bActive = false;
    m_MsgNrCmdList[m_pDSPInterFace->deactivateInterface()] = deactivatedsp; // wat wohl
}


void cDftModuleMeasProgram::freePGRMem()
{
    Zera::Proxy::getInstance()->releaseConnection(m_pDspClient); // no async. messages anymore
    deleteDspVarList(); // so we can destroy our actual var list
    deleteDspCmdList(); // and command list

    m_MsgNrCmdList[m_rmInterface.freeResource("DSP1", "PGRMEMC")] = freepgrmem;
}


void cDftModuleMeasProgram::freeUSERMem()
{
    m_MsgNrCmdList[m_rmInterface.freeResource("DSP1", "USERMEM")] = freeusermem;
}


void cDftModuleMeasProgram::deactivateDSPdone()
{
    if (m_pcbIFaceList.count() > 0)
    {
        for (int i = 0; i < m_pcbIFaceList.count(); i++)
        {
            Zera::Proxy::getInstance()->releaseConnection(m_pcbClientList.at(i));
            delete m_pcbIFaceList.at(i); // our signals are also gone
        }
        m_pcbIFaceList.clear();
        m_pcbClientList.clear();
    }

    disconnect(&m_rmInterface, 0, this, 0); // but we must disconnect this 2 manually
    disconnect(m_pDSPInterFace, 0, this, 0);

    emit deactivated();
}


void cDftModuleMeasProgram::dataAcquisitionDSP()
{
    m_pMeasureSignal->setValue(QVariant(0));
    m_MsgNrCmdList[m_pDSPInterFace->dataAcquisition(m_pActualValuesDSP)] = dataaquistion; // we start our data aquisition now
}


void cDftModuleMeasProgram::dataReadDSP()
{
    QHash<QString, complex> DftActValuesHash;
    if (m_bActive)
    {
        //double corr;
        m_pDSPInterFace->getData(m_pActualValuesDSP, m_ModuleActualValues); // we fetch our actual values

        /*
        // dft(0) is a speciality. sin and cos in dsp are set so that we get amplitude rather than energy.
        // so dc is multiplied  by sqrt(2) * sqrt(2) = 2
        if (getConfData()->m_nDftOrder == 0)
            corr = 0.5; // so we correct this here
        else
            corr = 1.0;

        // as our dft produces math positive values, we correct them to technical positive values
        for (int i = 0; i < m_veinActValueList.count(); i++)
        {
            double im, re;
            im = m_ModuleActualValues[i*2+1] * -1.0;
            m_ModuleActualValues.replace(i*2+1, im);
            re = m_ModuleActualValues[i*2] * corr;
            m_ModuleActualValues.replace(i*2, re);
        }
        */


        // dft(0) is a speciality. sin and cos in dsp are set so that we get amplitude rather than energy.
        // so dc is multiplied  by sqrt(2) * sqrt(2) = 2
        if (getConfData()->m_nDftOrder == 0)
        {
            double re;
            for (int i = 0; i < m_veinActValueList.count(); i++)
            {
                re = m_ModuleActualValues[i*2] * 0.5;
                m_ModuleActualValues.replace(i*2, re);
            }
        }
        else
        {
            // as our dft produces math positive values, we correct them to technical positive values (im * -1)
            for (int i = 0; i < m_veinActValueList.count(); i++)
            {
                double im;
                for (int i = 0; i < m_veinActValueList.count(); i++)
                {
                    im = m_ModuleActualValues[i*2+1] * -1.0;
                    m_ModuleActualValues.replace(i*2+1, im);
                }
            }

            // first we test if reference channel is configured
            if (getConfData()->m_bRefChannelOn)
            {
                // if so ....
                //QHash<QString, complex> DftActValuesHash;

                for (int i = 0; i < getConfData()->m_valueChannelList.count(); i++)
                  DftActValuesHash[getConfData()->m_valueChannelList.at(i)] = complex(m_ModuleActualValues[i*2], m_ModuleActualValues[i*2+1]);

                // the complex reference vector
                complex complexRef = DftActValuesHash.value(m_ChannelSystemNameHash.value(getConfData()->m_sRefChannel.m_sPar));

                double tanRef = complexRef.im() / complexRef.re();
                double divisor = sqrt(1.0+(tanRef * tanRef));
                // the turnvector has the negative reference angle
                // computing in complex is more acurate, but we have to keep in mind the
                // point of discontinuity of the arctan function
                complex turnVector;
                if (complexRef.re() < 0)
                    turnVector = complex(-(1.0 / divisor), (tanRef / divisor));
                else
                    turnVector = complex((1.0 / divisor), -(tanRef / divisor));

                // this method is alternative ... but it is not so accurate as the above one
                //double phiRef = userAtan(complexRef.im(), complexRef.re());
                //complex turnVector = complex(cos(-phiRef*0.017453292), sin(-phiRef*0.017453292));

                for (int i = 0; i < getConfData()->m_valueChannelList.count(); i++)
                {
                    QString key;
                    key = getConfData()->m_valueChannelList.at(i);
                    complex newDft = DftActValuesHash.take(key);
                    newDft *= turnVector;
                    if (fabs(newDft.im()) < 1e-8)
                        newDft = complex(newDft.re(), 0.0);
                    DftActValuesHash[key] = newDft;
                }


                // now we have to compute the difference vectors and store all new values
                for (int i = 0; i < getConfData()->m_valueChannelList.count(); i++)
                {
                    QString key;
                    QStringList sl;

                    key = getConfData()->m_valueChannelList.at(i);
                    sl = key.split('-');

                    // we have 2 entries
                    if (sl.count() == 2)
                    {
                        DftActValuesHash.remove(key);
                        DftActValuesHash[key] = DftActValuesHash[sl.at(0)] - DftActValuesHash[sl.at(1)];
                    }

                    m_ModuleActualValues.replace(i*2, DftActValuesHash[key].re());
                    m_ModuleActualValues.replace(i*2+1, DftActValuesHash[key].im());
                }
            }
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
    else
    {
        m_pDSPInterFace->setVarData(m_pParameterDSP, QString("TIPAR:%1;TISTART:%2;").arg(getConfData()->m_fMeasInterval.m_fValue*1000)
                                                                                .arg(0), DSPDATA::dInt);
        m_MsgNrCmdList[m_pDSPInterFace->dspMemoryWrite(m_pParameterDSP)] = writeparameter;
    }

    emit m_pModule->parameterChanged();
}


void cDftModuleMeasProgram::newRefChannel(QVariant refchn)
{
    getConfData()->m_sRefChannel.m_sPar = refchn.toString();
    emit m_pModule->parameterChanged();
}

}





