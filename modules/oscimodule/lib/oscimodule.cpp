#include "oscimodule.h"
#include "oscimoduleconfiguration.h"
#include "oscimodulemeasprogram.h"
#include <errormessages.h>

namespace OSCIMODULE
{

cOsciModule::cOsciModule(ModuleFactoryParam moduleParam) :
    cBaseMeasModule(moduleParam, std::shared_ptr<BaseModuleConfiguration>(new cOsciModuleConfiguration()))
{
    m_sModuleName = QString("%1%2").arg(BaseModuleName).arg(moduleParam.m_moduleNum);
    m_sModuleDescription = QString("This module measures oscillograms for configured channels");
    m_sSCPIModuleName = QString("%1%2").arg(BaseSCPIModuleName).arg(moduleParam.m_moduleNum);

    m_ActivationStartState.addTransition(this, &cOsciModule::activationContinue, &m_ActivationExecState);
    m_ActivationExecState.addTransition(this, &cOsciModule::activationContinue, &m_ActivationDoneState);
    m_ActivationDoneState.addTransition(this, &cOsciModule::activationNext, &m_ActivationExecState);
    m_ActivationDoneState.addTransition(this, &cOsciModule::activationContinue, &m_ActivationFinishedState);
    m_ActivationMachine.addState(&m_ActivationStartState);
    m_ActivationMachine.addState(&m_ActivationExecState);
    m_ActivationMachine.addState(&m_ActivationDoneState);
    m_ActivationMachine.addState(&m_ActivationFinishedState);
    m_ActivationMachine.setInitialState(&m_ActivationStartState);
    connect(&m_ActivationStartState, &QState::entered, this, &cOsciModule::activationStart);
    connect(&m_ActivationExecState, &QState::entered, this, &cOsciModule::activationExec);
    connect(&m_ActivationDoneState, &QState::entered, this, &cOsciModule::activationDone);
    connect(&m_ActivationFinishedState, &QState::entered, this, &cOsciModule::activationFinished);

    m_DeactivationStartState.addTransition(this, &cOsciModule::deactivationContinue, &m_DeactivationExecState);
    m_DeactivationExecState.addTransition(this, &cOsciModule::deactivationContinue, &m_DeactivationDoneState);
    m_DeactivationDoneState.addTransition(this, &cOsciModule::deactivationNext, &m_DeactivationExecState);
    m_DeactivationDoneState.addTransition(this, &cOsciModule::deactivationContinue, &m_DeactivationFinishedState);
    m_DeactivationMachine.addState(&m_DeactivationStartState);
    m_DeactivationMachine.addState(&m_DeactivationExecState);
    m_DeactivationMachine.addState(&m_DeactivationDoneState);
    m_DeactivationMachine.addState(&m_DeactivationFinishedState);
    m_DeactivationMachine.setInitialState(&m_DeactivationStartState);
    connect(&m_DeactivationStartState, &QState::entered, this, &cOsciModule::deactivationStart);
    connect(&m_DeactivationExecState, &QState::entered, this, &cOsciModule::deactivationExec);
    connect(&m_DeactivationDoneState, &QState::entered, this, &cOsciModule::deactivationDone);
    connect(&m_DeactivationFinishedState, &QState::entered, this, &cOsciModule::deactivationFinished);

}


QByteArray cOsciModule::getConfiguration() const
{
    return m_pConfiguration->exportConfiguration();
}



void cOsciModule::setupModule()
{
    emit addEventSystem(m_pModuleValidator);
    cBaseMeasModule::setupModule();

    // we need some program that does the measuring on dsp
    m_pMeasProgram = new cOsciModuleMeasProgram(this, m_pConfiguration);
    m_ModuleActivistList.append(m_pMeasProgram);
    connect(m_pMeasProgram, &cOsciModuleMeasProgram::activated, this, &cOsciModule::activationContinue);
    connect(m_pMeasProgram, &cOsciModuleMeasProgram::deactivated, this, &cOsciModule::deactivationContinue);
    connect(m_pMeasProgram, &cOsciModuleMeasProgram::errMsg, m_pModuleErrorComponent, &VfModuleErrorComponent::setValue);

    for (int i = 0; i < m_ModuleActivistList.count(); i++)
        m_ModuleActivistList.at(i)->generateInterface();
}


void cOsciModule::startMeas()
{
    m_pMeasProgram->start();
}


void cOsciModule::stopMeas()
{
    m_pMeasProgram->stop();
}


void cOsciModule::activationStart()
{
    m_nActivationIt = 0; // we start with the first
    emit activationContinue();
}


void cOsciModule::activationExec()
{
    m_ModuleActivistList.at(m_nActivationIt)->activate();
}


void cOsciModule::activationDone()
{
    m_nActivationIt++;

    if (m_nActivationIt < m_ModuleActivistList.count())
        emit activationNext(); // and iterate over our list
    else
        emit activationContinue();
}


void cOsciModule::activationFinished()
{
    m_pModuleValidator->setParameterMap(m_veinModuleParameterMap);
    // now we still have to export the json interface information

    exportMetaData();

    emit activationReady();
}


void cOsciModule::deactivationStart()
{
    m_nActivationIt = 0; // we start with the first
    emit deactivationContinue();
}


void cOsciModule::deactivationExec()
{
    m_ModuleActivistList.at(m_nActivationIt)->deactivate();
}


void cOsciModule::deactivationDone()
{
    m_nActivationIt++;

    if (m_nActivationIt < m_ModuleActivistList.count())
        emit deactivationNext(); // and iterate over our list
    else
        emit deactivationContinue();
}


void cOsciModule::deactivationFinished()
{
    emit deactivationReady();
}

}
