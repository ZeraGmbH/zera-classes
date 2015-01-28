#include <rminterface.h>
#include <dspinterface.h>
#include <proxy.h>
#include <veinentity.h>

#include "samplemodule.h"
#include "samplemoduleconfiguration.h"
#include "samplemoduleconfigdata.h"
#include "samplechannel.h"
#include "pllmeaschannel.h"
#include "samplemodulemeasprogram.h"
#include "samplemoduleobservation.h"
#include "pllobsermatic.h"
#include "moduleerror.h"


namespace SAMPLEMODULE
{

cSampleModule::cSampleModule(quint8 modnr, Zera::Proxy::cProxy *proxy, VeinPeer* peer, QObject *parent)
    :cBaseModule(modnr, proxy, peer, new cSampleModuleConfiguration(), parent)
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


cSampleModule::~cSampleModule()
{
    delete m_pConfiguration;
}


QByteArray cSampleModule::getConfiguration()
{
    return m_pConfiguration->exportConfiguration();
}


cPllMeasChannel* cSampleModule::getPllMeasChannel(QString name)
{
    cPllMeasChannel* p_pllmc = 0;
    for (int i = 0; i < m_pllMeasChannelList.count(); i++)
    {
        p_pllmc =  m_pllMeasChannelList.at(i);
        if ((p_pllmc->getName()) == name)
            return p_pllmc;
    }

    return p_pllmc;
}


void cSampleModule::doConfiguration(QByteArray xmlConfigData)
{
    m_pConfiguration->setConfiguration(xmlConfigData);
}


void cSampleModule::setupModule()
{
    cSampleModuleConfigData *pConfData;
    pConfData = qobject_cast<cSampleModuleConfiguration*>(m_pConfiguration)->getConfigurationData();

    errorMessage = new cModuleError(m_pPeer, "ERR_Message");

    // first we build a list of our pll meas channels, that hold informations for other activists
    for (int i = 0; i < pConfData->m_ObsermaticConfPar.m_npllChannelCount; i ++)
    {
        cPllMeasChannel* pllchn = new cPllMeasChannel(m_pProxy, m_pPeer, &(pConfData->m_RMSocket),
                                                        &(pConfData->m_PCBServerSocket),
                                                        pConfData->m_ObsermaticConfPar.m_pllChannelList.at(i), i+1);
        m_pllMeasChannelList.append(pllchn);
        m_ModuleActivistList.append(pllchn);
        connect(pllchn, SIGNAL(activated()), this, SIGNAL(activationContinue()));
        connect(pllchn, SIGNAL(deactivated()), this, SIGNAL(deactivationContinue()));
        connect(pllchn, SIGNAL(errMsg(QString)), errorMessage, SLOT(appendMsg(QString)));
    }

    // next we instantiate an object of sample channel so we can switch sample frequency ranges
    cSampleChannel* schn = new cSampleChannel(m_pProxy, m_pPeer, &(pConfData->m_RMSocket),
                                                        &(pConfData->m_PCBServerSocket),
                                                        pConfData->m_ObsermaticConfPar.m_sSampleSystem, 1);

    m_sampleChannelList.append(schn); // we hold a list although we only have 1
    m_ModuleActivistList.append(schn);
    connect(schn, SIGNAL(activated()), this, SIGNAL(activationContinue()));
    connect(schn, SIGNAL(deactivated()), this, SIGNAL(deactivationContinue()));
    connect(schn, SIGNAL(errMsg(QString)), errorMessage, SLOT(appendMsg(QString)));

    // we need some program that does the pll handling (observation, automatic, setting)
    m_pPllObsermatic = new cPllObsermatic(this, m_pProxy, m_pPeer, *pConfData);
    m_ModuleActivistList.append(m_pPllObsermatic);
    connect(m_pPllObsermatic, SIGNAL(activated()), SIGNAL(activationContinue()));
    connect(m_pPllObsermatic, SIGNAL(deactivated()), this, SIGNAL(deactivationContinue()));
    connect(m_pPllObsermatic, SIGNAL(errMsg(QString)), errorMessage, SLOT(appendMsg(QString)));

    // at last we need some program that does the measuring on dsp
    m_pMeasProgram = new cSampleModuleMeasProgram(this, m_pProxy, m_pPeer, *pConfData);
    m_ModuleActivistList.append(m_pMeasProgram);
    connect(m_pMeasProgram, SIGNAL(activated()), SIGNAL(activationContinue()));
    connect(m_pMeasProgram, SIGNAL(deactivated()), this, SIGNAL(deactivationContinue()));
    connect(m_pMeasProgram, SIGNAL(errMsg(QString)), errorMessage, SLOT(appendMsg(QString)));
    //
    m_pSampleModuleObservation = new cSampleModuleObservation(this, m_pProxy, &(pConfData->m_PCBServerSocket));
    m_ModuleActivistList.append(m_pSampleModuleObservation);
    connect(m_pSampleModuleObservation, SIGNAL(activated()), SIGNAL(activationContinue()));
    connect(m_pSampleModuleObservation, SIGNAL(deactivated()), this, SIGNAL(deactivationContinue()));
    connect(m_pSampleModuleObservation, SIGNAL(errMsg(QString)), errorMessage, SLOT(appendMsg(QString)));

    for (int i = 0; i < m_ModuleActivistList.count(); i++)
        m_ModuleActivistList.at(i)->generateInterface();
}


void cSampleModule::unsetModule()
{
    if (m_ModuleActivistList.count() > 0)
    {
        for (int i = 0; i < m_ModuleActivistList.count(); i++)
        {
            m_ModuleActivistList.at(i)->deleteInterface();
            delete m_ModuleActivistList.at(i);
        }
        m_ModuleActivistList.clear();
        m_pllMeasChannelList.clear();
        if (errorMessage) delete errorMessage;
    }
}


void cSampleModule::startMeas()
{
    m_pMeasProgram->start();
}


void cSampleModule::stopMeas()
{
    m_pMeasProgram->stop();
}


void cSampleModule::activationStart()
{
    m_nActivationIt = 0; // we start with the first
    emit activationContinue();
}


void cSampleModule::activationExec()
{
    m_ModuleActivistList.at(m_nActivationIt)->activate();
}


void cSampleModule::activationDone()
{
    m_nActivationIt++;

    if (m_nActivationIt < m_ModuleActivistList.count())
        emit activationNext(); // and iterate over our list
    else
        emit activationContinue();
}


void cSampleModule::activationFinished()
{
    // we connect the measurement output to our pll obsermatic module
    connect(m_pMeasProgram, SIGNAL(actualValues(QVector<float>*)), m_pPllObsermatic, SLOT(ActionHandler(QVector<float>*)));

    // if we get informed we have to reconfigure
    connect(m_pSampleModuleObservation, SIGNAL(moduleReconfigure()), this, SLOT(sampleModuleReconfigure()));

    // we have to connect all cmddone from our pll meas channel to pll obsermatic
    // this is also used for synchronizing purpose
    for (int i = 0; i < m_pllMeasChannelList.count(); i ++)
    {
        cPllMeasChannel* pllchn = m_pllMeasChannelList.at(i);
        connect(pllchn, SIGNAL(cmdDone(quint32)), m_pPllObsermatic, SLOT(catchChannelReply(quint32)));
    }

    emit activationReady();
}


void cSampleModule::deactivationStart()
{
    // we first disconnect all what we connected when activation took place
    disconnect(m_pMeasProgram, SIGNAL(actualValues(QVector<float>*)), m_pPllObsermatic, SLOT(ActionHandler(QVector<float>*)));

    // if we get informed we have to reconfigure
    disconnect(m_pSampleModuleObservation, SIGNAL(moduleReconfigure()), this, SLOT(sampleModuleReconfigure()));

    m_nActivationIt = 0; // we start with the first
    emit deactivationContinue();
}


void cSampleModule::deactivationExec()
{
    m_ModuleActivistList.at(m_nActivationIt)->deactivate();
}


void cSampleModule::deactivationDone()
{
    m_nActivationIt++;

    if (m_nActivationIt < m_ModuleActivistList.count())
        emit deactivationNext(); // and iterate over our list
    else
        emit deactivationContinue();
}


void cSampleModule::deactivationFinished()
{
    emit deactivationReady();
}


void cSampleModule::sampleModuleReconfigure()
{
    emit sigConfiguration(); // we configure after our notifier has detected
}

}
