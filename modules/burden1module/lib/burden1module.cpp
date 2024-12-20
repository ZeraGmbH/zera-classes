#include "burden1module.h"
#include "burden1moduleconfiguration.h"
#include "burden1modulemeasprogram.h"
#include <errormessages.h>

namespace BURDEN1MODULE
{

cBurden1Module::cBurden1Module(ModuleFactoryParam moduleParam) :
    cBaseMeasModule(moduleParam, std::shared_ptr<BaseModuleConfiguration>(new cBurden1ModuleConfiguration()))
{
    m_sModuleName = QString("%1%2").arg(BaseModuleName).arg(moduleParam.m_moduleNum);
    m_sModuleDescription = QString("This module measures configured number burden and powerfactor from configured input dft values");
    m_sSCPIModuleName = QString("%1%2").arg(BaseSCPIModuleName).arg(moduleParam.m_moduleNum);

    m_ActivationStartState.addTransition(this, &cBurden1Module::activationContinue, &m_ActivationExecState);
    m_ActivationExecState.addTransition(this, &cBurden1Module::activationContinue, &m_ActivationDoneState);
    m_ActivationDoneState.addTransition(this, &cBurden1Module::activationNext, &m_ActivationExecState);
    m_ActivationDoneState.addTransition(this, &cBurden1Module::activationContinue, &m_ActivationFinishedState);
    m_ActivationMachine.addState(&m_ActivationStartState);
    m_ActivationMachine.addState(&m_ActivationExecState);
    m_ActivationMachine.addState(&m_ActivationDoneState);
    m_ActivationMachine.addState(&m_ActivationFinishedState);
    m_ActivationMachine.setInitialState(&m_ActivationStartState);
    connect(&m_ActivationStartState, &QState::entered, this, &cBurden1Module::activationStart);
    connect(&m_ActivationExecState, &QState::entered, this, &cBurden1Module::activationExec);
    connect(&m_ActivationDoneState, &QState::entered, this, &cBurden1Module::activationDone);
    connect(&m_ActivationFinishedState, &QState::entered, this, &cBurden1Module::activationFinished);

    m_DeactivationStartState.addTransition(this, &cBurden1Module::deactivationContinue, &m_DeactivationExecState);
    m_DeactivationExecState.addTransition(this, &cBurden1Module::deactivationContinue, &m_DeactivationDoneState);
    m_DeactivationDoneState.addTransition(this, &cBurden1Module::deactivationNext, &m_DeactivationExecState);
    m_DeactivationDoneState.addTransition(this, &cBurden1Module::deactivationContinue, &m_DeactivationFinishedState);
    m_DeactivationMachine.addState(&m_DeactivationStartState);
    m_DeactivationMachine.addState(&m_DeactivationExecState);
    m_DeactivationMachine.addState(&m_DeactivationDoneState);
    m_DeactivationMachine.addState(&m_DeactivationFinishedState);
    m_DeactivationMachine.setInitialState(&m_DeactivationStartState);
    connect(&m_DeactivationStartState, &QState::entered, this, &cBurden1Module::deactivationStart);
    connect(&m_DeactivationExecState, &QState::entered, this, &cBurden1Module::deactivationExec);
    connect(&m_DeactivationDoneState, &QState::entered, this, &cBurden1Module::deactivationDone);
    connect(&m_DeactivationFinishedState, &QState::entered, this, &cBurden1Module::deactivationFinished);

}


QByteArray cBurden1Module::getConfiguration() const
{
    return m_pConfiguration->exportConfiguration();
}



void cBurden1Module::setupModule()
{
    emit addEventSystem(m_pModuleValidator);

    cBaseMeasModule::setupModule();

    // we need some program that does the measuring on dsp
    m_pMeasProgram = new cBurden1ModuleMeasProgram(this, m_pConfiguration);
    m_ModuleActivistList.append(m_pMeasProgram);
    connect(m_pMeasProgram, &cBurden1ModuleMeasProgram::activated, this, &cBurden1Module::activationContinue);
    connect(m_pMeasProgram, &cBurden1ModuleMeasProgram::deactivated, this, &cBurden1Module::deactivationContinue);

    for (int i = 0; i < m_ModuleActivistList.count(); i++)
        m_ModuleActivistList.at(i)->generateVeinInterface();
}


void cBurden1Module::startMeas()
{
    m_pMeasProgram->start();
}


void cBurden1Module::stopMeas()
{
    m_pMeasProgram->stop();
}

void cBurden1Module::activationFinished()
{
    m_pModuleValidator->setParameterMap(m_veinModuleParameterMap);

    // now we still have to export the json interface information
    exportMetaData();

    emit activationReady();
}

void cBurden1Module::deactivationFinished()
{
    emit deactivationReady();
}

}
