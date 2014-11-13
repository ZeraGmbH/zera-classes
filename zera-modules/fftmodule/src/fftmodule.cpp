#include <rminterface.h>
#include <dspinterface.h>
#include <proxy.h>

#include "fftmodule.h"
#include "fftmoduleconfiguration.h"
#include "fftmoduleconfigdata.h"
#include "fftmodulemeasprogram.h"
#include "fftmoduleobservation.h"
#include "moduleparameter.h"
#include "moduleinfo.h"
#include "modulesignal.h"
#include "moduleerror.h"

namespace FFTMODULE
{

cFftModule::cFftModule(quint8 modnr, Zera::Proxy::cProxy *proxy, VeinPeer* peer, QObject *parent)
    :cBaseModule(modnr, proxy, peer, new cFftModuleConfiguration(), parent)
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


cFftModule::~cFftModule()
{
    delete m_pConfiguration;
    unsetModule();
}


QByteArray cFftModule::getConfiguration()
{
    return m_pConfiguration->exportConfiguration();
}



void cFftModule::doConfiguration(QByteArray xmlConfigData)
{
    m_pConfiguration->setConfiguration(xmlConfigData);
}


void cFftModule::setupModule()
{
    cFftModuleConfigData* pConfData;
    pConfData = qobject_cast<cFftModuleConfiguration*>(m_pConfiguration)->getConfigurationData();

    errorMessage = new ::cModuleError(m_pPeer, "ERR_Message");

    // we need some program that does the measuring on dsp
    m_pMeasProgram = new cFftModuleMeasProgram(this, m_pProxy, m_pPeer, *pConfData);
    m_ModuleActivistList.append(m_pMeasProgram);
    connect(m_pMeasProgram, SIGNAL(activated()), SIGNAL(activationContinue()));
    connect(m_pMeasProgram, SIGNAL(deactivated()), this, SIGNAL(deactivationContinue()));
    connect(m_pMeasProgram, SIGNAL(errMsg(QString)), errorMessage, SLOT(appendMsg(QString)));

    // and module observation in case we have to react to naming changes
    m_pFftModuleObservation = new cFftModuleObservation(this, m_pProxy, &(pConfData->m_PCBServerSocket));
    m_ModuleActivistList.append(m_pFftModuleObservation);
    connect(m_pFftModuleObservation, SIGNAL(activated()), SIGNAL(activationContinue()));
    connect(m_pFftModuleObservation, SIGNAL(deactivated()), this, SIGNAL(deactivationContinue()));
    connect(m_pFftModuleObservation, SIGNAL(errMsg(QString)), errorMessage, SLOT(appendMsg(QString)));

    for (int i = 0; i < m_ModuleActivistList.count(); i++)
        m_ModuleActivistList.at(i)->generateInterface();
}


void cFftModule::unsetModule()
{
    if (m_ModuleActivistList.count() > 0)
    {
        for (int i = 0; i < m_ModuleActivistList.count(); i++)
        {
            m_ModuleActivistList.at(i)->deleteInterface();
            m_ModuleActivistList.at(i)->deleteLater();
        }
        m_ModuleActivistList.clear();
    }
}


void cFftModule::startMeas()
{
    m_pMeasProgram->start();
}


void cFftModule::stopMeas()
{
    m_pMeasProgram->stop();
}


void cFftModule::activationStart()
{
    m_nActivationIt = 0; // we start with the first
    emit activationContinue();
}


void cFftModule::activationExec()
{
    m_ModuleActivistList.at(m_nActivationIt)->activate();
}


void cFftModule::activationDone()
{
    m_nActivationIt++;

    if (m_nActivationIt < m_ModuleActivistList.count())
        emit activationNext(); // and iterate over our list
    else
        emit activationContinue();
}


void cFftModule::activationFinished()
{
    // if we get informed we have to reconfigure
    connect(m_pFftModuleObservation, SIGNAL(moduleReconfigure()), this, SLOT(fftModuleReconfigure()));

    emit activationReady();
}


void cFftModule::deactivationStart()
{
    // if we get informed we have to reconfigure
    disconnect(m_pFftModuleObservation, SIGNAL(moduleReconfigure()), this, SLOT(fftModuleReconfigure()));

    m_nActivationIt = 0; // we start with the first
    emit deactivationContinue();
}


void cFftModule::deactivationExec()
{
    m_ModuleActivistList.at(m_nActivationIt)->deactivate();
}


void cFftModule::deactivationDone()
{
    m_nActivationIt++;

    if (m_nActivationIt < m_ModuleActivistList.count())
        emit deactivationNext(); // and iterate over our list
    else
        emit deactivationContinue();
}


void cFftModule::deactivationFinished()
{
    emit deactivationReady();
}


void cFftModule::fftModuleReconfigure()
{
    emit sigConfiguration(); // we configure after our notifier has detected
}

}
