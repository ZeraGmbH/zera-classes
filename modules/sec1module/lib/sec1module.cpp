#include "sec1module.h"
#include "sec1moduleconfiguration.h"
#include "sec1modulemeasprogram.h"
#include <QJsonDocument>
#include <QJsonObject>

namespace SEC1MODULE
{

cSec1Module::cSec1Module(ModuleFactoryParam moduleParam) :
    cBaseMeasModule(moduleParam, std::shared_ptr<BaseModuleConfiguration>(new cSec1ModuleConfiguration()))
{
    m_sModuleName = QString("%1%2").arg(BaseModuleName).arg(moduleParam.m_moduleNum);
    m_sModuleDescription = QString("This module povides a configurable error calculator");
    m_sSCPIModuleName = QString(BaseSCPIModuleName);

    m_ActivationStartState.addTransition(this, &cSec1Module::activationContinue, &m_ActivationExecState);
    m_ActivationExecState.addTransition(this, &cSec1Module::activationContinue, &m_ActivationDoneState);
    m_ActivationDoneState.addTransition(this, &cSec1Module::activationNext, &m_ActivationExecState);
    m_ActivationDoneState.addTransition(this, &cSec1Module::activationContinue, &m_ActivationFinishedState);
    m_ActivationMachine.addState(&m_ActivationStartState);
    m_ActivationMachine.addState(&m_ActivationExecState);
    m_ActivationMachine.addState(&m_ActivationDoneState);
    m_ActivationMachine.addState(&m_ActivationFinishedState);
    m_ActivationMachine.setInitialState(&m_ActivationStartState);
    connect(&m_ActivationStartState, &QState::entered, this, &cSec1Module::activationStart);
    connect(&m_ActivationExecState, &QState::entered, this, &cSec1Module::activationExec);
    connect(&m_ActivationDoneState, &QState::entered, this, &cSec1Module::activationDone);
    connect(&m_ActivationFinishedState, &QState::entered, this, &cSec1Module::activationFinished);

    m_DeactivationStartState.addTransition(this, &cSec1Module::deactivationContinue, &m_DeactivationExecState);
    m_DeactivationExecState.addTransition(this, &cSec1Module::deactivationContinue, &m_DeactivationDoneState);
    m_DeactivationDoneState.addTransition(this, &cSec1Module::deactivationNext, &m_DeactivationExecState);
    m_DeactivationDoneState.addTransition(this, &cSec1Module::deactivationContinue, &m_DeactivationFinishedState);
    m_DeactivationMachine.addState(&m_DeactivationStartState);
    m_DeactivationMachine.addState(&m_DeactivationExecState);
    m_DeactivationMachine.addState(&m_DeactivationDoneState);
    m_DeactivationMachine.addState(&m_DeactivationFinishedState);
    m_DeactivationMachine.setInitialState(&m_DeactivationStartState);
    connect(&m_DeactivationStartState, &QState::entered, this, &cSec1Module::deactivationStart);
    connect(&m_DeactivationExecState, &QState::entered, this, &cSec1Module::deactivationExec);
    connect(&m_DeactivationDoneState, &QState::entered, this, &cSec1Module::deactivationDone);
    connect(&m_DeactivationFinishedState, &QState::entered, this, &cSec1Module::deactivationFinished);

}


QByteArray cSec1Module::getConfiguration() const
{
    return m_pConfiguration->exportConfiguration();
}


void cSec1Module::setupModule()
{
    emit addEventSystem(m_pModuleValidator);
    cBaseMeasModule::setupModule();

    // we only have this activist
    m_pMeasProgram = new cSec1ModuleMeasProgram(this, m_pConfiguration);
    m_ModuleActivistList.append(m_pMeasProgram);
    connect(m_pMeasProgram, &cSec1ModuleMeasProgram::activated, this, &cSec1Module::activationContinue);
    connect(m_pMeasProgram, &cSec1ModuleMeasProgram::deactivated, this, &cSec1Module::deactivationContinue);

    for (int i = 0; i < m_ModuleActivistList.count(); i++)
        m_ModuleActivistList.at(i)->generateVeinInterface();
}


void cSec1Module::startMeas()
{
    m_pMeasProgram->start();
}


void cSec1Module::stopMeas()
{
    m_pMeasProgram->stop();
}


void cSec1Module::activationFinished()
{
    m_pModuleValidator->setParameterMap(m_veinModuleParameterMap);

    // now we still have to export the json interface information
    exportMetaData();

    emit activationReady();
}

void cSec1Module::deactivationFinished()
{
    emit deactivationReady();
}

}
