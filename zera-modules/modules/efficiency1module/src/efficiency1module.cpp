#include "efficiency1module.h"
#include "efficiency1moduleconfiguration.h"
#include "efficiency1moduleconfigdata.h"
#include "efficiency1modulemeasprogram.h"
#include <errormessages.h>

namespace EFFICIENCY1MODULE
{

cEfficiency1Module::cEfficiency1Module(quint8 modnr, int entityId, VeinEvent::StorageSystem* storagesystem, QObject* parent) :
    cBaseMeasModule(modnr, entityId, storagesystem, std::shared_ptr<cBaseModuleConfiguration>(new cEfficiency1ModuleConfiguration()), parent)
{
    m_sModuleName = QString("%1%2").arg(BaseModuleName).arg(modnr);
    m_sModuleDescription = QString("This module measures configured number of harmonic power values from configured input values");
    m_sSCPIModuleName = QString("%1%2").arg(BaseSCPIModuleName).arg(modnr);

    m_ActivationStartState.addTransition(this, &cEfficiency1Module::activationContinue, &m_ActivationExecState);
    m_ActivationExecState.addTransition(this, &cEfficiency1Module::activationContinue, &m_ActivationDoneState);
    m_ActivationDoneState.addTransition(this, &cEfficiency1Module::activationNext, &m_ActivationExecState);
    m_ActivationDoneState.addTransition(this, &cEfficiency1Module::activationContinue, &m_ActivationFinishedState);
    m_ActivationMachine.addState(&m_ActivationStartState);
    m_ActivationMachine.addState(&m_ActivationExecState);
    m_ActivationMachine.addState(&m_ActivationDoneState);
    m_ActivationMachine.addState(&m_ActivationFinishedState);
    m_ActivationMachine.setInitialState(&m_ActivationStartState);
    connect(&m_ActivationStartState, &QState::entered, this, &cEfficiency1Module::activationStart);
    connect(&m_ActivationExecState, &QState::entered, this, &cEfficiency1Module::activationExec);
    connect(&m_ActivationDoneState, &QState::entered, this, &cEfficiency1Module::activationDone);
    connect(&m_ActivationFinishedState, &QState::entered, this, &cEfficiency1Module::activationFinished);

    m_DeactivationStartState.addTransition(this, &cEfficiency1Module::deactivationContinue, &m_DeactivationExecState);
    m_DeactivationExecState.addTransition(this, &cEfficiency1Module::deactivationContinue, &m_DeactivationDoneState);
    m_DeactivationDoneState.addTransition(this, &cEfficiency1Module::deactivationNext, &m_DeactivationExecState);
    m_DeactivationDoneState.addTransition(this, &cEfficiency1Module::deactivationContinue, &m_DeactivationFinishedState);
    m_DeactivationMachine.addState(&m_DeactivationStartState);
    m_DeactivationMachine.addState(&m_DeactivationExecState);
    m_DeactivationMachine.addState(&m_DeactivationDoneState);
    m_DeactivationMachine.addState(&m_DeactivationFinishedState);
    m_DeactivationMachine.setInitialState(&m_DeactivationStartState);
    connect(&m_DeactivationStartState, &QState::entered, this, &cEfficiency1Module::deactivationStart);
    connect(&m_DeactivationExecState, &QState::entered, this, &cEfficiency1Module::deactivationExec);
    connect(&m_DeactivationDoneState, &QState::entered, this, &cEfficiency1Module::deactivationDone);
    connect(&m_DeactivationFinishedState, &QState::entered, this, &cEfficiency1Module::deactivationFinished);

}


QByteArray cEfficiency1Module::getConfiguration() const
{
    return m_pConfiguration->exportConfiguration();
}


void cEfficiency1Module::doConfiguration(QByteArray xmlConfigData)
{
    m_pConfiguration->setConfiguration(xmlConfigData);
}


void cEfficiency1Module::setupModule()
{
    emit addEventSystem(m_pModuleValidator);

    cBaseMeasModule::setupModule();

    // we need some program that does the measuring on dsp
    m_pMeasProgram = new cEfficiency1ModuleMeasProgram(this, m_pConfiguration);
    m_ModuleActivistList.append(m_pMeasProgram);
    connect(m_pMeasProgram, &cEfficiency1ModuleMeasProgram::activated, this, &cEfficiency1Module::activationContinue);
    connect(m_pMeasProgram, &cEfficiency1ModuleMeasProgram::deactivated, this, &cEfficiency1Module::deactivationContinue);
    connect(m_pMeasProgram, &cEfficiency1ModuleMeasProgram::errMsg, m_pModuleErrorComponent, &VfModuleErrorComponent::setValue);

    emit addEventSystem(m_pMeasProgram->getEventSystem());

    for (int i = 0; i < m_ModuleActivistList.count(); i++)
        m_ModuleActivistList.at(i)->generateInterface();
}


void cEfficiency1Module::startMeas()
{
    m_pMeasProgram->start();
}


void cEfficiency1Module::stopMeas()
{
    m_pMeasProgram->stop();
}


void cEfficiency1Module::activationStart()
{
    m_nActivationIt = 0; // we start with the first
    emit activationContinue();
}


void cEfficiency1Module::activationExec()
{
    m_ModuleActivistList.at(m_nActivationIt)->activate();
}


void cEfficiency1Module::activationDone()
{
    m_nActivationIt++;

    if (m_nActivationIt < m_ModuleActivistList.count())
        emit activationNext(); // and iterate over our list
    else
        emit activationContinue();
}


void cEfficiency1Module::activationFinished()
{
    m_pModuleValidator->setParameterHash(veinModuleParameterHash);

    // now we still have to export the json interface information
    exportMetaData();

    emit activationReady();
}


void cEfficiency1Module::deactivationStart()
{
    m_nActivationIt = 0; // we start with the first
    emit deactivationContinue();
}


void cEfficiency1Module::deactivationExec()
{
    m_ModuleActivistList.at(m_nActivationIt)->deactivate();
}


void cEfficiency1Module::deactivationDone()
{
    m_nActivationIt++;

    if (m_nActivationIt < m_ModuleActivistList.count())
        emit deactivationNext(); // and iterate over our list
    else
        emit deactivationContinue();
}


void cEfficiency1Module::deactivationFinished()
{
    emit deactivationReady();
}


void cEfficiency1Module::efficiency1ModuleReconfigure()
{
    emit sigConfiguration(); // we configure after our notifier has detected
}

}
