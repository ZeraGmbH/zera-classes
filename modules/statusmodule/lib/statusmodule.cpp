#include "statusmodule.h"
#include "statusmoduleconfiguration.h"
#include "statusmoduleconfigdata.h"
#include "statusmoduleinit.h"

namespace STATUSMODULE
{

cStatusModule::cStatusModule(ModuleFactoryParam moduleParam) :
    cBaseMeasModule(moduleParam, std::shared_ptr<BaseModuleConfiguration>(new cStatusModuleConfiguration()))
{
    m_sModuleName = QString("%1%2").arg(BaseModuleName).arg(moduleParam.m_moduleNum);
    m_sModuleDescription = QString("This module is responsible for reading and providing system Status information");
    m_sSCPIModuleName = QString("%1%2").arg(BaseSCPIModuleName).arg(moduleParam.m_moduleNum);

    m_ActivationStartState.addTransition(this, &cStatusModule::activationContinue, &m_ActivationExecState);
    m_ActivationExecState.addTransition(this, &cStatusModule::activationContinue, &m_ActivationDoneState);
    m_ActivationDoneState.addTransition(this, &cStatusModule::activationNext, &m_ActivationExecState);
    m_ActivationDoneState.addTransition(this, &cStatusModule::activationContinue, &m_ActivationFinishedState);
    m_ActivationMachine.addState(&m_ActivationStartState);
    m_ActivationMachine.addState(&m_ActivationExecState);
    m_ActivationMachine.addState(&m_ActivationDoneState);
    m_ActivationMachine.addState(&m_ActivationFinishedState);
    m_ActivationMachine.setInitialState(&m_ActivationStartState);
    connect(&m_ActivationStartState, &QState::entered, this, &cStatusModule::activationStart);
    connect(&m_ActivationExecState, &QState::entered, this, &cStatusModule::activationExec);
    connect(&m_ActivationDoneState, &QState::entered, this, &cStatusModule::activationDone);
    connect(&m_ActivationFinishedState, &QState::entered, this, &cStatusModule::activationFinished);

    m_DeactivationStartState.addTransition(this, &cStatusModule::deactivationContinue, &m_DeactivationExecState);
    m_DeactivationExecState.addTransition(this, &cStatusModule::deactivationContinue, &m_DeactivationDoneState);
    m_DeactivationDoneState.addTransition(this, &cStatusModule::deactivationNext, &m_DeactivationExecState);
    m_DeactivationDoneState.addTransition(this, &cStatusModule::deactivationContinue, &m_DeactivationFinishedState);
    m_DeactivationMachine.addState(&m_DeactivationStartState);
    m_DeactivationMachine.addState(&m_DeactivationExecState);
    m_DeactivationMachine.addState(&m_DeactivationDoneState);
    m_DeactivationMachine.addState(&m_DeactivationFinishedState);
    m_DeactivationMachine.setInitialState(&m_DeactivationStartState);
    connect(&m_DeactivationStartState, &QState::entered, this, &cStatusModule::deactivationStart);
    connect(&m_DeactivationExecState, &QState::entered, this, &cStatusModule::deactivationExec);
    connect(&m_DeactivationDoneState, &QState::entered, this, &cStatusModule::deactivationDone);
    connect(&m_DeactivationFinishedState, &QState::entered, this, &cStatusModule::deactivationFinished);

}


QByteArray cStatusModule::getConfiguration() const
{
    return m_pConfiguration->exportConfiguration();
}


void cStatusModule::setupModule()
{
    emit addEventSystem(m_pModuleValidator);
    cBaseMeasModule::setupModule();

    cStatusModuleConfigData *pConfData;
    pConfData = qobject_cast<cStatusModuleConfiguration*>(m_pConfiguration.get())->getConfigurationData();

    // we only have to read some status information from pcb- and dspserver
    m_pStatusModuleInit = new cStatusModuleInit(this, *pConfData);
    m_ModuleActivistList.append(m_pStatusModuleInit);
    connect(m_pStatusModuleInit, &cStatusModuleInit::activated, this, &cStatusModule::activationContinue);
    connect(m_pStatusModuleInit, &cStatusModuleInit::deactivated, this, &cStatusModule::deactivationContinue);

    for (int i = 0; i < m_ModuleActivistList.count(); i++)
        m_ModuleActivistList.at(i)->generateVeinInterface();
}


void cStatusModule::startMeas()
{
    // nothing to start here
}


void cStatusModule::stopMeas()
{
    // also nothing to stop
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

    m_pModuleValidator->setParameterMap(m_veinModuleParameterMap);
    exportMetaData();

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
