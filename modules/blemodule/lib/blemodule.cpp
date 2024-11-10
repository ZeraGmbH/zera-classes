#include "blemodule.h"
#include "blemoduleconfiguration.h"
#include <errormessages.h>

namespace BLEMODULE
{

cBleModule::cBleModule(ModuleFactoryParam moduleParam) :
    cBaseMeasModule(moduleParam, std::shared_ptr<BaseModuleConfiguration>(new cBleModuleConfiguration()))
{
    m_sModuleName = QString("%1%2").arg(BaseModuleName).arg(moduleParam.m_moduleNum);
    m_sModuleDescription = QString("This module measures configured number of harmonic power values from configured input values");
    m_sSCPIModuleName = QString("%1%2").arg(BaseSCPIModuleName).arg(moduleParam.m_moduleNum);

    m_ActivationStartState.addTransition(this, &cBleModule::activationContinue, &m_ActivationExecState);
    m_ActivationExecState.addTransition(this, &cBleModule::activationContinue, &m_ActivationDoneState);
    m_ActivationDoneState.addTransition(this, &cBleModule::activationNext, &m_ActivationExecState);
    m_ActivationDoneState.addTransition(this, &cBleModule::activationContinue, &m_ActivationFinishedState);
    m_ActivationMachine.addState(&m_ActivationStartState);
    m_ActivationMachine.addState(&m_ActivationExecState);
    m_ActivationMachine.addState(&m_ActivationDoneState);
    m_ActivationMachine.addState(&m_ActivationFinishedState);
    m_ActivationMachine.setInitialState(&m_ActivationStartState);
    connect(&m_ActivationStartState, &QState::entered, this, &cBleModule::activationStart);
    connect(&m_ActivationExecState, &QState::entered, this, &cBleModule::activationExec);
    connect(&m_ActivationDoneState, &QState::entered, this, &cBleModule::activationDone);
    connect(&m_ActivationFinishedState, &QState::entered, this, &cBleModule::activationFinished);

    m_DeactivationStartState.addTransition(this, &cBleModule::deactivationContinue, &m_DeactivationExecState);
    m_DeactivationExecState.addTransition(this, &cBleModule::deactivationContinue, &m_DeactivationDoneState);
    m_DeactivationDoneState.addTransition(this, &cBleModule::deactivationNext, &m_DeactivationExecState);
    m_DeactivationDoneState.addTransition(this, &cBleModule::deactivationContinue, &m_DeactivationFinishedState);
    m_DeactivationMachine.addState(&m_DeactivationStartState);
    m_DeactivationMachine.addState(&m_DeactivationExecState);
    m_DeactivationMachine.addState(&m_DeactivationDoneState);
    m_DeactivationMachine.addState(&m_DeactivationFinishedState);
    m_DeactivationMachine.setInitialState(&m_DeactivationStartState);
    connect(&m_DeactivationStartState, &QState::entered, this, &cBleModule::deactivationStart);
    connect(&m_DeactivationExecState, &QState::entered, this, &cBleModule::deactivationExec);
    connect(&m_DeactivationDoneState, &QState::entered, this, &cBleModule::deactivationDone);
    connect(&m_DeactivationFinishedState, &QState::entered, this, &cBleModule::deactivationFinished);
}

QByteArray cBleModule::getConfiguration() const
{
    return m_pConfiguration->exportConfiguration();
}

void cBleModule::setupModule()
{
    emit addEventSystem(m_pModuleValidator);
    cBaseMeasModule::setupModule();

    m_pMeasProgram = new cBleModuleMeasProgram(this, m_pConfiguration);
    m_ModuleActivistList.append(m_pMeasProgram);
    connect(m_pMeasProgram, &cBleModuleMeasProgram::activated, this, &cBleModule::activationContinue);
    connect(m_pMeasProgram, &cBleModuleMeasProgram::deactivated, this, &cBleModule::deactivationContinue);

    for (int i = 0; i < m_ModuleActivistList.count(); i++)
        m_ModuleActivistList.at(i)->generateInterface();
}

void cBleModule::startMeas()
{
    m_pMeasProgram->start();
}

void cBleModule::stopMeas()
{
    m_pMeasProgram->stop();
}

void cBleModule::activationStart()
{
    m_nActivationIt = 0; // we start with the first
    emit activationContinue();
}

void cBleModule::activationExec()
{
    m_ModuleActivistList.at(m_nActivationIt)->activate();
}

void cBleModule::activationDone()
{
    m_nActivationIt++;

    if (m_nActivationIt < m_ModuleActivistList.count())
        emit activationNext(); // and iterate over our list
    else
        emit activationContinue();
}

void cBleModule::activationFinished()
{
    m_pModuleValidator->setParameterMap(m_veinModuleParameterMap);

    // now we still have to export the json interface information
    exportMetaData();

    emit activationReady();
}

void cBleModule::deactivationStart()
{
    m_nActivationIt = 0; // we start with the first
    emit deactivationContinue();
}

void cBleModule::deactivationExec()
{
    m_ModuleActivistList.at(m_nActivationIt)->deactivate();
}

void cBleModule::deactivationDone()
{
    m_nActivationIt++;

    if (m_nActivationIt < m_ModuleActivistList.count())
        emit deactivationNext(); // and iterate over our list
    else
        emit deactivationContinue();
}

void cBleModule::deactivationFinished()
{
    emit deactivationReady();
}

}
