#include "thdnmodulemeasprogram.h"
#include "thdnmodule.h"
#include "thdnmoduleconfiguration.h"
#include "thdnmoduleconfigdata.h"
#include "errormessages.h"
#include <reply.h>
#include <movingwindowfilter.h>
#include <proxy.h>
#include <proxyclient.h>
#include <scpiinfo.h>
#include <vfmodulemetadata.h>
#include <vfmodulecomponent.h>
#include <vfmoduleparameter.h>
#include <vfmoduleactvalue.h>
#include <doublevalidator.h>
#include <intvalidator.h>
#include <rminterface.h>
#include <dspinterface.h>
#include <pcbinterface.h>
#include <math.h>
#include <QString>
#include <QStateMachine>
#include <QJsonObject>
#include <QJsonArray>
#include <timerfactoryqt.h>

namespace THDNMODULE
{

cThdnModuleMeasProgram::cThdnModuleMeasProgram(cThdnModule *module, std::shared_ptr<cBaseModuleConfiguration> pConfiguration)
    :cBaseDspMeasProgram(pConfiguration), m_pModule(module)
{
    m_pDSPInterFace = new Zera::cDSPInterface();

    m_IdentifyState.addTransition(this, &cThdnModuleMeasProgram::activationContinue, &m_readResourceTypesState);
    m_readResourceTypesState.addTransition(this, &cThdnModuleMeasProgram::activationContinue, &m_readResourceState);
    m_readResourceState.addTransition(this, &cThdnModuleMeasProgram::activationContinue, &m_readResourceInfosState);
    m_readResourceInfosState.addTransition(this, &cThdnModuleMeasProgram::activationContinue, &m_readResourceInfoState);
    m_readResourceInfoState.addTransition(this, &cThdnModuleMeasProgram::activationContinue, &m_readResourceInfoDoneState);
    m_readResourceInfoDoneState.addTransition(this, &cThdnModuleMeasProgram::activationContinue, &m_pcbserverConnectState);
    m_readResourceInfoDoneState.addTransition(this, &cThdnModuleMeasProgram::activationLoop, &m_readResourceInfoState);
    m_pcbserverConnectState.addTransition(this, &cThdnModuleMeasProgram::activationContinue, &m_readSampleRateState);
    m_readSampleRateState.addTransition(this, &cThdnModuleMeasProgram::activationContinue, &m_readChannelInformationState);
    m_readChannelInformationState.addTransition(this, &cThdnModuleMeasProgram::activationContinue, &m_readChannelAliasState);
    m_readChannelAliasState.addTransition(this, &cThdnModuleMeasProgram::activationContinue, &m_readChannelUnitState);
    m_readChannelUnitState.addTransition(this, &cThdnModuleMeasProgram::activationContinue, &m_readDspChannelState);
    m_readDspChannelState.addTransition(this, &cThdnModuleMeasProgram::activationContinue, &m_readDspChannelDoneState);
    m_readDspChannelDoneState.addTransition(this, &cThdnModuleMeasProgram::activationContinue, &m_dspserverConnectState);
    m_readDspChannelDoneState.addTransition(this, &cThdnModuleMeasProgram::activationLoop, &m_readChannelAliasState);
    m_claimPGRMemState.addTransition(this, &cThdnModuleMeasProgram::activationContinue, &m_claimUSERMemState);
    m_claimUSERMemState.addTransition(this, &cThdnModuleMeasProgram::activationContinue, &m_var2DSPState);
    m_var2DSPState.addTransition(this, &cThdnModuleMeasProgram::activationContinue, &m_cmd2DSPState);
    m_cmd2DSPState.addTransition(this, &cThdnModuleMeasProgram::activationContinue, &m_activateDSPState);
    m_activateDSPState.addTransition(this, &cThdnModuleMeasProgram::activationContinue, &m_loadDSPDoneState);

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

    connect(&m_resourceManagerConnectState, &QAbstractState::entered, this, &cThdnModuleMeasProgram::resourceManagerConnect);
    connect(&m_IdentifyState, &QAbstractState::entered, this, &cThdnModuleMeasProgram::sendRMIdent);
    connect(&m_readResourceTypesState, &QAbstractState::entered, this, &cThdnModuleMeasProgram::readResourceTypes);
    connect(&m_readResourceState, &QAbstractState::entered, this, &cThdnModuleMeasProgram::readResource);
    connect(&m_readResourceInfosState, &QAbstractState::entered, this, &cThdnModuleMeasProgram::readResourceInfos);
    connect(&m_readResourceInfoState, &QAbstractState::entered, this, &cThdnModuleMeasProgram::readResourceInfo);
    connect(&m_readResourceInfoDoneState, &QAbstractState::entered, this, &cThdnModuleMeasProgram::readResourceInfoDone);
    connect(&m_pcbserverConnectState, &QAbstractState::entered, this, &cThdnModuleMeasProgram::pcbserverConnect);
    connect(&m_readSampleRateState, &QAbstractState::entered, this, &cThdnModuleMeasProgram::readSampleRate);
    connect(&m_readChannelInformationState, &QAbstractState::entered, this, &cThdnModuleMeasProgram::readChannelInformation);
    connect(&m_readChannelAliasState, &QAbstractState::entered, this, &cThdnModuleMeasProgram::readChannelAlias);
    connect(&m_readChannelUnitState, &QAbstractState::entered, this, &cThdnModuleMeasProgram::readChannelUnit);
    connect(&m_readDspChannelState, &QAbstractState::entered, this, &cThdnModuleMeasProgram::readDspChannel);
    connect(&m_readDspChannelDoneState, &QAbstractState::entered, this, &cThdnModuleMeasProgram::readDspChannelDone);
    connect(&m_dspserverConnectState, &QAbstractState::entered, this, &cThdnModuleMeasProgram::dspserverConnect);
    connect(&m_claimPGRMemState, &QAbstractState::entered, this, &cThdnModuleMeasProgram::claimPGRMem);
    connect(&m_claimUSERMemState, &QAbstractState::entered, this, &cThdnModuleMeasProgram::claimUSERMem);
    connect(&m_var2DSPState, &QAbstractState::entered, this, &cThdnModuleMeasProgram::varList2DSP);
    connect(&m_cmd2DSPState, &QAbstractState::entered, this, &cThdnModuleMeasProgram::cmdList2DSP);
    connect(&m_activateDSPState, &QAbstractState::entered, this, &cThdnModuleMeasProgram::activateDSP);
    connect(&m_loadDSPDoneState, &QAbstractState::entered, this, &cThdnModuleMeasProgram::activateDSPdone);

    // setting up statemachine for unloading dsp and setting resources free
    m_deactivateDSPState.addTransition(this, &cThdnModuleMeasProgram::deactivationContinue, &m_freePGRMemState);
    m_freePGRMemState.addTransition(this, &cThdnModuleMeasProgram::deactivationContinue, &m_freeUSERMemState);
    m_freeUSERMemState.addTransition(this, &cThdnModuleMeasProgram::deactivationContinue, &m_unloadDSPDoneState);
    m_deactivationMachine.addState(&m_deactivateDSPState);
    m_deactivationMachine.addState(&m_freePGRMemState);
    m_deactivationMachine.addState(&m_freeUSERMemState);
    m_deactivationMachine.addState(&m_unloadDSPDoneState);

    m_deactivationMachine.setInitialState(&m_deactivateDSPState);

    connect(&m_deactivateDSPState, &QAbstractState::entered, this, &cThdnModuleMeasProgram::deactivateDSP);
    connect(&m_freePGRMemState, &QAbstractState::entered, this, &cThdnModuleMeasProgram::freePGRMem);
    connect(&m_freeUSERMemState, &QAbstractState::entered, this, &cThdnModuleMeasProgram::freeUSERMem);
    connect(&m_unloadDSPDoneState, &QAbstractState::entered, this, &cThdnModuleMeasProgram::deactivateDSPdone);

    // setting up statemachine for data acquisition
    m_dataAcquisitionState.addTransition(this, &cThdnModuleMeasProgram::dataAquisitionContinue, &m_dataAcquisitionDoneState);
    m_dataAcquisitionMachine.addState(&m_dataAcquisitionState);
    m_dataAcquisitionMachine.addState(&m_dataAcquisitionDoneState);
    m_dataAcquisitionMachine.setInitialState(&m_dataAcquisitionState);
    connect(&m_dataAcquisitionState, &QAbstractState::entered, this, &cThdnModuleMeasProgram::dataAcquisitionDSP);
    connect(&m_dataAcquisitionDoneState, &QAbstractState::entered, this, &cThdnModuleMeasProgram::dataReadDSP);

    if(m_pModule->getDemo()){
        m_demoPeriodicTimer = TimerFactoryQt::createPeriodic(500);
        connect(m_demoPeriodicTimer.get(), &TimerTemplateQt::sigExpired,this, &cThdnModuleMeasProgram::handleDemoActualValues);
    }
}


cThdnModuleMeasProgram::~cThdnModuleMeasProgram()
{
    delete m_pDSPInterFace;
    Zera::Proxy::getInstance()->releaseConnection(m_pDspClient);
}


void cThdnModuleMeasProgram::start()
{
    if (getConfData()->m_bmovingWindow)
    {
        m_movingwindowFilter.setIntegrationtime(getConfData()->m_fMeasInterval.m_fValue);
        connect(this, &cBaseMeasProgram::actualValues, &m_movingwindowFilter, &cMovingwindowFilter::receiveActualValues);
        connect(&m_movingwindowFilter, &cMovingwindowFilter::actualValues, this, &cThdnModuleMeasProgram::setInterfaceActualValues);
    }
    else
        connect(this, &cBaseMeasProgram::actualValues, this, &cThdnModuleMeasProgram::setInterfaceActualValues);
    if(m_pModule->getDemo())
        m_demoPeriodicTimer->start();
}


void cThdnModuleMeasProgram::stop()
{
    disconnect(this, &cThdnModuleMeasProgram::actualValues, 0, 0);
    disconnect(&m_movingwindowFilter, &cMovingwindowFilter::actualValues, 0, 0);
    if(m_pModule->getDemo())
        m_demoPeriodicTimer->stop();
}


void cThdnModuleMeasProgram::generateInterface()
{
    QString key;

    VfModuleActvalue *pActvalue;
    int n;
    n = getConfData()->m_valueChannelList.count();

    for (int i = 0; i < n; i++)
    {
        pActvalue = new VfModuleActvalue(m_pModule->getEntityId(), m_pModule->m_pModuleValidator,
                                            QString("ACT_THD%1%2").arg(getConfData()->m_sTHDType).arg(i+1),
                                            QString("THD%1 actual value").arg(getConfData()->m_sTHDType.toLower()),
                                            QVariant(0.0) );
        m_veinActValueList.append(pActvalue); // we add the component for our measurement
        m_pModule->veinModuleActvalueList.append(pActvalue); // and for the modules interface
    }

    m_pThdnCountInfo = new VfModuleMetaData(QString("THD%1Count").arg(getConfData()->m_sTHDType), QVariant(n));
    m_pModule->veinModuleMetaDataList.append(m_pThdnCountInfo);

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

    m_pMeasureSignal = new VfModuleComponent(m_pModule->getEntityId(), m_pModule->m_pModuleValidator,
                                                QString("SIG_Measuring"),
                                                QString("Signal indicating measurement activity"),
                                                QVariant(0));

    m_pModule->veinModuleComponentList.append(m_pMeasureSignal);
}


void cThdnModuleMeasProgram::setDspVarList()
{
    // we fetch a handle for sampled data and other temporary values
    m_pTmpDataDsp = m_pDSPInterFace->getMemHandle("TmpData");
    m_pTmpDataDsp->addVarItem( new cDspVar("MEASSIGNAL", m_nSRate, DSPDATA::vDspTemp));
    m_pTmpDataDsp->addVarItem( new cDspVar("VALXTHDN",m_veinActValueList.count(), DSPDATA::vDspTemp));
    m_pTmpDataDsp->addVarItem( new cDspVar("FILTER",2*m_veinActValueList.count(),DSPDATA::vDspTemp));
    m_pTmpDataDsp->addVarItem( new cDspVar("N",1,DSPDATA::vDspTemp));

    // a handle for parameter
    m_pParameterDSP =  m_pDSPInterFace->getMemHandle("Parameter");
    m_pParameterDSP->addVarItem( new cDspVar("TIPAR",1, DSPDATA::vDspParam, DSPDATA::dInt)); // integrationtime res = 1ms
    // we use tistart as parameter, so we can finish actual measuring interval bei setting 0
    m_pParameterDSP->addVarItem( new cDspVar("TISTART",1, DSPDATA::vDspParam, DSPDATA::dInt));

    // and one for filtered actual values
    m_pActualValuesDSP = m_pDSPInterFace->getMemHandle("ActualValues");
    m_pActualValuesDSP->addVarItem( new cDspVar("VALXTHDNF",m_veinActValueList.count(), DSPDATA::vDspResult));

    m_ModuleActualValues.resize(m_pActualValuesDSP->getSize()); // we provide a vector for generated actual values
    m_nDspMemUsed = m_pTmpDataDsp->getSize() + m_pParameterDSP->getSize() + m_pActualValuesDSP->getSize();
}


void cThdnModuleMeasProgram::deleteDspVarList()
{
    m_pDSPInterFace->deleteMemHandle(m_pTmpDataDsp);
    m_pDSPInterFace->deleteMemHandle(m_pParameterDSP);
    m_pDSPInterFace->deleteMemHandle(m_pActualValuesDSP);
}


void cThdnModuleMeasProgram::setDspCmdList()
{
    QString s;

    m_pDSPInterFace->addCycListItem( s = "STARTCHAIN(1,1,0x0101)"); // aktiv, prozessnr. (dummy),hauptkette 1 subkette 1 start
        m_pDSPInterFace->addCycListItem( s = QString("CLEARN(%1,MEASSIGNAL)").arg(m_nSRate) ); // clear meassignal
        m_pDSPInterFace->addCycListItem( s = QString("CLEARN(%1,FILTER)").arg(2*m_veinActValueList.count()+1) ); // clear the whole filter incl. count

        if (getConfData()->m_bmovingWindow)
            m_pDSPInterFace->addCycListItem( s = QString("SETVAL(TIPAR,%1)").arg(getConfData()->m_fmovingwindowInterval*1000.0)); // initial ti time
        else
            m_pDSPInterFace->addCycListItem( s = QString("SETVAL(TIPAR,%1)").arg(getConfData()->m_fMeasInterval.m_fValue*1000.0)); // initial ti time

        m_pDSPInterFace->addCycListItem( s = "GETSTIME(TISTART)"); // einmal ti start setzen
        m_pDSPInterFace->addCycListItem( s = "DEACTIVATECHAIN(1,0x0101)"); // ende prozessnr., hauptkette 1 subkette 1
    m_pDSPInterFace->addCycListItem( s = "STOPCHAIN(1,0x0101)"); // ende prozessnr., hauptkette 1 subkette 1

    // we compute or copy our wanted actual values
    for (int i = 0; i < m_veinActValueList.count(); i++)
    {
        m_pDSPInterFace->addCycListItem( s = QString("COPYDATA(CH%1,0,MEASSIGNAL)").arg(m_measChannelInfoHash.value(getConfData()->m_valueChannelList.at(i)).dspChannelNr));
        m_pDSPInterFace->addCycListItem( s = QString("THDN(MEASSIGNAL,VALXTHDN+%1)").arg(i));
    }

    // and filter them
    m_pDSPInterFace->addCycListItem( s = QString("AVERAGE1(%1,VALXTHDN,FILTER)").arg(m_veinActValueList.count())); // we add results to filter

    m_pDSPInterFace->addCycListItem( s = "TESTTIMESKIPNEX(TISTART,TIPAR)");
    m_pDSPInterFace->addCycListItem( s = "ACTIVATECHAIN(1,0x0102)");

    m_pDSPInterFace->addCycListItem( s = "STARTCHAIN(0,1,0x0102)");
        m_pDSPInterFace->addCycListItem( s = "GETSTIME(TISTART)"); // set new system time
        m_pDSPInterFace->addCycListItem( s = QString("CMPAVERAGE1(%1,FILTER,VALXTHDNF)").arg(m_veinActValueList.count()));
        m_pDSPInterFace->addCycListItem( s = QString("CLEARN(%1,FILTER)").arg(2*m_veinActValueList.count()+1) );
        m_pDSPInterFace->addCycListItem( s = QString("DSPINTTRIGGER(0x0,0x%1)").arg(irqNr)); // send interrupt to module
        m_pDSPInterFace->addCycListItem( s = "DEACTIVATECHAIN(1,0x0102)");
    m_pDSPInterFace->addCycListItem( s = "STOPCHAIN(1,0x0102)"); // end processnr., mainchain 1 subchain 2
}


void cThdnModuleMeasProgram::deleteDspCmdList()
{
    m_pDSPInterFace->clearCmdList();
}


void cThdnModuleMeasProgram::catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer)
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

cThdnModuleConfigData *cThdnModuleMeasProgram::getConfData()
{
    return qobject_cast<cThdnModuleConfiguration*>(m_pConfiguration.get())->getConfigurationData();
}


void cThdnModuleMeasProgram::setActualValuesNames()
{
    for (int i = 0; i < getConfData()->m_valueChannelList.count(); i++)
    {
        QString s;
        QString s1,s2;
        QString name;

        s1 = s2 = m_measChannelInfoHash.value(getConfData()->m_valueChannelList.at(i)).alias;
        s1.remove(QRegExp("[1-9][0-9]?"));
        s2.remove(s1);

        s = s1 + "%1" + QString(";%1;[%]").arg(s2);

        name = s1 + s2;

        m_veinActValueList.at(i)->setChannelName(name);
        m_veinActValueList.at(i)->setUnit(QString("%"));
    }
}


void cThdnModuleMeasProgram::setSCPIMeasInfo()
{
    cSCPIInfo* pSCPIInfo;

    for (int i = 0; i < getConfData()->m_valueChannelList.count(); i++)
    {
        pSCPIInfo = new cSCPIInfo("MEASURE", m_veinActValueList.at(i)->getChannelName(), "8", m_veinActValueList.at(i)->getName(), "0", QString("%"));
        m_veinActValueList.at(i)->setSCPIInfo(pSCPIInfo);
    }
}

void cThdnModuleMeasProgram::setupDemoOperation()
{
    m_measChannelInfoHash.clear();
    cMeasChannelInfo mi;
    for (int i = 0; i < getConfData()->m_valueChannelList.count(); i++)
    {
        QString channelName = getConfData()->m_valueChannelList.at(i);
        if (!m_measChannelInfoHash.contains(channelName))
            m_measChannelInfoHash[channelName] = mi;
    }
    QList<QString> channelInfoList = m_measChannelInfoHash.keys();
    foreach (QString channelInfo, channelInfoList) {
        mi = m_measChannelInfoHash.take(channelInfo);
        if (channelInfo == "m0") {
            mi.alias = "UL1";
        }
        else if (channelInfo == "m1") {
            mi.alias = "UL2";
        }
        else if (channelInfo == "m2") {
            mi.alias = "UL3";
        }
        else if (channelInfo == "m3") {
            mi.alias = "IL1";
        }
        else if (channelInfo == "m4") {
            mi.alias = "IL2";
        }
        else if (channelInfo == "m5") {
            mi.alias = "IL3";
        }
        else if (channelInfo == "m6") {
            mi.alias = "UAUX";
        }
        else if (channelInfo == "m7") {
            mi.alias = "IAUX";
        }
        else {
        }
        mi.unit = "%";
        m_measChannelInfoHash[channelInfo] = mi;
    }
}


void cThdnModuleMeasProgram::setInterfaceActualValues(QVector<float> *actualValues)
{
    if (m_bActive) // maybe we are deactivating !!!!
    {
        for (int i = 0; i < m_veinActValueList.count(); i++)
            m_veinActValueList.at(i)->setValue(QVariant((double)actualValues->at(i))); // and set entities
    }
}

void cThdnModuleMeasProgram::handleDemoActualValues()
{
    QVector<float> demoValues;
    double thdn, thdr;

    for (int i = 0; i < getConfData()->m_valueChannelList.count(); i++) {
        float val = 20 * (float)rand() / RAND_MAX ;
        demoValues.append(val);
        thdn = demoValues.at(i) / 100.0;
        thdr = 100.0 * thdn / sqrt(1 + (thdn * thdn));
        demoValues.replace(i, thdr);
    }
    m_ModuleActualValues = demoValues;
    Q_ASSERT(demoValues.size() == m_ModuleActualValues.size());
    emit actualValues(&m_ModuleActualValues);
}


void cThdnModuleMeasProgram::resourceManagerConnect()
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
    // and then we set resource manager interface's connection
    m_rmInterface.setClientSmart(m_rmClient);
    m_resourceManagerConnectState.addTransition(m_rmClient.get(), &Zera::ProxyClient::connected, &m_IdentifyState);
    connect(&m_rmInterface, &AbstractServerInterface::serverAnswer, this, &cThdnModuleMeasProgram::catchInterfaceAnswer);
    Zera::Proxy::getInstance()->startConnectionSmart(m_rmClient);
}


void cThdnModuleMeasProgram::sendRMIdent()
{
    m_MsgNrCmdList[m_rmInterface.rmIdent(QString("ThdnModule%1").arg(m_pModule->getModuleNr()))] = sendrmident;
}


void cThdnModuleMeasProgram::readResourceTypes()
{
    m_MsgNrCmdList[m_rmInterface.getResourceTypes()] = readresourcetypes;
}


void cThdnModuleMeasProgram::readResource()
{
    m_MsgNrCmdList[m_rmInterface.getResources("SENSE")] = readresource;
}


void cThdnModuleMeasProgram::readResourceInfos()
{
    channelInfoReadList = m_measChannelInfoHash.keys(); // we have to read information for all channels in this list
    emit activationContinue();
}


void cThdnModuleMeasProgram::readResourceInfo()
{
    channelInfoRead = channelInfoReadList.takeLast();
    m_MsgNrCmdList[m_rmInterface.getResourceInfo("SENSE", channelInfoRead)] = readresourceinfo;
}


void cThdnModuleMeasProgram::readResourceInfoDone()
{
    if (channelInfoReadList.isEmpty())
        emit activationContinue();
    else
        emit activationLoop();
}


void cThdnModuleMeasProgram::pcbserverConnect()
{
    m_pcbClient = Zera::Proxy::getInstance()->getConnectionSmart(getConfData()->m_PCBServerSocket.m_sIP, getConfData()->m_PCBServerSocket.m_nPort);
    m_pcbInterface->setClientSmart(m_pcbClient);
    connect(m_pcbClient.get(), &Zera::ProxyClient::connected, this, &cBaseMeasProgram::activationContinue);
    connect(m_pcbInterface.get(), &AbstractServerInterface::serverAnswer, this, &cThdnModuleMeasProgram::catchInterfaceAnswer);
    Zera::Proxy::getInstance()->startConnectionSmart(m_pcbClient);
}


void cThdnModuleMeasProgram::readSampleRate()
{
    m_MsgNrCmdList[m_pcbInterface->getSampleRate()] = readsamplerate;
}


void cThdnModuleMeasProgram::readChannelInformation()
{
    channelInfoReadList = m_measChannelInfoHash.keys(); // we have to read information for all channels in this list
    emit activationContinue();
}


void cThdnModuleMeasProgram::readChannelAlias()
{
    channelInfoRead = channelInfoReadList.takeFirst();
    m_MsgNrCmdList[m_pcbInterface->getAlias(channelInfoRead)] = readalias;
}


void cThdnModuleMeasProgram::readChannelUnit()
{
    m_MsgNrCmdList[m_pcbInterface->getUnit(channelInfoRead)] = readunit;
}


void cThdnModuleMeasProgram::readDspChannel()
{
    m_MsgNrCmdList[m_pcbInterface->getDSPChannel(channelInfoRead)] = readdspchannel;
}


void cThdnModuleMeasProgram::readDspChannelDone()
{
    if (channelInfoReadList.isEmpty())
        emit activationContinue();
    else
        emit activationLoop();
}


void cThdnModuleMeasProgram::dspserverConnect()
{
    m_pDspClient = Zera::Proxy::getInstance()->getConnection(getConfData()->m_DSPServerSocket.m_sIP, getConfData()->m_DSPServerSocket.m_nPort);
    m_pDSPInterFace->setClient(m_pDspClient);
    m_dspserverConnectState.addTransition(m_pDspClient, &Zera::ProxyClient::connected, &m_claimPGRMemState);
    connect(m_pDSPInterFace, &Zera::cDSPInterface::serverAnswer, this, &cThdnModuleMeasProgram::catchInterfaceAnswer);
    Zera::Proxy::getInstance()->startConnection(m_pDspClient);
}


void cThdnModuleMeasProgram::claimPGRMem()
{
    setDspVarList(); // first we set the var list for our dsp
    setDspCmdList(); // and the cmd list he has to work on
    m_MsgNrCmdList[m_rmInterface.setResource("DSP1", "PGRMEMC", m_pDSPInterFace->cmdListCount())] = claimpgrmem;
}


void cThdnModuleMeasProgram::claimUSERMem()
{
   m_MsgNrCmdList[m_rmInterface.setResource("DSP1", "USERMEM", m_nDspMemUsed)] = claimusermem;
}


void cThdnModuleMeasProgram::varList2DSP()
{
    m_MsgNrCmdList[m_pDSPInterFace->varList2Dsp()] = varlist2dsp;
}


void cThdnModuleMeasProgram::cmdList2DSP()
{
    m_MsgNrCmdList[m_pDSPInterFace->cmdList2Dsp()] = cmdlist2dsp;
}


void cThdnModuleMeasProgram::activateDSP()
{
    m_MsgNrCmdList[m_pDSPInterFace->activateInterface()] = activatedsp; // aktiviert die var- und cmd-listen im dsp
}


void cThdnModuleMeasProgram::activateDSPdone()
{
    m_bActive = true;

    if(m_pModule->getDemo())
        setupDemoOperation();

    setActualValuesNames();
    setSCPIMeasInfo();

    m_pMeasureSignal->setValue(QVariant(1));
    connect(m_pIntegrationTimeParameter, &VfModuleComponent::sigValueChanged, this, &cThdnModuleMeasProgram::newIntegrationtime);

    emit activated();
}


void cThdnModuleMeasProgram::deactivateDSP()
{
    m_bActive = false;
    m_MsgNrCmdList[m_pDSPInterFace->deactivateInterface()] = deactivatedsp; // wat wohl
}


void cThdnModuleMeasProgram::freePGRMem()
{
    Zera::Proxy::getInstance()->releaseConnection(m_pDspClient); // no async. messages anymore
    deleteDspVarList(); // so we can destroy our actual var list
    deleteDspCmdList(); // and command list

    m_MsgNrCmdList[m_rmInterface.freeResource("DSP1", "PGRMEMC")] = freepgrmem;
}


void cThdnModuleMeasProgram::freeUSERMem()
{
    m_MsgNrCmdList[m_rmInterface.freeResource("DSP1", "USERMEM")] = freeusermem;
}


void cThdnModuleMeasProgram::deactivateDSPdone()
{
    disconnect(&m_rmInterface, 0, this, 0);
    disconnect(m_pDSPInterFace, 0, this, 0);
    disconnect(m_pcbInterface.get(), 0, this, 0);
    emit deactivated();
}


void cThdnModuleMeasProgram::dataAcquisitionDSP()
{
    m_pMeasureSignal->setValue(QVariant(0));
    m_MsgNrCmdList[m_pDSPInterFace->dataAcquisition(m_pActualValuesDSP)] = dataaquistion; // we start our data aquisition now
}


void cThdnModuleMeasProgram::dataReadDSP()
{
    if (m_bActive)
    {
        m_pDSPInterFace->getData(m_pActualValuesDSP, m_ModuleActualValues); // we fetch our actual values
        if (getConfData()->m_sTHDType == "R")
        {
            double thdn, thdr;
            for (int i = 0; i < m_ModuleActualValues.length(); i++)
            {
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
    bool ok;
    getConfData()->m_fMeasInterval.m_fValue = ti.toDouble(&ok);

    if (getConfData()->m_bmovingWindow)
        m_movingwindowFilter.setIntegrationtime(getConfData()->m_fMeasInterval.m_fValue);
    else {
        m_pDSPInterFace->setVarData(m_pParameterDSP, QString("TIPAR:%1;TISTART:%2;").arg(getConfData()->m_fMeasInterval.m_fValue*1000)
                                                                                .arg(0), DSPDATA::dInt);
        m_MsgNrCmdList[m_pDSPInterFace->dspMemoryWrite(m_pParameterDSP)] = writeparameter;
    }

    emit m_pModule->parameterChanged();
}

}
