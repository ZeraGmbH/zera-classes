#include "lambdamodule.h"
#include "lambdamoduleconfiguration.h"
#include <errormessages.h>

namespace LAMBDAMODULE
{

cLambdaModule::cLambdaModule(ModuleFactoryParam moduleParam) :
    cBaseMeasModule(moduleParam,std::shared_ptr<cBaseModuleConfiguration>(new cLambdaModuleConfiguration()))
{
    m_sModuleName = QString("%1%2").arg(BaseModuleName).arg(moduleParam.m_moduleNum);
    m_sModuleDescription = QString("This module measures configured number of harmonic power values from configured input values");
    m_sSCPIModuleName = QString("%1%2").arg(BaseSCPIModuleName).arg(moduleParam.m_moduleNum);

    m_ActivationStartState.addTransition(this, &cLambdaModule::activationContinue, &m_ActivationExecState);
    m_ActivationExecState.addTransition(this, &cLambdaModule::activationContinue, &m_ActivationDoneState);
    m_ActivationDoneState.addTransition(this, &cLambdaModule::activationNext, &m_ActivationExecState);
    m_ActivationDoneState.addTransition(this, &cLambdaModule::activationContinue, &m_ActivationFinishedState);
    m_ActivationMachine.addState(&m_ActivationStartState);
    m_ActivationMachine.addState(&m_ActivationExecState);
    m_ActivationMachine.addState(&m_ActivationDoneState);
    m_ActivationMachine.addState(&m_ActivationFinishedState);
    m_ActivationMachine.setInitialState(&m_ActivationStartState);
    connect(&m_ActivationStartState, &QState::entered, this, &cLambdaModule::activationStart);
    connect(&m_ActivationExecState, &QState::entered, this, &cLambdaModule::activationExec);
    connect(&m_ActivationDoneState, &QState::entered, this, &cLambdaModule::activationDone);
    connect(&m_ActivationFinishedState, &QState::entered, this, &cLambdaModule::activationFinished);

    m_DeactivationStartState.addTransition(this, &cLambdaModule::deactivationContinue, &m_DeactivationExecState);
    m_DeactivationExecState.addTransition(this, &cLambdaModule::deactivationContinue, &m_DeactivationDoneState);
    m_DeactivationDoneState.addTransition(this, &cLambdaModule::deactivationNext, &m_DeactivationExecState);
    m_DeactivationDoneState.addTransition(this, &cLambdaModule::deactivationContinue, &m_DeactivationFinishedState);
    m_DeactivationMachine.addState(&m_DeactivationStartState);
    m_DeactivationMachine.addState(&m_DeactivationExecState);
    m_DeactivationMachine.addState(&m_DeactivationDoneState);
    m_DeactivationMachine.addState(&m_DeactivationFinishedState);
    m_DeactivationMachine.setInitialState(&m_DeactivationStartState);
    connect(&m_DeactivationStartState, &QState::entered, this, &cLambdaModule::deactivationStart);
    connect(&m_DeactivationExecState, &QState::entered, this, &cLambdaModule::deactivationExec);
    connect(&m_DeactivationDoneState, &QState::entered, this, &cLambdaModule::deactivationDone);
    connect(&m_DeactivationFinishedState, &QState::entered, this, &cLambdaModule::deactivationFinished);

}


QByteArray cLambdaModule::getConfiguration() const
{
    return m_pConfiguration->exportConfiguration();
}



void cLambdaModule::setupModule()
{
    emit addEventSystem(m_pModuleValidator);

    cBaseMeasModule::setupModule();

    // we need some program that does the measuring on dsp
    m_pMeasProgram = new cLambdaModuleMeasProgram(this, m_pConfiguration);
    m_ModuleActivistList.append(m_pMeasProgram);
    connect(m_pMeasProgram, &cLambdaModuleMeasProgram::activated, this, &cLambdaModule::activationContinue);
    connect(m_pMeasProgram, &cLambdaModuleMeasProgram::deactivated, this, &cLambdaModule::deactivationContinue);
    connect(m_pMeasProgram, &cLambdaModuleMeasProgram::errMsg, m_pModuleErrorComponent, &VfModuleErrorComponent::setValue);

    for (int i = 0; i < m_ModuleActivistList.count(); i++)
        m_ModuleActivistList.at(i)->generateInterface();
}


void cLambdaModule::startMeas()
{
    m_pMeasProgram->start();
}


void cLambdaModule::stopMeas()
{
    m_pMeasProgram->stop();
}


void cLambdaModule::activationStart()
{
    m_nActivationIt = 0; // we start with the first
    emit activationContinue();
}


void cLambdaModule::activationExec()
{
    m_ModuleActivistList.at(m_nActivationIt)->activate();
}


void cLambdaModule::activationDone()
{
    m_nActivationIt++;

    if (m_nActivationIt < m_ModuleActivistList.count())
        emit activationNext(); // and iterate over our list
    else
        emit activationContinue();
}


void cLambdaModule::activationFinished()
{
    m_pModuleValidator->setParameterMap(m_veinModuleParameterMap);

    // now we still have to export the json interface information
    exportMetaData();

    emit activationReady();
}


void cLambdaModule::deactivationStart()
{
    m_nActivationIt = 0; // we start with the first
    emit deactivationContinue();
}


void cLambdaModule::deactivationExec()
{
    m_ModuleActivistList.at(m_nActivationIt)->deactivate();
}


void cLambdaModule::deactivationDone()
{
    m_nActivationIt++;

    if (m_nActivationIt < m_ModuleActivistList.count())
        emit deactivationNext(); // and iterate over our list
    else
        emit deactivationContinue();
}


void cLambdaModule::deactivationFinished()
{
    emit deactivationReady();
}

}
