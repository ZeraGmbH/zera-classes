#include <QString>
#include <QStateMachine>
#include <rminterface.h>
#include <dspinterface.h>
#include <pcbinterface.h>
#include <proxy.h>
#include <proxyclient.h>
#include <veinpeer.h>
#include <veinentity.h>
#include <QPointF>

#include "modulesignal.h"
#include "moduleparameter.h"
#include "rmsmodule.h"
#include "rmsmoduleconfigdata.h"
#include "rmsmodulemeasprogram.h"


cRmsModuleMeasProgram::cRmsModuleMeasProgram(Zera::Proxy::cProxy* proxy, VeinPeer* peer, Zera::Server::cDSPInterface* iface, cRmsModuleConfigData& configdata)
    :cBaseMeasProgram(proxy, peer, iface), m_ConfigData(configdata)
{
    m_pRMInterface = new Zera::Server::cRMInterface();

    m_IdentifyState.addTransition(this, SIGNAL(activationContinue()), &m_readResourceTypesState);
    m_readResourceTypesState.addTransition(this, SIGNAL(activationContinue()), &m_readResourceState);
    m_readResourceState.addTransition(this, SIGNAL(activationContinue()), &m_readResourceInfosState);
    m_readResourceInfosState.addTransition(this, SIGNAL(activationContinue()), &m_readResourceInfoState);
    m_readResourceInfoState.addTransition(this, SIGNAL(activationContinue()), &m_readResourceInfoDoneState);
    m_readResourceInfoDoneState.addTransition(this, SIGNAL(activationContinue()), &m_pcbserverConnectState);
    m_readResourceInfoDoneState.addTransition(this, SIGNAL(activationLoop()), &m_readResourceInfoState);
    m_pcbserverConnectState.addTransition(this, SIGNAL(activationContinue()), &m_readSamplenrState);
    m_readSamplenrState.addTransition(this, SIGNAL(activationContinue()), &m_readChannelInformationState);
    m_readChannelInformationState.addTransition(this, SIGNAL(activationContinue()), &m_readChannelAliasState);
    m_readChannelAliasState.addTransition(this, SIGNAL(activationContinue()), &m_readDspChannelState);
    m_readDspChannelState.addTransition(this, SIGNAL(activationContinue()), &m_readDspChannelDoneState);
    m_readDspChannelDoneState.addTransition(this, SIGNAL(activationContinue()), &m_claimPGRMemState);
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
    m_activationMachine.addState(&m_readSamplenrState);
    m_activationMachine.addState(&m_readChannelInformationState);
    m_activationMachine.addState(&m_readChannelAliasState);
    m_activationMachine.addState(&m_readDspChannelState);
    m_activationMachine.addState(&m_readDspChannelDoneState);
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
    connect(&m_pcbserverConnectState, SIGNAL(entered()), SLOT(pcbserverConnect()));
    connect(&m_readSamplenrState, SIGNAL(entered()), SLOT(readSamplenr()));
    connect(&m_readChannelInformationState, SIGNAL(entered()), SLOT(readDspChannels()));
    connect(&m_readDspChannelState, SIGNAL(entered()), SLOT(readDspChannel()));
    connect(&m_readDspChannelDoneState, SIGNAL(entered()), SLOT(readDspChannelDone()));
    connect(&m_claimPGRMemState, SIGNAL(entered()), SLOT(claimPGRMem()));
    connect(&m_claimUSERMemState, SIGNAL(entered()), SLOT(claimUSERMem()));
    connect(&m_var2DSPState, SIGNAL(entered()), SLOT(varList2DSP()));
    connect(&m_cmd2DSPState, SIGNAL(entered()), SLOT(cmdList2DSP()));
    connect(&m_activateDSPState, SIGNAL(entered()), SLOT(activateDSP()));
    connect(&m_loadDSPDoneState, SIGNAL(entered()), SLOT(activateDSPdone()));

    // setting up statemachine for unloading dsp and setting resources free
    m_deactivateDSPState.addTransition(this, SIGNAL(activationContinue()), &m_freePGRMemState);
    m_freePGRMemState.addTransition(this, SIGNAL(activationContinue()), &m_freeUSERMemState);
    m_freeUSERMemState.addTransition(this, SIGNAL(activationContinue()), &m_unloadDSPDoneState);
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
    m_dataAcquisitionState.addTransition(this, SIGNAL(activationContinue()), &m_dataAcquisitionDoneState);
    m_dataAcquisitionMachine.addState(&m_dataAcquisitionState);
    m_dataAcquisitionMachine.addState(&m_dataAcquisitionDoneState);
    m_dataAcquisitionMachine.setInitialState(&m_dataAcquisitionState);
    connect(&m_dataAcquisitionState, SIGNAL(entered()), SLOT(dataAcquisitionDSP()));
    connect(&m_dataAcquisitionDoneState, SIGNAL(entered()), SLOT(dataReadDSP()));
}


cRmsModuleMeasProgram::~cRmsModuleMeasProgram()
{
    delete m_pRMInterface;
    //delete m_pPCBInterface;
    m_pProxy->releaseConnection(m_pRMClient);
    //m_pProxy->releaseConnection(m_pPCBClient);
}


void cRmsModuleMeasProgram::start()
{
    connect(this, SIGNAL(actualValues(QVector<float>*)), this, SLOT(setInterfaceActualValues(QVector<float>*)));
}


void cRmsModuleMeasProgram::stop()
{
    disconnect(this, SIGNAL(actualValues(QVector<float>*)), this, SLOT(setInterfaceActualValues(QVector<float>*)));
}


void cRmsModuleMeasProgram::generateInterface()
{
    VeinEntity* p_entity;
    for (int i = 0; i < m_ActValueList.count(); i++)
    {
        p_entity = m_pPeer->dataAdd(QString("ACT_RMS%1").arg(m_ActValueList.at(i)));
        p_entity->modifiersAdd(VeinEntity::MOD_READONLY);
        p_entity->modifiersAdd(VeinEntity::MOD_NOECHO);
        p_entity->setValue(QVariant((double) 0.0), m_pPeer);
        m_EntityList.append(p_entity);
    }

    m_pIntegrationTimeParameter = new cModuleParameter(m_pPeer, "PAR_INTEGRATIONTIME", QVariant((double) m_ConfigData.m_fMeasInterval.m_fValue), "PAR_INTEGRATIONTIME_LIMITS", QVariant(QPointF(0.1,100.0)));
    m_pMeasureSignal = new cModuleSignal(m_pPeer, "SIG_MEASURING", QVariant(0));
}


void cRmsModuleMeasProgram::deleteInterface()
{
    for (int i = 0; i < m_EntityList.count(); i++)
        m_pPeer->dataRemove(m_EntityList.at(i));
    delete m_pIntegrationTimeParameter;
    delete m_pMeasureSignal;
}


void cRmsModuleMeasProgram::setDspVarList()
{
    // we fetch a handle for sampled data and other temporary values
    m_pTmpDataDsp = m_pDSPIFace->getMemHandle("TmpData");
    m_pTmpDataDsp->addVarItem( new cDspVar("MEASSIGNAL", m_nSamples, DSPDATA::vDspTemp));
    m_pTmpDataDsp->addVarItem( new cDspVar("VALXRMS",m_ActValueList.count(), DSPDATA::vDspTemp));
    m_pTmpDataDsp->addVarItem( new cDspVar("FILTER",2*2*m_ActValueList.count(),DSPDATA::vDspTemp));
    m_pTmpDataDsp->addVarItem( new cDspVar("N",1,DSPDATA::vDspTemp));

    // a handle for parameter
    m_pParameterDSP =  m_pDSPIFace->getMemHandle("Parameter");
    m_pParameterDSP->addVarItem( new cDspVar("TIPAR",1, DSPDATA::vDspParam, DSPDATA::dInt)); // integrationtime res = 1ms
    // we use tistart as parameter, so we can finish actual measuring interval bei setting 0
    m_pParameterDSP->addVarItem( new cDspVar("TISTART",1, DSPDATA::vDspTemp, DSPDATA::dInt));

    // and one for filtered actual values
    m_pActualValuesDSP = m_pDSPIFace->getMemHandle("ActualValues");
    m_pActualValuesDSP->addVarItem( new cDspVar("VALXRMSF",m_ActValueList.count(), DSPDATA::vDspResult));

    m_ModuleActualValues.resize(m_pActualValuesDSP->getSize()); // we provide a vector for generated actual values
    m_nDspMemUsed = m_pTmpDataDsp->getSize() + m_pParameterDSP->getSize() + m_pActualValuesDSP->getSize();
}


void cRmsModuleMeasProgram::deleteDspVarList()
{
    m_pDSPIFace->deleteMemHandle(m_pTmpDataDsp);
    m_pDSPIFace->deleteMemHandle(m_pParameterDSP);
    m_pDSPIFace->deleteMemHandle(m_pActualValuesDSP);
}


void cRmsModuleMeasProgram::setDspCmdList()
{
    QString s;

    m_pDSPIFace->addCycListItem( s = "STARTCHAIN(1,1,0x0101)"); // aktiv, prozessnr. (dummy),hauptkette 1 subkette 1 start
        m_pDSPIFace->addCycListItem( s = QString("CLEARN(%1,MEASSIGNAL)").arg(m_nSamples) ); // clear meassignal
        m_pDSPIFace->addCycListItem( s = QString("CLEARN(%1,FILTER)").arg(2*2*m_ActValueList.count()+1) ); // clear the whole filter incl. count
        m_pDSPIFace->addCycListItem( s = QString("SETVAL(TIPAR,%1)").arg(m_ConfigData.m_fMeasInterval.m_fValue*1000.0)); // initial ti time  /* todo variabel */
        m_pDSPIFace->addCycListItem( s = "GETSTIME(TISTART)"); // einmal ti start setzen
        m_pDSPIFace->addCycListItem( s = "DEACTIVATECHAIN(1,0x0101)"); // ende prozessnr., hauptkette 1 subkette 1
    m_pDSPIFace->addCycListItem( s = "STOPCHAIN(1,0x0101)"); // ende prozessnr., hauptkette 1 subkette 1

    // we compute or copy our wanted actual values
    for (int i = 0; i < m_ActValueList.count(); i++)
    {
        QStringList sl = m_ActValueList.at(i).split('-');
        // we have 1 or 2 entries for each value
        if (sl.count() == 1)
            m_pDSPIFace->addCycListItem( s = QString("COPYDATA(CH%1,0,MEASSIGNAL)").arg(m_measChannelInfoHash.value(sl.at(0)).dspChannelNr));
        else
            m_pDSPIFace->addCycListItem( s = QString("COPYDIFF(CH%1,CH%2,MEASSIGNAL)").arg(m_measChannelInfoHash.value(sl.at(0)).dspChannelNr)
                                                                                      .arg(m_measChannelInfoHash.value(sl.at(1)).dspChannelNr));
        m_pDSPIFace->addCycListItem( s = QString("RMS(MEASSIGNAL,VALXRMS+%1)").arg(i));
    }

    // and filter them
    m_pDSPIFace->addCycListItem( s = QString("AVERAGE1(%1,VALXRMS,FILTER)").arg(2*m_ActValueList.count())); // we add results to filter

    m_pDSPIFace->addCycListItem( s = "TESTTIMESKIPNEX(TISTART,TIPAR)");
    m_pDSPIFace->addCycListItem( s = "ACTIVATECHAIN(1,0x0102)");

    m_pDSPIFace->addCycListItem( s = "STARTCHAIN(0,1,0x0102)");
        m_pDSPIFace->addCycListItem( s = "GETSTIME(TISTART)"); // set new system time
        m_pDSPIFace->addCycListItem( s = QString("CMPAVERAGE1(%1,FILTER,VALXRMSF)").arg(2*m_ActValueList.count()));
        m_pDSPIFace->addCycListItem( s = QString("CLEARN(%1,FILTER)").arg(2*2*m_ActValueList.count()+1) );
        m_pDSPIFace->addCycListItem( s = "DSPINTTRIGGER(0x0,0x0001)"); // send interrupt to module
        m_pDSPIFace->addCycListItem( s = "DEACTIVATECHAIN(1,0x0102)");
    m_pDSPIFace->addCycListItem( s = "STOPCHAIN(1,0x0102)"); // end processnr., mainchain 1 subchain 2
}


void cRmsModuleMeasProgram::deleteDspCmdList()
{
    m_pDSPIFace->clearCmdList();
}


void cRmsModuleMeasProgram::catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer)
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
        case 1:
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
            case RMSMEASPROGRAM::sendrmident:
            case RMSMEASPROGRAM::claimpgrmem:
            case RMSMEASPROGRAM::claimusermem:
            case RMSMEASPROGRAM::varlist2dsp:
            case RMSMEASPROGRAM::cmdlist2dsp:
            case RMSMEASPROGRAM::activatedsp:
                if (reply == RMSMEASPROGRAM::ack) // we only continue if resource manager acknowledges
                    emit activationContinue();
                else
                    emit activationError();
                break;

            case RMSMEASPROGRAM::readresourcetypes:
                if ((reply == RMSMEASPROGRAM::ack) && (answer.toString().contains("SENSE")))
                    emit activationContinue();
                else
                    emit activationError();
                break;

            case RMSMEASPROGRAM::readresource:
            {
                if (reply == RMSMEASPROGRAM::ack)
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
                        emit activationError();
                }
                else
                    emit activationError();
                break;
            }

            case RMSMEASPROGRAM::readresourceinfo:

            {
                int port;
                QStringList sl;
                cMeasChannelInfo mi;

                sl = answer.toString().split(';');
                if ((reply == RMSMEASPROGRAM::ack) && (sl.length() >= 4))
                {
                    port = sl.at(3).toInt(&ok); // we have to set the port where we can find our meas channel
                    if (ok)
                    {
                        mi = m_measChannelInfoHash.take(channelInfoRead);
                        mi.socket.m_nPort = port;
                        m_measChannelInfoHash[channelInfoRead] = mi;
                        emit activationContinue();
                    }
                    else
                        emit activationError();
                }
                else
                    emit activationError();
                break;
            }

            case RMSMEASPROGRAM::readsamplenr:
            if (reply == RMSMEASPROGRAM::ack)
            {
                m_nSamples = answer.toInt(&ok);
                emit activationContinue();
            }
            else
                emit activationError();
            break;

            case RMSMEASPROGRAM::readalias:
            {
                QString alias;
                cMeasChannelInfo mi;

                if (reply == RMSMEASPROGRAM::ack)
                {
                    alias = answer.toString();
                    mi = m_measChannelInfoHash.take(channelInfoRead);
                    mi.alias = alias;
                    m_measChannelInfoHash[channelInfoRead] = mi;
                    emit activationContinue();
                }
                else
                    emit activationError();
                break;
            }

            case RMSMEASPROGRAM::readdspchannel:
            {
                int chnnr;
                cMeasChannelInfo mi;

                if (reply == RMSMEASPROGRAM::ack)
                {
                    chnnr = answer.toInt(&ok);
                    mi = m_measChannelInfoHash.take(channelInfoRead);
                    mi.dspChannelNr = chnnr;
                    m_measChannelInfoHash[channelInfoRead] = mi;
                    emit activationContinue();
                }
                else
                    emit activationError();
            break;
            }


            case RMSMEASPROGRAM::writeparameter:
                if (reply == RMSMEASPROGRAM::ack) // we ignore ack
                    ;
                else
                    emit executionError(); // but we send error message
                break;

            case RMSMEASPROGRAM::deactivatedsp:
            case RMSMEASPROGRAM::freepgrmem:
            case RMSMEASPROGRAM::freeusermem:
                if (reply == RMSMEASPROGRAM::ack) // we only continue if resource manager acknowledges
                    emit activationContinue();
                else
                    emit deactivationError();
                break;

            case RMSMEASPROGRAM::dataaquistion:
                if (reply == RMSMEASPROGRAM::ack)
                    emit activationContinue();
                else
                {
                    m_dataAcquisitionMachine.stop();
                    emit executionError(); // but we send error message
                }
                break;
            }
        }
    }
}


void cRmsModuleMeasProgram::setActualValuesNames()
{
    for (int i = 0; i < m_ActValueList.count(); i++)
    {
        QStringList sl = m_ActValueList.at(i).split('-');
        QString s;
        // we have 1 or 2 entries for each value
        if (sl.count() == 1)
            s = QString("ACT_RMS%1").arg(m_measChannelInfoHash.value(sl.at(0)).alias);
        else
            s = QString("ACT_RMS%1-%2").arg(m_measChannelInfoHash.value(sl.at(0)).alias)
                                .arg(m_measChannelInfoHash.value(sl.at(1)).alias);

        m_EntityList.at(i)->setValue(s, m_pPeer);
    }
}


void cRmsModuleMeasProgram::setInterfaceActualValues(QVector<float> *actualValues)
{
    int i;
    if (m_bActive) // maybe we are deactivating !!!!
    {
        for (i = 0; i < m_EntityList.count()-1; i++) // we set n rms values
            m_EntityList.at(i)->setValue((*actualValues)[i], m_pPeer);
    }
}


void cRmsModuleMeasProgram::resourceManagerConnect()
{
    // as this is our entry point when activating the module, we do some initialization first
    m_measChannelInfoHash.clear(); // we build up a new channel info hash
    cMeasChannelInfo mi;
    mi.socket = m_ConfigData.m_PCBServerSocket; // the default from configuration file
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
    // and then we set connection resource manager interface's connection
    m_pRMInterface->setClient(m_pRMClient); //
    // todo insert timer for timeout

    connect(m_pRMInterface, SIGNAL(serverAnswer(quint32, quint8, QVariant)), this, SLOT(catchInterfaceAnswer(quint32, quint8, QVariant)));
}


void cRmsModuleMeasProgram::sendRMIdent()
{
    m_MsgNrCmdList[m_pRMInterface->rmIdent("RMSModule")] = RMSMEASPROGRAM::sendrmident;
}


void cRmsModuleMeasProgram::readResourceTypes()
{
    m_MsgNrCmdList[m_pRMInterface->getResourceTypes()] = RMSMEASPROGRAM::readresourcetypes;

}


void cRmsModuleMeasProgram::readResource()
{
    m_MsgNrCmdList[m_pRMInterface->getResources("SENSE")] = RMSMEASPROGRAM::readresource;
}


void cRmsModuleMeasProgram::readResourceInfos()
{
    channelInfoReadList = m_measChannelInfoHash.keys(); // we have to read information for all channels in this list
    emit activationContinue();
}


void cRmsModuleMeasProgram::readResourceInfo()
{
    channelInfoRead = channelInfoReadList.takeLast();
    m_MsgNrCmdList[m_pRMInterface->getResourceInfo("SENSE", channelInfoRead)] = RMSMEASPROGRAM::readresourceinfo;
}


void cRmsModuleMeasProgram::readResourceInfoDone()
{
    if (channelInfoReadList.isEmpty())
        emit activationContinue();
    else
        emit activationLoop();
}


void cRmsModuleMeasProgram::pcbserverConnect()
{
    // we have to connect to all ports....
    channelInfoReadList = m_measChannelInfoHash.keys(); // so first we look for our different pcb sockets
    m_nConnectionCount = channelInfoReadList.count();
    for (int i = 0; i < m_nConnectionCount; i++)
    {
        QString key = channelInfoReadList.at(i);
        cMeasChannelInfo mi = m_measChannelInfoHash.take(key);
        cSocket sock = mi.socket;
        Zera::Proxy::cProxyClient* pcbClient = m_pProxy->getConnection(sock.m_sIP, sock.m_nPort);
        m_pcbClientList.append(pcbClient);
        Zera::Server::cPCBInterface* pcbIFace = new Zera::Server::cPCBInterface();
        m_pcbIFaceList.append(pcbIFace);
        pcbIFace->setClient(pcbClient);
        mi.pcbIFace = pcbIFace;
        m_measChannelInfoHash[key] = mi;
        connect(pcbClient, SIGNAL(connected()), this, SLOT(monitorConnection())); // here we wait until all connections are established
        connect(pcbIFace, SIGNAL(serverAnswer(quint32, quint8, QVariant)), this, SLOT(catchInterfaceAnswer(quint32, quint8, QVariant)));
    }

    connect(m_pDSPIFace, SIGNAL(serverAnswer(quint32, quint8, QVariant)), this, SLOT(catchInterfaceAnswer(quint32, quint8, QVariant)));
}


void cRmsModuleMeasProgram::readSamplenr()
{
    // we always take the sample count from the first channels pcb server
    m_MsgNrCmdList[m_pcbIFaceList.at(0)->getSamples()] = RMSMEASPROGRAM::readsamplenr;
}


void cRmsModuleMeasProgram::readChannelInformation()
{
    channelInfoReadList = m_measChannelInfoHash.keys(); // we have to read information for all channels in this list
    emit activationContinue();
}


void cRmsModuleMeasProgram::readChannelAlias()
{
    channelInfoRead = channelInfoReadList.takeFirst();
    m_MsgNrCmdList[m_measChannelInfoHash[channelInfoRead].pcbIFace->getAlias(channelInfoRead)] = RMSMEASPROGRAM::readalias;
}


void cRmsModuleMeasProgram::readDspChannel()
{
    m_MsgNrCmdList[m_measChannelInfoHash[channelInfoRead].pcbIFace->getDSPChannel(channelInfoRead)] = RMSMEASPROGRAM::readdspchannel;
}


void cRmsModuleMeasProgram::readDspChannelDone()
{
    if (channelInfoReadList.isEmpty())
    {
        // now we have all information to setup our var and cmd lists
        setDspVarList(); // first we set the var list for our dsp
        setDspCmdList(); // and the cmd list he has to work on
        emit activationContinue();
    }
    else
        emit activationLoop();
}


void cRmsModuleMeasProgram::claimPGRMem()
{
  m_MsgNrCmdList[m_pRMInterface->setResource("DSP1", "PGRMEMC", m_pDSPIFace->cmdListCount())] = RMSMEASPROGRAM::claimpgrmem;
}


void cRmsModuleMeasProgram::claimUSERMem()
{
   m_MsgNrCmdList[m_pRMInterface->setResource("DSP1", "USERMEM", m_nDspMemUsed)] = RMSMEASPROGRAM::claimusermem;
}


void cRmsModuleMeasProgram::varList2DSP()
{
    m_MsgNrCmdList[m_pDSPIFace->varList2Dsp()] = RMSMEASPROGRAM::varlist2dsp;
}


void cRmsModuleMeasProgram::cmdList2DSP()
{
    m_MsgNrCmdList[m_pDSPIFace->cmdList2Dsp()] = RMSMEASPROGRAM::cmdlist2dsp;
}


void cRmsModuleMeasProgram::activateDSP()
{
    m_MsgNrCmdList[m_pDSPIFace->activateInterface()] = RMSMEASPROGRAM::activatedsp; // aktiviert die var- und cmd-listen im dsp
}


void cRmsModuleMeasProgram::activateDSPdone()
{
    m_bActive = true;

    m_pMeasureSignal->m_pParEntity->setValue(QVariant(1), m_pPeer);
    connect(m_pIntegrationTimeParameter, SIGNAL(updated(QVariant)), this, SLOT(newIntegrationtime(QVariant)));

    emit activated();
}


void cRmsModuleMeasProgram::deactivateDSP()
{
    m_bActive = false;
    deleteDspVarList();
    deleteDspCmdList();

    m_MsgNrCmdList[m_pDSPIFace->deactivateInterface()] = RMSMEASPROGRAM::deactivatedsp; // wat wohl
}


void cRmsModuleMeasProgram::freePGRMem()
{
    m_MsgNrCmdList[m_pRMInterface->freeResource("DSP1", "PGRMEMC")] = RMSMEASPROGRAM::freepgrmem;
}


void cRmsModuleMeasProgram::freeUSERMem()
{
    m_MsgNrCmdList[m_pRMInterface->freeResource("DSP1", "USERMEM")] = RMSMEASPROGRAM::freeusermem;
}


void cRmsModuleMeasProgram::deactivateDSPdone()
{
    disconnect(m_pRMInterface, 0, this, 0);
    disconnect(m_pDSPIFace, 0, this, 0);
    for (int i = 0; m_pcbIFaceList.count(); i++)
        disconnect(m_pcbIFaceList.at(i), 0 ,this, 0);
    emit deactivated();
}


void cRmsModuleMeasProgram::dataAcquisitionDSP()
{
    m_pMeasureSignal->m_pParEntity->setValue(QVariant(0), m_pPeer);
    m_MsgNrCmdList[m_pDSPIFace->dataAcquisition(m_pActualValuesDSP)] = RMSMEASPROGRAM::dataaquistion; // we start our data aquisition now
}


void cRmsModuleMeasProgram::dataReadDSP()
{
    m_pDSPIFace->getData(m_pActualValuesDSP, m_ModuleActualValues);
    m_pMeasureSignal->m_pParEntity->setValue(QVariant(1), m_pPeer);
    emit actualValues(&m_ModuleActualValues); // and send them
}


void cRmsModuleMeasProgram::newIntegrationtime(QVariant ti)
{
    bool ok;
    m_ConfigData.m_fMeasInterval.m_fValue = ti.toDouble(&ok);
    m_pDSPIFace->setVarData(m_pParameterDSP, QString("TIPAR:%1;TISTART:%2;").arg(m_ConfigData.m_fMeasInterval.m_fValue*1000)
                                                                            .arg(0), DSPDATA::dInt);
    m_MsgNrCmdList[m_pDSPIFace->dspMemoryWrite(m_pParameterDSP)] = RMSMEASPROGRAM::writeparameter;
}







