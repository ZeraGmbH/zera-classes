#include <rminterface.h>
#include <dspinterface.h>
#include <proxy.h>
#include <modulevalidator.h>
#include <veinmodulecomponent.h>
#include <veinmoduleerrorcomponent.h>
#include <veinmodulemetadata.h>

#include "statusmodule.h"
#include "statusmoduleconfiguration.h"
#include "statusmoduleconfigdata.h"
#include "statusmoduleinit.h"


namespace STATUSMODULE
{

cStatusModule::cStatusModule(quint8 modnr, Zera::Proxy::cProxy *proxy, int entityId, VeinEvent::StorageSystem* storagesystem, QObject *parent)
    :cBaseMeasModule(modnr, proxy, entityId, storagesystem, new cStatusModuleConfiguration(), parent)
{
    m_sModuleName = QString("%1%2").arg(BaseModuleName).arg(modnr);
    m_sModuleDescription = QString("This module is responsible for reading and providing system Status information");
    m_sSCPIModuleName = QString("%1%2").arg(BaseSCPIModuleName).arg(modnr);

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


cStatusModule::~cStatusModule()
{
    delete m_pConfiguration;
}


QByteArray cStatusModule::getConfiguration() const
{
    return m_pConfiguration->exportConfiguration();
}


void cStatusModule::doConfiguration(QByteArray xmlConfigData)
{
    m_pConfiguration->setConfiguration(xmlConfigData);
}


void cStatusModule::setupModule()
{
    emit addEventSystem(m_pModuleValidator);
    cBaseMeasModule::setupModule();

    cStatusModuleConfigData *pConfData;
    pConfData = qobject_cast<cStatusModuleConfiguration*>(m_pConfiguration)->getConfigurationData();

    // we only have to read some status information from pcb- and dspserver
    m_pStatusModuleInit = new cStatusModuleInit(this, m_pProxy, *pConfData);
    m_ModuleActivistList.append(m_pStatusModuleInit);
    connect(m_pStatusModuleInit, SIGNAL(activated()), this, SIGNAL(activationContinue()));
    connect(m_pStatusModuleInit, SIGNAL(deactivated()), this, SIGNAL(deactivationContinue()));
    connect(m_pStatusModuleInit, SIGNAL(errMsg(QVariant)), m_pModuleErrorComponent, SLOT(setValue(QVariant)));

    for (int i = 0; i < m_ModuleActivistList.count(); i++)
        m_ModuleActivistList.at(i)->generateInterface();
}


void cStatusModule::startMeas()
{
    // nothing to start here
}


void cStatusModule::stopMeas()
{
    // also nothing to stop
}


void cStatusModule::activationStart()
{
    m_nActivationIt = 0; // we start with the first
    emit activationContinue();
}


void cStatusModule::activationExec()
{
    m_ModuleActivistList.at(m_nActivationIt)->activate();
}


void cStatusModule::activationDone()
{
    m_nActivationIt++;

    if (m_nActivationIt < m_ModuleActivistList.count())
        emit activationNext(); // and iterate over our list
    else
        emit activationContinue();
}


void cStatusModule::activationFinished()
{
    // now we still have to export the json interface information, then we are ready
    exportMetaData();
    m_pModuleValidator->setParameterHash(veinModuleParameterHash);
    emit activationReady();
}


void cStatusModule::deactivationStart()
{
    m_nActivationIt = 0; // we start with the first
    emit deactivationContinue();
}


void cStatusModule::deactivationExec()
{
    m_ModuleActivistList.at(m_nActivationIt)->deactivate();
}


void cStatusModule::deactivationDone()
{
    m_nActivationIt++;

    if (m_nActivationIt < m_ModuleActivistList.count())
        emit deactivationNext(); // and iterate over our list
    else
        emit deactivationContinue();
}


void cStatusModule::deactivationFinished()
{
    emit deactivationReady();
}

}
