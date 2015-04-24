#include <rminterface.h>
#include <dspinterface.h>
#include <proxy.h>
#include <veinentity.h>

#include "modemodule.h"
#include "modemoduleconfiguration.h"
#include "modemoduleconfigdata.h"
#include "modemoduleinit.h"
#include "moduleinfo.h"
#include "moduleerror.h"
#include "modulesignal.h"

namespace MODEMODULE
{

cModeModule::cModeModule(quint8 modnr, Zera::Proxy::cProxy *proxy, VeinPeer* peer, QObject *parent)
    :cBaseModule(modnr, proxy, peer, new cModeModuleConfiguration(), parent)
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


cModeModule::~cModeModule()
{
    delete m_pConfiguration;
}


QByteArray cModeModule::getConfiguration()
{
    return m_pConfiguration->exportConfiguration();
}


void cModeModule::doConfiguration(QByteArray xmlConfigData)
{
    m_pConfiguration->setConfiguration(xmlConfigData);
}


void cModeModule::setupModule()
{
    cModeModuleConfigData *pConfData;
    pConfData = qobject_cast<cModeModuleConfiguration*>(m_pConfiguration)->getConfigurationData();

    // we only have to initialize the pcb's measuring mode
    m_pModeModuleInit = new cModeModuleInit(this, m_pProxy, m_pPeer, *pConfData);
    m_ModuleActivistList.append(m_pModeModuleInit);
    connect(m_pModeModuleInit, SIGNAL(activated()), this, SIGNAL(activationContinue()));
    connect(m_pModeModuleInit, SIGNAL(deactivated()), this, SIGNAL(deactivationContinue()));
    connect(m_pModeModuleInit, SIGNAL(errMsg(QString)), errorMessage, SLOT(appendMsg(QString)));

    for (int i = 0; i < m_ModuleActivistList.count(); i++)
        m_ModuleActivistList.at(i)->generateInterface();
}


void cModeModule::unsetModule()
{
    if (m_ModuleActivistList.count() > 0)
    {
        for (int i = 0; i < m_ModuleActivistList.count(); i++)
        {
            m_ModuleActivistList.at(i)->deleteInterface();
            delete m_ModuleActivistList.at(i);
        }
        m_ModuleActivistList.clear();
        if (errorMessage) delete errorMessage;
    }
}


void cModeModule::startMeas()
{
    // nothing to start here
}


void cModeModule::stopMeas()
{
    // also nothing to stop
}


void cModeModule::activationStart()
{
    m_nActivationIt = 0; // we start with the first
    emit activationContinue();
}


void cModeModule::activationExec()
{
    m_ModuleActivistList.at(m_nActivationIt)->activate();
}


void cModeModule::activationDone()
{
    m_nActivationIt++;

    if (m_nActivationIt < m_ModuleActivistList.count())
        emit activationNext(); // and iterate over our list
    else
        emit activationContinue();
}


void cModeModule::activationFinished()
{
    //we are "only" ready
    emit activationReady();
}


void cModeModule::deactivationStart()
{
    m_nActivationIt = 0; // we start with the first
    emit deactivationContinue();
}


void cModeModule::deactivationExec()
{
    m_ModuleActivistList.at(m_nActivationIt)->deactivate();
}


void cModeModule::deactivationDone()
{
    m_nActivationIt++;

    if (m_nActivationIt < m_ModuleActivistList.count())
        emit deactivationNext(); // and iterate over our list
    else
        emit deactivationContinue();
}


void cModeModule::deactivationFinished()
{
    emit deactivationReady();
}

}
