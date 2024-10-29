#include "modemodule.h"
#include "modemoduleconfiguration.h"
#include "modemoduleconfigdata.h"
#include "modemoduleinit.h"
#include <vfmodulecomponent.h>
#include <vfmoduleerrorcomponent.h>
#include <vfmodulemetadata.h>

namespace MODEMODULE
{

cModeModule::cModeModule(ModuleFactoryParam moduleParam) :
    cBaseMeasModule(moduleParam, std::shared_ptr<cBaseModuleConfiguration>(new cModeModuleConfiguration()))
{
    m_sModuleName = QString("%1%2").arg(BaseModuleName).arg(moduleParam.m_moduleNum);
    m_sModuleDescription = QString("This module is responsible for setting measuring mode and resetting dsp adjustment data");
    m_sSCPIModuleName = QString("%1%2").arg(BaseSCPIModuleName).arg(moduleParam.m_moduleNum);

    m_ActivationStartState.addTransition(this, &cModeModule::activationContinue, &m_ActivationExecState);
    m_ActivationExecState.addTransition(this, &cModeModule::activationContinue, &m_ActivationDoneState);
    m_ActivationDoneState.addTransition(this, &cModeModule::activationNext, &m_ActivationExecState);
    m_ActivationDoneState.addTransition(this, &cModeModule::activationContinue, &m_ActivationFinishedState);
    m_ActivationMachine.addState(&m_ActivationStartState);
    m_ActivationMachine.addState(&m_ActivationExecState);
    m_ActivationMachine.addState(&m_ActivationDoneState);
    m_ActivationMachine.addState(&m_ActivationFinishedState);
    m_ActivationMachine.setInitialState(&m_ActivationStartState);
    connect(&m_ActivationStartState, &QState::entered, this, &cModeModule::activationStart);
    connect(&m_ActivationExecState, &QState::entered, this, &cModeModule::activationExec);
    connect(&m_ActivationDoneState, &QState::entered, this, &cModeModule::activationDone);
    connect(&m_ActivationFinishedState, &QState::entered, this, &cModeModule::activationFinished);

    m_DeactivationStartState.addTransition(this, &cModeModule::deactivationContinue, &m_DeactivationExecState);
    m_DeactivationExecState.addTransition(this, &cModeModule::deactivationContinue, &m_DeactivationDoneState);
    m_DeactivationDoneState.addTransition(this, &cModeModule::deactivationNext, &m_DeactivationExecState);
    m_DeactivationDoneState.addTransition(this, &cModeModule::deactivationContinue, &m_DeactivationFinishedState);
    m_DeactivationMachine.addState(&m_DeactivationStartState);
    m_DeactivationMachine.addState(&m_DeactivationExecState);
    m_DeactivationMachine.addState(&m_DeactivationDoneState);
    m_DeactivationMachine.addState(&m_DeactivationFinishedState);
    m_DeactivationMachine.setInitialState(&m_DeactivationStartState);
    connect(&m_DeactivationStartState, &QState::entered, this, &cModeModule::deactivationStart);
    connect(&m_DeactivationExecState, &QState::entered, this, &cModeModule::deactivationExec);
    connect(&m_DeactivationDoneState, &QState::entered, this, &cModeModule::deactivationDone);
    connect(&m_DeactivationFinishedState, &QState::entered, this, &cModeModule::deactivationFinished);

}


QByteArray cModeModule::getConfiguration() const
{
    return m_pConfiguration->exportConfiguration();
}


void cModeModule::setupModule()
{
    emit addEventSystem(m_pModuleValidator);
    cBaseMeasModule::setupModule();

    cModeModuleConfigData *pConfData;
    pConfData = qobject_cast<cModeModuleConfiguration*>(m_pConfiguration.get())->getConfigurationData();

    // we only have to initialize the pcb's measuring mode
    m_pModeModuleInit = new cModeModuleInit(this, *pConfData);
    m_ModuleActivistList.append(m_pModeModuleInit);
    connect(m_pModeModuleInit, &cModeModuleInit::activated, this, &cModeModule::activationContinue);
    connect(m_pModeModuleInit, &cModeModuleInit::deactivated, this, &cModeModule::deactivationContinue);
    connect(m_pModeModuleInit, &cModeModuleInit::errMsg, m_pModuleErrorComponent, &VfModuleErrorComponent::setValue);

    for (int i = 0; i < m_ModuleActivistList.count(); i++)
        m_ModuleActivistList.at(i)->generateInterface();
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
    // now we still have to export the json interface information, then we are ready
    exportMetaData();
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
