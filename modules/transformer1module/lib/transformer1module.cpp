#include "transformer1module.h"
#include "transformer1moduleconfiguration.h"
#include "transformer1modulemeasprogram.h"
#include <vfmodulecomponent.h>
#include <vfmodulemetadata.h>

namespace TRANSFORMER1MODULE
{

cTransformer1Module::cTransformer1Module(ModuleFactoryParam moduleParam) :
    cBaseMeasModule(moduleParam, std::shared_ptr<BaseModuleConfiguration>(new cTransformer1ModuleConfiguration()))
{
    m_sModuleName = QString("%1%2").arg(BaseModuleName).arg(moduleParam.m_moduleNum);
    m_sModuleDescription = QString("This module measures configured number transformer errors from configured input dft values");
    m_sSCPIModuleName = QString("%1%2").arg(BaseSCPIModuleName).arg(moduleParam.m_moduleNum);

    m_ActivationStartState.addTransition(this, &cTransformer1Module::activationContinue, &m_ActivationExecState);
    m_ActivationExecState.addTransition(this, &cTransformer1Module::activationContinue, &m_ActivationDoneState);
    m_ActivationDoneState.addTransition(this, &cTransformer1Module::activationNext, &m_ActivationExecState);
    m_ActivationDoneState.addTransition(this, &cTransformer1Module::activationContinue, &m_ActivationFinishedState);
    m_ActivationMachine.addState(&m_ActivationStartState);
    m_ActivationMachine.addState(&m_ActivationExecState);
    m_ActivationMachine.addState(&m_ActivationDoneState);
    m_ActivationMachine.addState(&m_ActivationFinishedState);
    m_ActivationMachine.setInitialState(&m_ActivationStartState);
    connect(&m_ActivationStartState, &QState::entered, this, &cTransformer1Module::activationStart);
    connect(&m_ActivationExecState, &QState::entered, this, &cTransformer1Module::activationExec);
    connect(&m_ActivationDoneState, &QState::entered, this, &cTransformer1Module::activationDone);
    connect(&m_ActivationFinishedState, &QState::entered, this, &cTransformer1Module::activationFinished);

    m_DeactivationStartState.addTransition(this, &cTransformer1Module::deactivationContinue, &m_DeactivationExecState);
    m_DeactivationExecState.addTransition(this, &cTransformer1Module::deactivationContinue, &m_DeactivationDoneState);
    m_DeactivationDoneState.addTransition(this, &cTransformer1Module::deactivationNext, &m_DeactivationExecState);
    m_DeactivationDoneState.addTransition(this, &cTransformer1Module::deactivationContinue, &m_DeactivationFinishedState);
    m_DeactivationMachine.addState(&m_DeactivationStartState);
    m_DeactivationMachine.addState(&m_DeactivationExecState);
    m_DeactivationMachine.addState(&m_DeactivationDoneState);
    m_DeactivationMachine.addState(&m_DeactivationFinishedState);
    m_DeactivationMachine.setInitialState(&m_DeactivationStartState);
    connect(&m_DeactivationStartState, &QState::entered, this, &cTransformer1Module::deactivationStart);
    connect(&m_DeactivationExecState, &QState::entered, this, &cTransformer1Module::deactivationExec);
    connect(&m_DeactivationDoneState, &QState::entered, this, &cTransformer1Module::deactivationDone);
    connect(&m_DeactivationFinishedState, &QState::entered, this, &cTransformer1Module::deactivationFinished);
}

void cTransformer1Module::setupModule()
{
    emit addEventSystem(m_pModuleValidator);

    cBaseMeasModule::setupModule();

    // we need some program that does the measuring on dsp
    m_pMeasProgram = new cTransformer1ModuleMeasProgram(this, m_pConfiguration);
    m_ModuleActivistList.append(m_pMeasProgram);
    connect(m_pMeasProgram, &cTransformer1ModuleMeasProgram::activated, this, &cTransformer1Module::activationContinue);
    connect(m_pMeasProgram, &cTransformer1ModuleMeasProgram::deactivated, this, &cTransformer1Module::deactivationContinue);

    for (int i = 0; i < m_ModuleActivistList.count(); i++)
        m_ModuleActivistList.at(i)->generateVeinInterface();
}

void cTransformer1Module::startMeas()
{
    m_pMeasProgram->start();
}

void cTransformer1Module::stopMeas()
{
    m_pMeasProgram->stop();
}

}
