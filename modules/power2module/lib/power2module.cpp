#include "power2module.h"
#include "power2moduleconfiguration.h"
#include "power2modulemeasprogram.h"
#include <errormessages.h>

namespace POWER2MODULE
{

cPower2Module::cPower2Module(ModuleFactoryParam moduleParam) :
    cBaseMeasModule(moduleParam, std::shared_ptr<BaseModuleConfiguration>(new cPower2ModuleConfiguration()))
{
    m_sModuleName = QString("%1%2").arg(BaseModuleName).arg(moduleParam.m_moduleNum);
    m_sModuleDescription = QString("This module measures +/- power with configured measuring and integration modes");
    m_sSCPIModuleName = QString("%1%2").arg(BaseSCPIModuleName).arg(moduleParam.m_moduleNum);

    m_ActivationStartState.addTransition(this, &cPower2Module::activationContinue, &m_ActivationExecState);
    m_ActivationExecState.addTransition(this, &cPower2Module::activationContinue, &m_ActivationDoneState);
    m_ActivationDoneState.addTransition(this, &cPower2Module::activationNext, &m_ActivationExecState);
    m_ActivationDoneState.addTransition(this, &cPower2Module::activationContinue, &m_ActivationFinishedState);
    m_ActivationMachine.addState(&m_ActivationStartState);
    m_ActivationMachine.addState(&m_ActivationExecState);
    m_ActivationMachine.addState(&m_ActivationDoneState);
    m_ActivationMachine.addState(&m_ActivationFinishedState);
    m_ActivationMachine.setInitialState(&m_ActivationStartState);
    connect(&m_ActivationStartState, &QAbstractState::entered, this, &cPower2Module::activationStart);
    connect(&m_ActivationExecState, &QAbstractState::entered, this, &cPower2Module::activationExec);
    connect(&m_ActivationDoneState, &QAbstractState::entered, this, &cPower2Module::activationDone);
    connect(&m_ActivationFinishedState, &QAbstractState::entered, this, &cPower2Module::activationFinished);

    m_DeactivationStartState.addTransition(this, &cPower2Module::deactivationContinue, &m_DeactivationExecState);
    m_DeactivationExecState.addTransition(this, &cPower2Module::deactivationContinue, &m_DeactivationDoneState);
    m_DeactivationDoneState.addTransition(this, &cPower2Module::deactivationNext, &m_DeactivationExecState);
    m_DeactivationDoneState.addTransition(this, &cPower2Module::deactivationContinue, &m_DeactivationFinishedState);
    m_DeactivationMachine.addState(&m_DeactivationStartState);
    m_DeactivationMachine.addState(&m_DeactivationExecState);
    m_DeactivationMachine.addState(&m_DeactivationDoneState);
    m_DeactivationMachine.addState(&m_DeactivationFinishedState);
    m_DeactivationMachine.setInitialState(&m_DeactivationStartState);
    connect(&m_DeactivationStartState, &QAbstractState::entered, this, &cPower2Module::deactivationStart);
    connect(&m_DeactivationExecState, &QAbstractState::entered, this, &cPower2Module::deactivationExec);
    connect(&m_DeactivationDoneState, &QAbstractState::entered, this, &cPower2Module::deactivationDone);
    connect(&m_DeactivationFinishedState, &QAbstractState::entered, this, &cPower2Module::deactivationFinished);

}


QByteArray cPower2Module::getConfiguration() const
{
    return m_pConfiguration->exportConfiguration();
}



void cPower2Module::setupModule()
{
    emit addEventSystem(m_pModuleValidator);

    cBaseMeasModule::setupModule();

    // we need some program that does the measuring on dsp
    m_pMeasProgram = new cPower2ModuleMeasProgram(this, m_pConfiguration);
    m_ModuleActivistList.append(m_pMeasProgram);
    connect(m_pMeasProgram, &cModuleActivist::activated, this, &BaseModule::activationContinue);
    connect(m_pMeasProgram, &cModuleActivist::deactivated, this, &BaseModule::deactivationContinue);

    for (int i = 0; i < m_ModuleActivistList.count(); i++)
        m_ModuleActivistList.at(i)->generateVeinInterface();
}


void cPower2Module::startMeas()
{
    m_pMeasProgram->start();
}


void cPower2Module::stopMeas()
{
    m_pMeasProgram->stop();
}


void cPower2Module::activationFinished()
{
    m_pModuleValidator->setParameterMap(m_veinModuleParameterMap);

    // now we still have to export the json interface information
    exportMetaData();

    emit activationReady();

}

void cPower2Module::deactivationDone()
{
    m_nActivationIt++;

    if (m_nActivationIt < m_ModuleActivistList.count())
        emit deactivationNext(); // and iterate over our list
    else
        emit deactivationContinue();
}


void cPower2Module::deactivationFinished()
{
    emit deactivationReady();
}

}
