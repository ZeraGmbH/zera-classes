#include "oscimodulemeasprogram.h"
#include "oscimodule.h"
#include "oscimoduleconfiguration.h"
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

cOsciModuleMeasProgram::cOsciModuleMeasProgram(cOsciModule* module, std::shared_ptr<cBaseModuleConfiguration> pConfiguration)
    :cBaseDspMeasProgram(pConfiguration), m_pModule(module)
{
    m_dspInterface = m_pModule->getServiceInterfaceFactory()->createDspInterfaceOther();

    m_IdentifyState.addTransition(this, &cOsciModuleMeasProgram::activationContinue, &m_readResourceTypesState);
    m_readResourceTypesState.addTransition(this, &cOsciModuleMeasProgram::activationContinue, &m_readResourceState);
    m_readResourceState.addTransition(this, &cOsciModuleMeasProgram::activationContinue, &m_readResourceInfosState);
    m_readResourceInfosState.addTransition(this, &cOsciModuleMeasProgram::activationContinue, &m_readResourceInfoState);
    m_readResourceInfoState.addTransition(this, &cOsciModuleMeasProgram::activationContinue, &m_readResourceInfoDoneState);
    m_readResourceInfoDoneState.addTransition(this, &cOsciModuleMeasProgram::activationContinue, &m_pcbserverConnectState);
    m_readResourceInfoDoneState.addTransition(this, &cOsciModuleMeasProgram::activationLoop, &m_readResourceInfoState);
    m_pcbserverConnectState.addTransition(this, &cOsciModuleMeasProgram::activationContinue, &m_readSampleRateState);
    m_readSampleRateState.addTransition(this, &cOsciModuleMeasProgram::activationContinue, &m_readChannelInformationState);
    m_readChannelInformationState.addTransition(this, &cOsciModuleMeasProgram::activationContinue, &m_readChannelAliasState);
    m_readChannelAliasState.addTransition(this, &cOsciModuleMeasProgram::activationContinue, &m_readChannelUnitState);
    m_readChannelUnitState.addTransition(this, &cOsciModuleMeasProgram::activationContinue, &m_readDspChannelState);
    m_readDspChannelState.addTransition(this, &cOsciModuleMeasProgram::activationContinue, &m_readDspChannelDoneState);
    m_readDspChannelDoneState.addTransition(this, &cOsciModuleMeasProgram::activationContinue, &m_dspserverConnectState);
    m_readDspChannelDoneState.addTransition(this, &cOsciModuleMeasProgram::activationLoop, &m_readChannelAliasState);
    m_claimPGRMemState.addTransition(this, &cOsciModuleMeasProgram::activationContinue, &m_claimUSERMemState);
    m_claimUSERMemState.addTransition(this, &cOsciModuleMeasProgram::activationContinue, &m_var2DSPState);
    m_var2DSPState.addTransition(this, &cOsciModuleMeasProgram::activationContinue, &m_cmd2DSPState);
    m_cmd2DSPState.addTransition(this, &cOsciModuleMeasProgram::activationContinue, &m_activateDSPState);
    m_activateDSPState.addTransition(this, &cOsciModuleMeasProgram::activationContinue, &m_loadDSPDoneState);

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

    connect(&m_resourceManagerConnectState, &QState::entered, this, &cOsciModuleMeasProgram::resourceManagerConnect);
    connect(&m_IdentifyState, &QState::entered, this, &cOsciModuleMeasProgram::sendRMIdent);
    connect(&m_readResourceTypesState, &QState::entered, this, &cOsciModuleMeasProgram::readResourceTypes);
    connect(&m_readResourceState, &QState::entered, this, &cOsciModuleMeasProgram::readResource);
    connect(&m_readResourceInfosState, &QState::entered, this, &cOsciModuleMeasProgram::readResourceInfos);
    connect(&m_readResourceInfoState, &QState::entered, this, &cOsciModuleMeasProgram::readResourceInfo);
    connect(&m_readResourceInfoDoneState, &QState::entered, this, &cOsciModuleMeasProgram::readResourceInfoDone);
    connect(&m_pcbserverConnectState, &QState::entered, this, &cOsciModuleMeasProgram::pcbserverConnect);
    connect(&m_readSampleRateState, &QState::entered, this, &cOsciModuleMeasProgram::readSampleRate);
    connect(&m_readChannelInformationState, &QState::entered, this, &cOsciModuleMeasProgram::readChannelInformation);
    connect(&m_readChannelAliasState, &QState::entered, this, &cOsciModuleMeasProgram::readChannelAlias);
    connect(&m_readChannelUnitState, &QState::entered, this, &cOsciModuleMeasProgram::readChannelUnit);
    connect(&m_readDspChannelState, &QState::entered, this, &cOsciModuleMeasProgram::readDspChannel);
    connect(&m_readDspChannelDoneState, &QState::entered, this, &cOsciModuleMeasProgram::readDspChannelDone);
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

    if(m_pModule->getDemo()){
        m_demoPeriodicTimer = TimerFactoryQt::createPeriodic(500);
        connect(m_demoPeriodicTimer.get(), &TimerTemplateQt::sigExpired,this, &cOsciModuleMeasProgram::handleDemoActualValues);
    }
}

void cOsciModuleMeasProgram::start()
{
    m_startStopHandler.start();
    if(m_pModule->getDemo())
        m_demoPeriodicTimer->start();
}

void cOsciModuleMeasProgram::stop()
{
    m_startStopHandler.stop();
    if(m_pModule->getDemo())
        m_demoPeriodicTimer->stop();
}

void cOsciModuleMeasProgram::generateInterface()
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

    m_pRefChannelParameter = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->m_pModuleValidator,
                                                      key = QString("PAR_RefChannel"),
                                                      QString("Reference channel"),
                                                      QVariant(getConfData()->m_RefChannel.m_sPar));

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
    m_pTmpDataDsp = m_dspInterface->getMemHandle("TmpData");
    m_pTmpDataDsp->addVarItem( new cDspVar("MEASSIGNAL", m_veinActValueList.count() * m_nSRate, DSPDATA::vDspTemp));
    m_pTmpDataDsp->addVarItem(new cDspVar("WORKSPACE", 2 * m_nSRate, DSPDATA::vDspTemp));
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

    m_dspInterface->addCycListItem( s = "STARTCHAIN(1,1,0x0101)"); // aktiv, prozessnr. (dummy),hauptkette 1 subkette 1 start
        m_dspInterface->addCycListItem( s = QString("CLEARN(%1,MEASSIGNAL)").arg(m_veinActValueList.count() * m_nSRate) ); // clear meassignal
        m_dspInterface->addCycListItem( s = QString("SETVAL(GAPCOUNT,%1)").arg(getConfData()->m_nGap)); // we start with the first period
        m_dspInterface->addCycListItem( s = QString("SETVAL(GAPPAR,%1)").arg(getConfData()->m_nGap+1)); // our value to reload gap
        m_dspInterface->addCycListItem( s = QString("SETVAL(REFCHN,%1)").arg(m_measChannelInfoHash.value(getConfData()->m_RefChannel.m_sPar).dspChannelNr));
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
            m_dspInterface->addCycListItem( s = QString("COPYMEM(%1,MEASSIGNAL+%2,WORKSPACE)").arg(m_nSRate).arg(i * m_nSRate));
            m_dspInterface->addCycListItem( s = QString("COPYDATA(CH%1,0,WORKSPACE+%2)").arg(m_measChannelInfoHash.value(getConfData()->m_valueChannelList.at(i)).dspChannelNr)
                                                                                     .arg(m_nSRate));

            m_dspInterface->addCycListItem( s = QString("INTERPOLATIONIND(%1,IPOLADR,VALXOSCI+%2)")
                                             .arg(getConfData()->m_nInterpolation)
                                             .arg(i * getConfData()->m_nInterpolation));
            m_dspInterface->addCycListItem( s = QString("COPYMEM(%1,WORKSPACE+%2,MEASSIGNAL+%3)").arg(m_nSRate).arg(m_nSRate).arg(i * m_nSRate));

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

cOsciModuleConfigData *cOsciModuleMeasProgram::getConfData()
{
    return qobject_cast<cOsciModuleConfiguration*>(m_pConfiguration.get())->getConfigurationData();
}


void cOsciModuleMeasProgram::setActualValuesNames()
{
    for (int i = 0; i < getConfData()->m_valueChannelList.count(); i++) {
        QString s1,s2;
        s1 = s2 = m_measChannelInfoHash.value(getConfData()->m_valueChannelList.at(i)).alias;
        s1.remove(QRegExp("[1-9][0-9]?"));
        s2.remove(s1);

        QString name = s1 + s2;

        m_veinActValueList.at(i)->setChannelName(name);
        m_veinActValueList.at(i)->setUnit(m_measChannelInfoHash.value(getConfData()->m_valueChannelList.at(i)).unit);
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

QVector<float> cOsciModuleMeasProgram::generateSineCurve(int sampleCount, int amplitude, float phase)
{
    QVector<float> samples;
    for(int i = 0; i < sampleCount; i++) {
        samples.append(amplitude * sin((2 * M_PI * i / sampleCount) + phase));
    }
    return samples;
}

void cOsciModuleMeasProgram::handleDemoActualValues()
{
    QVector<float> demoValues;
    float phase = 0.0;
    for (int i = 0; i < getConfData()->m_valueChannelList.count(); i++) {
        double randomVal = (double)rand() / RAND_MAX ;
        if(m_veinActValueList.at(i)->getChannelName().contains("2")) //UL2,IL2
            phase = randomVal * M_PI/2;
        else if(m_veinActValueList.at(i)->getChannelName().contains("3")) //UL3,IL3
            phase = randomVal * -M_PI/2;
        else
            phase = 0.0;

        if(m_veinActValueList.at(i)->getUnit() == "A")
            demoValues.append(generateSineCurve(getConfData()->m_nInterpolation, 10, phase));
        else
            demoValues.append(generateSineCurve(getConfData()->m_nInterpolation, 230, phase));
    }
    m_ModuleActualValues = demoValues;
    emit actualValues(&m_ModuleActualValues);
}


void cOsciModuleMeasProgram::resourceManagerConnect()
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


void cOsciModuleMeasProgram::readResourceTypes()
{
    m_MsgNrCmdList[m_rmInterface.getResourceTypes()] = readresourcetypes;
}


void cOsciModuleMeasProgram::readResource()
{
    m_MsgNrCmdList[m_rmInterface.getResources("SENSE")] = readresource;
}


void cOsciModuleMeasProgram::readResourceInfos()
{
    channelInfoReadList = m_measChannelInfoHash.keys(); // we have to read information for all channels in this list
    emit activationContinue();
}


void cOsciModuleMeasProgram::readResourceInfo()
{
    channelInfoRead = channelInfoReadList.takeLast();
    m_MsgNrCmdList[m_rmInterface.getResourceInfo("SENSE", channelInfoRead)] = readresourceinfo;
}


void cOsciModuleMeasProgram::readResourceInfoDone()
{
    if (channelInfoReadList.isEmpty())
        emit activationContinue();
    else
        emit activationLoop();
}


void cOsciModuleMeasProgram::pcbserverConnect()
{
    m_pcbClient = Zera::Proxy::getInstance()->getConnectionSmart(getConfData()->m_PCBServerSocket.m_sIP, getConfData()->m_PCBServerSocket.m_nPort);
    m_pcbInterface->setClientSmart(m_pcbClient);
    connect(m_pcbClient.get(), &Zera::ProxyClient::connected, this, &cBaseMeasProgram::activationContinue);
    connect(m_pcbInterface.get(), &AbstractServerInterface::serverAnswer, this, &cOsciModuleMeasProgram::catchInterfaceAnswer);
    Zera::Proxy::getInstance()->startConnectionSmart(m_pcbClient);
}


void cOsciModuleMeasProgram::readSampleRate()
{
    m_MsgNrCmdList[m_pcbInterface->getSampleRate()] = readsamplerate;
}


void cOsciModuleMeasProgram::readChannelInformation()
{
    channelInfoReadList = m_measChannelInfoHash.keys(); // we have to read information for all channels in this list
    emit activationContinue();
}


void cOsciModuleMeasProgram::readChannelAlias()
{
    channelInfoRead = channelInfoReadList.takeFirst();
    m_MsgNrCmdList[m_pcbInterface->getAlias(channelInfoRead)] = readalias;
}


void cOsciModuleMeasProgram::readChannelUnit()
{
    m_MsgNrCmdList[m_pcbInterface->getUnit(channelInfoRead)] = readunit;
}


void cOsciModuleMeasProgram::readDspChannel()
{
    m_MsgNrCmdList[m_pcbInterface->getDSPChannel(channelInfoRead)] = readdspchannel;
}


void cOsciModuleMeasProgram::readDspChannelDone()
{
    if (channelInfoReadList.isEmpty())
        emit activationContinue();
    else
        emit activationLoop();
}


void cOsciModuleMeasProgram::dspserverConnect()
{
    m_dspClient = Zera::Proxy::getInstance()->getConnectionSmart(getConfData()->m_DSPServerSocket.m_sIP, getConfData()->m_DSPServerSocket.m_nPort);
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
    getConfData()->m_RefChannel.m_sPar = chn.toString();
    DspInterfaceCmdDecoder::setVarData(m_pParameterDSP, QString("REFCHN:%1;").arg(m_measChannelInfoHash.value(getConfData()->m_RefChannel.m_sPar).dspChannelNr));
    m_MsgNrCmdList[m_dspInterface->dspMemoryWrite(m_pParameterDSP)] = writeparameter;
    emit m_pModule->parameterChanged();
}

}
