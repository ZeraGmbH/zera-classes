#include "sem1module.h"
#include "sem1moduleconfiguration.h"
#include "sem1modulemeasprogram.h"
#include <vfmodulecomponent.h>
#include <vfmoduleerrorcomponent.h>
#include <vfmodulemetadata.h>

namespace SEM1MODULE
{

cSem1Module::cSem1Module(ModuleFactoryParam moduleParam) :
    cBaseMeasModule(moduleParam, std::shared_ptr<BaseModuleConfiguration>(new cSem1ModuleConfiguration()))
{
    m_sModuleName = QString("%1%2").arg(BaseModuleName).arg(moduleParam.m_moduleNum);
    m_sModuleDescription = QString("This module povides a configurable error calculator");
    m_sSCPIModuleName = QString(BaseSCPIModuleName);

    m_ActivationStartState.addTransition(this, &cSem1Module::activationContinue, &m_ActivationExecState);
    m_ActivationExecState.addTransition(this, &cSem1Module::activationContinue, &m_ActivationDoneState);
    m_ActivationDoneState.addTransition(this, &cSem1Module::activationNext, &m_ActivationExecState);
    m_ActivationDoneState.addTransition(this, &cSem1Module::activationContinue, &m_ActivationFinishedState);
    m_ActivationMachine.addState(&m_ActivationStartState);
    m_ActivationMachine.addState(&m_ActivationExecState);
    m_ActivationMachine.addState(&m_ActivationDoneState);
    m_ActivationMachine.addState(&m_ActivationFinishedState);
    m_ActivationMachine.setInitialState(&m_ActivationStartState);
    connect(&m_ActivationStartState, &QState::entered, this, &cSem1Module::activationStart);
    connect(&m_ActivationExecState, &QState::entered, this, &cSem1Module::activationExec);
    connect(&m_ActivationDoneState, &QState::entered, this, &cSem1Module::activationDone);
    connect(&m_ActivationFinishedState, &QState::entered, this, &cSem1Module::activationFinished);

    m_DeactivationStartState.addTransition(this, &cSem1Module::deactivationContinue, &m_DeactivationExecState);
    m_DeactivationExecState.addTransition(this, &cSem1Module::deactivationContinue, &m_DeactivationDoneState);
    m_DeactivationDoneState.addTransition(this, &cSem1Module::deactivationNext, &m_DeactivationExecState);
    m_DeactivationDoneState.addTransition(this, &cSem1Module::deactivationContinue, &m_DeactivationFinishedState);
    m_DeactivationMachine.addState(&m_DeactivationStartState);
    m_DeactivationMachine.addState(&m_DeactivationExecState);
    m_DeactivationMachine.addState(&m_DeactivationDoneState);
    m_DeactivationMachine.addState(&m_DeactivationFinishedState);
    m_DeactivationMachine.setInitialState(&m_DeactivationStartState);
    connect(&m_DeactivationStartState, &QState::entered, this, &cSem1Module::deactivationStart);
    connect(&m_DeactivationExecState, &QState::entered, this, &cSem1Module::deactivationExec);
    connect(&m_DeactivationDoneState, &QState::entered, this, &cSem1Module::deactivationDone);
    connect(&m_DeactivationFinishedState, &QState::entered, this, &cSem1Module::deactivationFinished);

}


QByteArray cSem1Module::getConfiguration() const
{
    return m_pConfiguration->exportConfiguration();
}


void cSem1Module::setupModule()
{
    emit addEventSystem(m_pModuleValidator);
    cBaseMeasModule::setupModule();

    // we only have this activist
    m_pMeasProgram = new cSem1ModuleMeasProgram(this, m_pConfiguration);
    m_ModuleActivistList.append(m_pMeasProgram);
    connect(m_pMeasProgram, &cSem1ModuleMeasProgram::activated, this, &cSem1Module::activationContinue);
    connect(m_pMeasProgram, &cSem1ModuleMeasProgram::deactivated, this, &cSem1Module::deactivationContinue);

    for (int i = 0; i < m_ModuleActivistList.count(); i++)
        m_ModuleActivistList.at(i)->generateInterface();
}


void cSem1Module::startMeas()
{
    m_pMeasProgram->start();
}


void cSem1Module::stopMeas()
{
    m_pMeasProgram->stop();
}


void cSem1Module::activationStart()
{
    m_nActivationIt = 0; // we start with the first
    emit activationContinue();
}


void cSem1Module::activationExec()
{
    m_ModuleActivistList.at(m_nActivationIt)->activate();
}


void cSem1Module::activationDone()
{
    m_nActivationIt++;

    if (m_nActivationIt < m_ModuleActivistList.count())
        emit activationNext(); // and iterate over our list
    else
        emit activationContinue();
}


void cSem1Module::activationFinished()
{
    m_pModuleValidator->setParameterMap(m_veinModuleParameterMap);

    // now we still have to export the json interface information
    exportMetaData();

    emit activationReady();
}


void cSem1Module::deactivationStart()
{
    m_nActivationIt = 0; // we start with the first
    emit deactivationContinue();
}


void cSem1Module::deactivationExec()
{
    m_ModuleActivistList.at(m_nActivationIt)->deactivate();
}


void cSem1Module::deactivationDone()
{
    m_nActivationIt++;

    if (m_nActivationIt < m_ModuleActivistList.count())
        emit deactivationNext(); // and iterate over our list
    else
        emit deactivationContinue();
}


void cSem1Module::deactivationFinished()
{
    emit deactivationReady();
}

}
