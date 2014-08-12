#include <rminterface.h>
#include <dspinterface.h>
#include <proxy.h>
#include <veinentity.h>

#include "rangemodule.h"
#include "rangemoduleconfiguration.h"
#include "rangemoduleconfigdata.h"
#include "rangemeaschannel.h"
#include "rangemodulemeasprogram.h"
#include "rangemoduleobservation.h"
#include "adjustment.h"
#include "rangeobsermatic.h"
#include "moduleparameter.h"
#include "moduleinfo.h"
#include "modulesignal.h"

cRangeModule::cRangeModule(quint8 modnr, Zera::Proxy::cProxy *proxy, VeinPeer* peer, QObject *parent)
    :cBaseModule(modnr, proxy, peer, new cRangeModuleConfiguration(), parent)
{
    m_ModuleActivistList.clear();

    m_ActivationStartState.addTransition(this, SIGNAL(activationContinue()), &m_ActivationExecState);
    m_ActivationExecState.addTransition(this, SIGNAL(activationContinue()), &m_ActivationDoneState);
    m_ActivationDoneState.addTransition(this, SIGNAL(activationNext()), &m_ActivationExecState);
    m_ActivationDoneState.addTransition(this, SIGNAL(activationContinue()), &m_ActivationFinishedState);
    m_ActivationMachine.addState(&m_ActivationStartState);
    m_ActivationMachine.addState(&m_ActivationExecState);
    m_ActivationMachine.addState(&m_ActivationDoneState);
    m_ActivationMachine.addState(&m_ActivationFinishedState);
    m_ActivationMachine.setInitialState(&m_ActivationStartState);
    connect(&m_ActivationStartState, SIGNAL(entered()), SLOT(activationStart()));
    connect(&m_ActivationExecState, SIGNAL(entered()), SLOT(activationExec()));
    connect(&m_ActivationDoneState, SIGNAL(entered()), SLOT(activationDone()));
    connect(&m_ActivationFinishedState, SIGNAL(entered()), SLOT(activationFinished()));

    m_DeactivationStartState.addTransition(this, SIGNAL(deactivationContinue()), &m_DeactivationExecState);
    m_DeactivationExecState.addTransition(this, SIGNAL(deactivationContinue()), &m_DeactivationDoneState);
    m_DeactivationDoneState.addTransition(this, SIGNAL(deactivationNext()), &m_DeactivationExecState);
    m_DeactivationDoneState.addTransition(this, SIGNAL(deactivationContinue()), &m_DeactivationFinishedState);
    m_DeactivationMachine.addState(&m_DeactivationStartState);
    m_DeactivationMachine.addState(&m_DeactivationExecState);
    m_DeactivationMachine.addState(&m_DeactivationDoneState);
    m_DeactivationMachine.addState(&m_DeactivationFinishedState);
    m_DeactivationMachine.setInitialState(&m_DeactivationStartState);
    connect(&m_DeactivationStartState, SIGNAL(entered()), SLOT(deactivationStart()));
    connect(&m_DeactivationExecState, SIGNAL(entered()), SLOT(deactivationExec()));
    connect(&m_DeactivationDoneState, SIGNAL(entered()), SLOT(deactivationDone()));
    connect(&m_DeactivationFinishedState, SIGNAL(entered()), SLOT(deactivationFinished()));

}


cRangeModule::~cRangeModule()
{
    delete m_pConfiguration;
    unsetModule();
}


QByteArray cRangeModule::getConfiguration()
{
    return m_pConfiguration->exportConfiguration();
}


cRangeMeasChannel *cRangeModule::getMeasChannel(QString name)
{
    cRangeMeasChannel* p_rmc = 0;
    for (int i = 0; i < m_rangeMeasChannelList.count(); i++)
    {
        p_rmc =  m_rangeMeasChannelList.at(i);
        if ((p_rmc->getName()) == name)
            return p_rmc;
    }

    return p_rmc;
}


void cRangeModule::doConfiguration(QByteArray xmlConfigData)
{
    m_pConfiguration->setConfiguration(xmlConfigData);
}


void cRangeModule::setupModule()
{
    cRangeModuleConfigData *pConfData;
    pConfData = qobject_cast<cRangeModuleConfiguration*>(m_pConfiguration)->getConfigurationData();

    channelNrInfo = new cModuleInfo(m_pPeer, "INF_ChannelCount", QVariant(pConfData->m_nChannelCount));
    groupNrInfo = new cModuleInfo(m_pPeer, "INF_GroupCount", QVariant(pConfData->m_nGroupCount));

    // first we build a list of our meas channels
    for (int i = 0; i < pConfData->m_nChannelCount; i ++)
    {
        cRangeMeasChannel* pchn = new cRangeMeasChannel(m_pProxy, m_pPeer, &(pConfData->m_RMSocket),
                                                        &(pConfData->m_PCBServerSocket),
                                                        pConfData->m_senseChannelList.at(i), i+1);
        m_rangeMeasChannelList.append(pchn);
        m_ModuleActivistList.append(pchn);
        QObject::connect(pchn, SIGNAL(activated()), this, SIGNAL(activationContinue()));
        QObject::connect(pchn, SIGNAL(deactivated()), this, SIGNAL(deactivationContinue()));
    }

    m_pDspProxyClient = m_pProxy->getConnection(pConfData->m_DSPServerSocket.m_sIP, pConfData->m_DSPServerSocket.m_nPort);
    m_pDSPInterface = new Zera::Server::cDSPInterface();
    m_pDSPInterface->setClient(m_pDspProxyClient);

    // we need some program that does the range handling (observation, automatic, setting and grouping)
    // it will also do the scaling job
    m_pRangeObsermatic = new cRangeObsermatic(this, m_pPeer, m_pDSPInterface, pConfData->m_GroupList, pConfData->m_senseChannelList, pConfData->m_ObsermaticConfPar);
    m_ModuleActivistList.append(m_pRangeObsermatic);
    connect(m_pRangeObsermatic, SIGNAL(activated()), SIGNAL(activationContinue()));
    connect(m_pRangeObsermatic, SIGNAL(deactivated()), this, SIGNAL(deactivationContinue()));

    // we also need some program for adjustment
    m_pAdjustment = new cAdjustManagement(this, m_pPeer, m_pDSPInterface, pConfData->m_senseChannelList, pConfData->m_fAdjInterval);
    m_ModuleActivistList.append(m_pAdjustment);
    connect(m_pAdjustment, SIGNAL(activated()), SIGNAL(activationContinue()));
    connect(m_pAdjustment, SIGNAL(deactivated()), this, SIGNAL(deactivationContinue()));

    // at last we need some program that does the measuring on dsp
    m_pMeasProgram = new cRangeModuleMeasProgram(this, m_pProxy, m_pPeer, m_pDSPInterface, *pConfData);
    m_ModuleActivistList.append(m_pMeasProgram);
    connect(m_pMeasProgram, SIGNAL(activated()), SIGNAL(activationContinue()));
    connect(m_pMeasProgram, SIGNAL(deactivated()), this, SIGNAL(deactivationContinue()));

    //
    m_pRangeModuleObservation = new cRangeModuleObservation(this, m_pProxy, &(pConfData->m_PCBServerSocket));
    m_ModuleActivistList.append(m_pRangeModuleObservation);
    connect(m_pRangeModuleObservation, SIGNAL(activated()), SIGNAL(activationContinue()));
    connect(m_pRangeModuleObservation, SIGNAL(deactivated()), this, SIGNAL(deactivationContinue()));

    for (int i = 0; i < m_ModuleActivistList.count(); i++)
        m_ModuleActivistList.at(i)->generateInterface();
}


void cRangeModule::unsetModule()
{
    if (m_ModuleActivistList.count() > 0)
    {
        for (int i = 0; i < m_ModuleActivistList.count(); i++)
        {
            m_ModuleActivistList.at(i)->deleteInterface();
            m_ModuleActivistList.at(i)->deleteLater();
        }
        m_ModuleActivistList.clear();
        m_rangeMeasChannelList.clear();
        if (channelNrInfo) delete channelNrInfo;
        if (groupNrInfo) delete groupNrInfo;
        if (m_pDSPInterface)
        {
            m_pProxy->releaseConnection(m_pDspProxyClient);
            delete m_pDSPInterface;
        }
    }
}


void cRangeModule::startMeas()
{
    m_pMeasProgram->start();
}


void cRangeModule::stopMeas()
{
    m_pMeasProgram->stop();
}


void cRangeModule::activationStart()
{
    m_nActivationIt = 0; // we start with the first
    emit activationContinue();
}


void cRangeModule::activationExec()
{
    m_ModuleActivistList.at(m_nActivationIt)->activate();
}


void cRangeModule::activationDone()
{
    m_nActivationIt++;

    if (m_nActivationIt < m_ModuleActivistList.count())
        emit activationNext(); // and iterate over our list
    else
        emit activationContinue();
}


void cRangeModule::activationFinished()
{
    // we connect the measurement output to our adjustment module
    connect(m_pMeasProgram, SIGNAL(actualValues(QVector<float>*)), m_pAdjustment, SLOT(ActionHandler(QVector<float>*)));
    // and to the range obsermatic
    connect(m_pMeasProgram, SIGNAL(actualValues(QVector<float>*)), m_pRangeObsermatic, SLOT(ActionHandler(QVector<float>*)));
    // we connect a signal that range has changed to measurement for synchronizing purpose
    connect(m_pRangeObsermatic->m_pRangingSignal->m_pParEntity, SIGNAL(sigValueChanged(QVariant)),  m_pMeasProgram, SLOT(syncRanging(QVariant)));

    // we have to connect all cmddone from our meas channel to range obsermatic
    // this is also used for synchronizing purpose
    for (int i = 0; i < m_rangeMeasChannelList.count(); i ++)
    {
        cRangeMeasChannel* pchn = m_rangeMeasChannelList.at(i);
        connect(pchn, SIGNAL(cmdDone(quint32)), m_pRangeObsermatic, SLOT(catchChannelReply(quint32)));
    }

    // if we get informed we have to reconfigure
    connect(m_pRangeModuleObservation, SIGNAL(moduleReconfigure()), this, SLOT(rangeModuleReconfigure()));

    emit activationReady();
}


void cRangeModule::deactivationStart()
{
    // we first disconnect all what we connected when activation took place
    disconnect(m_pMeasProgram, SIGNAL(actualValues(QVector<float>*)), m_pAdjustment, SLOT(ActionHandler(QVector<float>*)));
    disconnect(m_pMeasProgram, SIGNAL(actualValues(QVector<float>*)), m_pRangeObsermatic, SLOT(ActionHandler(QVector<float>*)));
    disconnect(m_pRangeObsermatic->m_pRangingSignal->m_pParEntity, SIGNAL(sigValueChanged(QVariant)),  m_pMeasProgram, SLOT(syncRanging(QVariant)));

    for (int i = 0; i < m_rangeMeasChannelList.count(); i ++)
    {
        cRangeMeasChannel* pchn = m_rangeMeasChannelList.at(i);
        disconnect(pchn, SIGNAL(cmdDone(quint32)), m_pRangeObsermatic, SLOT(catchChannelReply(quint32)));
    }

    // if we get informed we have to reconfigure
    disconnect(m_pRangeModuleObservation, SIGNAL(moduleReconfigure()), this, SLOT(rangeModuleReconfigure()));

    m_nActivationIt = 0; // we start with the first
    emit deactivationContinue();
}


void cRangeModule::deactivationExec()
{
    m_ModuleActivistList.at(m_nActivationIt)->deactivate();
}


void cRangeModule::deactivationDone()
{
    m_nActivationIt++;

    if (m_nActivationIt < m_ModuleActivistList.count())
        emit deactivationNext(); // and iterate over our list
    else
        emit deactivationContinue();
}


void cRangeModule::deactivationFinished()
{
    emit deactivationReady();
}


void cRangeModule::rangeModuleReconfigure()
{
    emit sigConfiguration(); // we configure after our notifier has detected
}


