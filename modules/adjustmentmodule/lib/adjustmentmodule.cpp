#include "adjustmentmodule.h"
#include "adjustmentmoduleconfiguration.h"
#include "adjustmentmodulemeasprogram.h"
#include <errormessages.h>

cAdjustmentModule::cAdjustmentModule(MeasurementModuleFactoryParam moduleParam) :
    cBaseMeasModule(moduleParam, std::shared_ptr<cBaseModuleConfiguration>(new cAdjustmentModuleConfiguration()))
{
    m_sModuleName = QString("%1%2").arg(BaseModuleName).arg(moduleParam.m_moduleNum);
    m_sModuleDescription = QString("This module supports commands for adjustment for a configured number of measuring channels");
    m_sSCPIModuleName = QString("%1%2").arg(BaseSCPIModuleName).arg(moduleParam.m_moduleNum);

    m_ActivationStartState.addTransition(this, &cAdjustmentModule::activationContinue, &m_ActivationExecState);
    m_ActivationExecState.addTransition(this, &cAdjustmentModule::activationContinue, &m_ActivationDoneState);
    m_ActivationDoneState.addTransition(this, &cAdjustmentModule::activationNext, &m_ActivationExecState);
    m_ActivationDoneState.addTransition(this, &cAdjustmentModule::activationContinue, &m_ActivationFinishedState);
    m_ActivationMachine.addState(&m_ActivationStartState);
    m_ActivationMachine.addState(&m_ActivationExecState);
    m_ActivationMachine.addState(&m_ActivationDoneState);
    m_ActivationMachine.addState(&m_ActivationFinishedState);
    m_ActivationMachine.setInitialState(&m_ActivationStartState);
    connect(&m_ActivationStartState, &QState::entered, this, &cAdjustmentModule::activationStart);
    connect(&m_ActivationExecState, &QState::entered, this, &cAdjustmentModule::activationExec);
    connect(&m_ActivationDoneState, &QState::entered, this, &cAdjustmentModule::activationDone);
    connect(&m_ActivationFinishedState, &QState::entered, this, &cAdjustmentModule::activationFinished);

    m_DeactivationStartState.addTransition(this, &cAdjustmentModule::deactivationContinue, &m_DeactivationExecState);
    m_DeactivationExecState.addTransition(this, &cAdjustmentModule::deactivationContinue, &m_DeactivationDoneState);
    m_DeactivationDoneState.addTransition(this, &cAdjustmentModule::deactivationNext, &m_DeactivationExecState);
    m_DeactivationDoneState.addTransition(this, &cAdjustmentModule::deactivationContinue, &m_DeactivationFinishedState);
    m_DeactivationMachine.addState(&m_DeactivationStartState);
    m_DeactivationMachine.addState(&m_DeactivationExecState);
    m_DeactivationMachine.addState(&m_DeactivationDoneState);
    m_DeactivationMachine.addState(&m_DeactivationFinishedState);
    m_DeactivationMachine.setInitialState(&m_DeactivationStartState);
    connect(&m_DeactivationStartState, &QState::entered, this, &cAdjustmentModule::deactivationStart);
    connect(&m_DeactivationExecState, &QState::entered, this, &cAdjustmentModule::deactivationExec);
    connect(&m_DeactivationDoneState, &QState::entered, this, &cAdjustmentModule::deactivationDone);
    connect(&m_DeactivationFinishedState, &QState::entered, this, &cAdjustmentModule::deactivationFinished);

}


QByteArray cAdjustmentModule::getConfiguration() const
{
    return m_pConfiguration->exportConfiguration();
}



void cAdjustmentModule::setupModule()
{
    emit addEventSystem(m_pModuleValidator);

    cBaseMeasModule::setupModule();

    // we need some program that does the job
    m_pMeasProgram = new cAdjustmentModuleMeasProgram(this, m_pConfiguration);
    m_ModuleActivistList.append(m_pMeasProgram);
    connect(m_pMeasProgram, &cAdjustmentModuleMeasProgram::activated, this, &cAdjustmentModule::activationContinue);
    connect(m_pMeasProgram, &cAdjustmentModuleMeasProgram::deactivated, this, &cAdjustmentModule::deactivationContinue);
    connect(m_pMeasProgram, &cAdjustmentModuleMeasProgram::errMsg, m_pModuleErrorComponent, &VfModuleErrorComponent::setValue);

    emit addEventSystem(m_pMeasProgram->getEventSystem());

    for (int i = 0; i < m_ModuleActivistList.count(); i++)
        m_ModuleActivistList.at(i)->generateInterface();
}


void cAdjustmentModule::startMeas()
{
    m_pMeasProgram->start();
}


void cAdjustmentModule::stopMeas()
{
    m_pMeasProgram->stop();
}


void cAdjustmentModule::activationStart()
{
    m_nActivationIt = 0; // we start with the first
    emit activationContinue();
}


void cAdjustmentModule::activationExec()
{
    m_ModuleActivistList.at(m_nActivationIt)->activate();
}


void cAdjustmentModule::activationDone()
{
    m_nActivationIt++;

    if (m_nActivationIt < m_ModuleActivistList.count())
        emit activationNext(); // and iterate over our list
    else
        emit activationContinue();
}


void cAdjustmentModule::activationFinished()
{
    m_pModuleValidator->setParameterMap(m_veinModuleParameterMap);

    // now we still have to export the json interface information
    exportMetaData();

    emit activationReady();
}


void cAdjustmentModule::deactivationStart()
{
    m_nActivationIt = 0; // we start with the first
    emit deactivationContinue();
}


void cAdjustmentModule::deactivationExec()
{
    m_ModuleActivistList.at(m_nActivationIt)->deactivate();
}


void cAdjustmentModule::deactivationDone()
{
    m_nActivationIt++;

    if (m_nActivationIt < m_ModuleActivistList.count())
        emit deactivationNext(); // and iterate over our list
    else
        emit deactivationContinue();
}


void cAdjustmentModule::deactivationFinished()
{
    emit deactivationReady();
}
