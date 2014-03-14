#include <rminterface.h>
#include <dspinterface.h>
#include <proxy.h>

#include "rangemodule.h"
#include "rangemoduleconfiguration.h"
#include "rangemoduleconfigdata.h"
#include "rangemeaschannel.h"
#include "rangemodulemeasprogram.h"
#include "justifynorm.h"
#include "rangeobsermatic.h"
#include "moduleparameter.h"
#include "moduleinfo.h"

cRangeModule::cRangeModule(Zera::Proxy::cProxy *proxy, VeinPeer* peer, QObject *parent)
    :cBaseModule(proxy, peer, new cRangeModuleConfiguration(), parent)
{
    m_ActivationStartState.addTransition(this, SIGNAL(activationContinue()), &m_ActivationExecState);
    m_ActivationExecState.addTransition(this, SIGNAL(activationContinue()), &m_ActivationDoneState);
    m_ActivationDoneState.addTransition(this, SIGNAL(activationNext()), &m_ActivationExecState);

    m_ActivationMachine.addState(&m_ActivationStartState);
    m_ActivationMachine.addState(&m_ActivationExecState);
    m_ActivationMachine.addState(&m_ActivationDoneState);
    m_ActivationMachine.setInitialState(&m_ActivationStartState);

    connect(&m_ActivationStartState, SIGNAL(entered()), SLOT(activationStart()));
    connect(&m_ActivationExecState, SIGNAL(entered()), SLOT(activationExec()));
    connect(&m_ActivationDoneState, SIGNAL(entered()), SLOT(activationDone()));
}


cRangeModule::~cRangeModule()
{
    delete m_pConfiguration;
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
    if (m_rangeMeasChannelList.count()) // did we already setupModule(), then we have to clean up
    {
        int i;

        for (i = 0; i < m_rangeMeasChannelList.count(); i++)
            delete m_rangeMeasChannelList.at(i);
        m_rangeMeasChannelList.clear();

        delete m_pMeasProgram;
        delete m_pJustifyNorm;
        delete m_pRangeObsermatic;
        m_ModuleActivistList.clear();
    }

    cRangeModuleConfigData *pConfData;
    pConfData = qobject_cast<cRangeModuleConfiguration*>(m_pConfiguration)->getConfigurationData();

    // first we build a list of our meas channels
    for (int i = 0; i < pConfData->m_nChannelCount; i ++)
    {
        cRangeMeasChannel* pchn = new cRangeMeasChannel(m_pProxy, m_pPeer, &(pConfData->m_RMSocket),
                                                        &(pConfData->m_PCBServerSocket),
                                                        pConfData->m_senseChannelList.at(i), i+1);
        m_rangeMeasChannelList.append(pchn);
        m_ModuleActivistList.append(pchn);
        QObject::connect(pchn, SIGNAL(activated()), this, SIGNAL(activationContinue()));
    }

    m_pDSPInterface = new Zera::Server::cDSPInterface();
    m_pDSPInterface->setClient(m_pProxy->getConnection(pConfData->m_DSPServerSocket.m_sIP, pConfData->m_DSPServerSocket.m_nPort));

    // then we need some program that does the measuring on dsp
    m_pMeasProgram = new cRangeModuleMeasProgram(this, m_pPeer, m_pDSPInterface, &(pConfData->m_RMSocket), pConfData->m_senseChannelList);
    m_ModuleActivistList.append(m_pMeasProgram);
    connect(m_pMeasProgram, SIGNAL(activated()), SIGNAL(activationContinue()));

    // we also need some program for adjustment and normation
    m_pJustifyNorm = new cJustifyNormManagement(this, m_pPeer, m_pDSPInterface, pConfData->m_senseChannelList);
    m_ModuleActivistList.append(m_pJustifyNorm);
    connect(m_pJustifyNorm, SIGNAL(activated()), SIGNAL(activationContinue()));

    // and at last something that does the range handling (observation, automatic, setting and grouping)
    m_pRangeObsermatic = new cRangeObsermatic(this, m_pPeer, pConfData->m_GroupList, pConfData->m_senseChannelList);
    m_ModuleActivistList.append(m_pRangeObsermatic);
    connect(m_pRangeObsermatic, SIGNAL(activated()), SIGNAL(activationContinue()));

}


void cRangeModule::doInitialization()
{
    m_ActivationMachine.start(); // we activate all our activists step by step by statemachine
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
    {
        m_ActivationMachine.stop();
        emit sigInitDone();
    }
}


