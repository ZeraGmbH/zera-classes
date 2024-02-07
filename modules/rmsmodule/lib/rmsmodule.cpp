#include "rmsmodule.h"
#include "rmsmoduleconfiguration.h"
#include <errormessages.h>

namespace RMSMODULE
{

cRmsModule::cRmsModule(MeasurementModuleFactoryParam moduleParam) :
    cBaseMeasModule(moduleParam, std::shared_ptr<cBaseModuleConfiguration>(new cRmsModuleConfiguration()))
{
    m_sModuleName = QString("%1%2").arg(BaseModuleName).arg(moduleParam.m_moduleNum);
    m_sModuleDescription = QString("This module measures true rms values for configured channels");
    m_sSCPIModuleName = QString("%1%2").arg(BaseSCPIModuleName).arg(moduleParam.m_moduleNum);

    m_ActivationStartState.addTransition(this, &cRmsModule::activationContinue, &m_ActivationExecState);
    m_ActivationExecState.addTransition(this, &cRmsModule::activationContinue, &m_ActivationDoneState);
    m_ActivationDoneState.addTransition(this, &cRmsModule::activationNext, &m_ActivationExecState);
    m_ActivationDoneState.addTransition(this, &cRmsModule::activationContinue, &m_ActivationFinishedState);
    m_ActivationMachine.addState(&m_ActivationStartState);
    m_ActivationMachine.addState(&m_ActivationExecState);
    m_ActivationMachine.addState(&m_ActivationDoneState);
    m_ActivationMachine.addState(&m_ActivationFinishedState);
    m_ActivationMachine.setInitialState(&m_ActivationStartState);
    connect(&m_ActivationStartState, &QAbstractState::entered, this, &cRmsModule::activationStart);
    connect(&m_ActivationExecState, &QAbstractState::entered, this, &cRmsModule::activationExec);
    connect(&m_ActivationDoneState, &QAbstractState::entered, this, &cRmsModule::activationDone);
    connect(&m_ActivationFinishedState, &QAbstractState::entered, this, &cRmsModule::activationFinished);

    m_DeactivationStartState.addTransition(this, &cRmsModule::deactivationContinue, &m_DeactivationExecState);
    m_DeactivationExecState.addTransition(this, &cRmsModule::deactivationContinue, &m_DeactivationDoneState);
    m_DeactivationDoneState.addTransition(this, &cRmsModule::deactivationNext, &m_DeactivationExecState);
    m_DeactivationDoneState.addTransition(this, &cRmsModule::deactivationContinue, &m_DeactivationFinishedState);
    m_DeactivationMachine.addState(&m_DeactivationStartState);
    m_DeactivationMachine.addState(&m_DeactivationExecState);
    m_DeactivationMachine.addState(&m_DeactivationDoneState);
    m_DeactivationMachine.addState(&m_DeactivationFinishedState);
    m_DeactivationMachine.setInitialState(&m_DeactivationStartState);
    connect(&m_DeactivationStartState, &QAbstractState::entered, this, &cRmsModule::deactivationStart);
    connect(&m_DeactivationExecState, &QAbstractState::entered, this, &cRmsModule::deactivationExec);
    connect(&m_DeactivationDoneState, &QAbstractState::entered, this, &cRmsModule::deactivationDone);
    connect(&m_DeactivationFinishedState, &QAbstractState::entered, this, &cRmsModule::deactivationFinished);

}


QByteArray cRmsModule::getConfiguration() const
{
    return m_pConfiguration->exportConfiguration();
}



void cRmsModule::doConfiguration(QByteArray xmlConfigData)
{
    m_pConfiguration->setConfiguration(xmlConfigData);
}


void cRmsModule::setupModule()
{
    emit addEventSystem(m_pModuleValidator);
    cBaseMeasModule::setupModule();

    // we need some program that does the measuring on dsp
    m_pMeasProgram = new cRmsModuleMeasProgram(this, m_pConfiguration);
    m_ModuleActivistList.append(m_pMeasProgram);
    connect(m_pMeasProgram, &cModuleActivist::activated, this, &cRmsModule::activationContinue);
    connect(m_pMeasProgram, &cModuleActivist::deactivated, this, &cRmsModule::deactivationContinue);
    connect(m_pMeasProgram, &cModuleActivist::errMsg, m_pModuleErrorComponent, &VfModuleErrorComponent::setValue);

    for (int i = 0; i < m_ModuleActivistList.count(); i++)
        m_ModuleActivistList.at(i)->generateInterface();
}


void cRmsModule::startMeas()
{
    m_pMeasProgram->start();
}


void cRmsModule::stopMeas()
{
    m_pMeasProgram->stop();
}


void cRmsModule::activationStart()
{
    m_nActivationIt = 0; // we start with the first
    emit activationContinue();
}


void cRmsModule::activationExec()
{
    m_ModuleActivistList.at(m_nActivationIt)->activate();
}


void cRmsModule::activationDone()
{
    m_nActivationIt++;

    if (m_nActivationIt < m_ModuleActivistList.count())
        emit activationNext(); // and iterate over our list
    else
        emit activationContinue();
}


void cRmsModule::activationFinished()
{
    m_pModuleValidator->setParameterHash(veinModuleParameterHash);

    // now we still have to export the json interface information
    exportMetaData();

    emit activationReady();
}


void cRmsModule::deactivationStart()
{
    m_nActivationIt = 0; // we start with the first
    emit deactivationContinue();
}


void cRmsModule::deactivationExec()
{
    m_ModuleActivistList.at(m_nActivationIt)->deactivate();
}


void cRmsModule::deactivationDone()
{
    m_nActivationIt++;

    if (m_nActivationIt < m_ModuleActivistList.count())
        emit deactivationNext(); // and iterate over our list
    else
        emit deactivationContinue();
}


void cRmsModule::deactivationFinished()
{
    emit deactivationReady();
}

}
