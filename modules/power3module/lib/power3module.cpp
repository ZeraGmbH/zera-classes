#include "power3module.h"
#include "power3moduleconfiguration.h"
#include "power3modulemeasprogram.h"
#include <errormessages.h>

namespace POWER3MODULE
{

cPower3Module::cPower3Module(MeasurementModuleFactoryParam moduleParam) :
    cBaseMeasModule(moduleParam, std::shared_ptr<cBaseModuleConfiguration>(new cPower3ModuleConfiguration()))
{
    m_sModuleName = QString("%1%2").arg(BaseModuleName).arg(moduleParam.m_moduleNum);
    m_sModuleDescription = QString("This module measures configured number of harmonic power values from configured input values");
    m_sSCPIModuleName = QString("%1%2").arg(BaseSCPIModuleName).arg(moduleParam.m_moduleNum);

    m_ActivationStartState.addTransition(this, &cPower3Module::activationContinue, &m_ActivationExecState);
    m_ActivationExecState.addTransition(this, &cPower3Module::activationContinue, &m_ActivationDoneState);
    m_ActivationDoneState.addTransition(this, &cPower3Module::activationNext, &m_ActivationExecState);
    m_ActivationDoneState.addTransition(this, &cPower3Module::activationContinue, &m_ActivationFinishedState);
    m_ActivationMachine.addState(&m_ActivationStartState);
    m_ActivationMachine.addState(&m_ActivationExecState);
    m_ActivationMachine.addState(&m_ActivationDoneState);
    m_ActivationMachine.addState(&m_ActivationFinishedState);
    m_ActivationMachine.setInitialState(&m_ActivationStartState);
    connect(&m_ActivationStartState, &QState::entered, this, &cPower3Module::activationStart);
    connect(&m_ActivationExecState, &QState::entered, this, &cPower3Module::activationExec);
    connect(&m_ActivationDoneState, &QState::entered, this, &cPower3Module::activationDone);
    connect(&m_ActivationFinishedState, &QState::entered, this, &cPower3Module::activationFinished);

    m_DeactivationStartState.addTransition(this, &cPower3Module::deactivationContinue, &m_DeactivationExecState);
    m_DeactivationExecState.addTransition(this, &cPower3Module::deactivationContinue, &m_DeactivationDoneState);
    m_DeactivationDoneState.addTransition(this, &cPower3Module::deactivationNext, &m_DeactivationExecState);
    m_DeactivationDoneState.addTransition(this, &cPower3Module::deactivationContinue, &m_DeactivationFinishedState);
    m_DeactivationMachine.addState(&m_DeactivationStartState);
    m_DeactivationMachine.addState(&m_DeactivationExecState);
    m_DeactivationMachine.addState(&m_DeactivationDoneState);
    m_DeactivationMachine.addState(&m_DeactivationFinishedState);
    m_DeactivationMachine.setInitialState(&m_DeactivationStartState);
    connect(&m_DeactivationStartState, &QState::entered, this, &cPower3Module::deactivationStart);
    connect(&m_DeactivationExecState, &QState::entered, this, &cPower3Module::deactivationExec);
    connect(&m_DeactivationDoneState, &QState::entered, this, &cPower3Module::deactivationDone);
    connect(&m_DeactivationFinishedState, &QState::entered, this, &cPower3Module::deactivationFinished);

}


QByteArray cPower3Module::getConfiguration() const
{
    return m_pConfiguration->exportConfiguration();
}



void cPower3Module::setupModule()
{
    emit addEventSystem(m_pModuleValidator);

    cBaseMeasModule::setupModule();

    // we need some program that does the measuring on dsp
    m_pMeasProgram = new cPower3ModuleMeasProgram(this, m_pConfiguration);
    m_ModuleActivistList.append(m_pMeasProgram);
    connect(m_pMeasProgram, &cPower3ModuleMeasProgram::activated, this, &cPower3Module::activationContinue);
    connect(m_pMeasProgram, &cPower3ModuleMeasProgram::deactivated, this, &cPower3Module::deactivationContinue);
    connect(m_pMeasProgram, &cPower3ModuleMeasProgram::errMsg, m_pModuleErrorComponent, &VfModuleErrorComponent::setValue);

    for (int i = 0; i < m_ModuleActivistList.count(); i++)
        m_ModuleActivistList.at(i)->generateInterface();
}


void cPower3Module::startMeas()
{
    m_pMeasProgram->start();
}


void cPower3Module::stopMeas()
{
    m_pMeasProgram->stop();
}


void cPower3Module::activationStart()
{
    m_nActivationIt = 0; // we start with the first
    emit activationContinue();
}


void cPower3Module::activationExec()
{
    m_ModuleActivistList.at(m_nActivationIt)->activate();
}


void cPower3Module::activationDone()
{
    m_nActivationIt++;

    if (m_nActivationIt < m_ModuleActivistList.count())
        emit activationNext(); // and iterate over our list
    else
        emit activationContinue();
}


void cPower3Module::activationFinished()
{
    m_pModuleValidator->setParameterMap(m_veinModuleParameterMap);

    // now we still have to export the json interface information
    exportMetaData();

    emit activationReady();
}


void cPower3Module::deactivationStart()
{
    m_nActivationIt = 0; // we start with the first
    emit deactivationContinue();
}


void cPower3Module::deactivationExec()
{
    m_ModuleActivistList.at(m_nActivationIt)->deactivate();
}


void cPower3Module::deactivationDone()
{
    m_nActivationIt++;

    if (m_nActivationIt < m_ModuleActivistList.count())
        emit deactivationNext(); // and iterate over our list
    else
        emit deactivationContinue();
}


void cPower3Module::deactivationFinished()
{
    emit deactivationReady();
}

}
