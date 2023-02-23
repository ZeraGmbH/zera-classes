#include "power1module.h"
#include "power1moduleconfiguration.h"
#include "power1moduleconfigdata.h"
#include "power1modulemeasprogram.h"
#include "power1moduleobservation.h"
#include <errormessages.h>

namespace POWER1MODULE
{

cPower1Module::cPower1Module(quint8 modnr, int entityId, VeinEvent::StorageSystem* storagesystem, QObject* parent) :
    cBaseMeasModule(modnr, entityId, storagesystem, std::shared_ptr<cBaseModuleConfiguration>(new cPower1ModuleConfiguration()), parent)
{
    m_inputComponentEventSystem = new VfEventSystemInputComponents;
    m_sModuleName = QString("%1%2").arg(BaseModuleName).arg(modnr);
    m_sModuleDescription = QString("This module measures power with configured measuring and integration modes");
    m_sSCPIModuleName = QString("%1%2").arg(BaseSCPIModuleName).arg(modnr);

    m_ActivationStartState.addTransition(this, &cPower1Module::activationContinue, &m_ActivationExecState);
    m_ActivationExecState.addTransition(this, &cPower1Module::activationContinue, &m_ActivationDoneState);
    m_ActivationDoneState.addTransition(this, &cPower1Module::activationNext, &m_ActivationExecState);
    m_ActivationDoneState.addTransition(this, &cPower1Module::activationContinue, &m_ActivationFinishedState);
    m_ActivationMachine.addState(&m_ActivationStartState);
    m_ActivationMachine.addState(&m_ActivationExecState);
    m_ActivationMachine.addState(&m_ActivationDoneState);
    m_ActivationMachine.addState(&m_ActivationFinishedState);
    m_ActivationMachine.setInitialState(&m_ActivationStartState);
    connect(&m_ActivationStartState, &QAbstractState::entered, this, &cPower1Module::activationStart);
    connect(&m_ActivationExecState, &QAbstractState::entered, this, &cPower1Module::activationExec);
    connect(&m_ActivationDoneState, &QAbstractState::entered, this, &cPower1Module::activationDone);
    connect(&m_ActivationFinishedState, &QAbstractState::entered, this, &cPower1Module::activationFinished);

    m_DeactivationStartState.addTransition(this, &cPower1Module::deactivationContinue, &m_DeactivationExecState);
    m_DeactivationExecState.addTransition(this, &cPower1Module::deactivationContinue, &m_DeactivationDoneState);
    m_DeactivationDoneState.addTransition(this, &cPower1Module::deactivationNext, &m_DeactivationExecState);
    m_DeactivationDoneState.addTransition(this, &cPower1Module::deactivationContinue, &m_DeactivationFinishedState);
    m_DeactivationMachine.addState(&m_DeactivationStartState);
    m_DeactivationMachine.addState(&m_DeactivationExecState);
    m_DeactivationMachine.addState(&m_DeactivationDoneState);
    m_DeactivationMachine.addState(&m_DeactivationFinishedState);
    m_DeactivationMachine.setInitialState(&m_DeactivationStartState);
    connect(&m_DeactivationStartState, &QAbstractState::entered, this, &cPower1Module::deactivationStart);
    connect(&m_DeactivationExecState, &QAbstractState::entered, this, &cPower1Module::deactivationExec);
    connect(&m_DeactivationDoneState, &QAbstractState::entered, this, &cPower1Module::deactivationDone);
    connect(&m_DeactivationFinishedState, &QAbstractState::entered, this, &cPower1Module::deactivationFinished);

}

QByteArray cPower1Module::getConfiguration() const
{
    return m_pConfiguration->exportConfiguration();
}

VfEventSystemInputComponents *cPower1Module::getPEventSystem() const
{
    return m_inputComponentEventSystem;
}

void cPower1Module::doConfiguration(QByteArray xmlConfigData)
{
    m_pConfiguration->setConfiguration(xmlConfigData);
}

void cPower1Module::setupModule()
{
    emit addEventSystem(m_pModuleValidator);
    emit addEventSystem(m_inputComponentEventSystem);// Add Eventsystem for input components as raw pointer
    cBaseMeasModule::setupModule();

    cPower1ModuleConfigData* pConfData;
    pConfData = qobject_cast<cPower1ModuleConfiguration*>(m_pConfiguration.get())->getConfigurationData();

    // we need some program that does the measuring on dsp
    m_pMeasProgram = new cPower1ModuleMeasProgram(this, m_pConfiguration);
    m_ModuleActivistList.append(m_pMeasProgram);
    connect(m_pMeasProgram, &cModuleActivist::activated, this, &cBaseModule::activationContinue);
    connect(m_pMeasProgram, &cModuleActivist::deactivated, this, &cBaseModule::deactivationContinue);
    connect(m_pMeasProgram, &cModuleActivist::errMsg, m_pModuleErrorComponent, &VfModuleErrorComponent::setValue);

    // and module observation in case we have to react to naming changes
    m_pPower1ModuleObservation = new cPower1ModuleObservation(this, &(pConfData->m_PCBServerSocket));
    m_ModuleActivistList.append(m_pPower1ModuleObservation);
    connect(m_pPower1ModuleObservation, &cModuleActivist::activated, this, &cBaseModule::activationContinue);
    connect(m_pPower1ModuleObservation, &cModuleActivist::deactivated, this, &cBaseModule::deactivationContinue);
    connect(m_pPower1ModuleObservation, &cModuleActivist::errMsg, m_pModuleErrorComponent, &VfModuleErrorComponent::setValue);

    for (int i = 0; i < m_ModuleActivistList.count(); i++)
        m_ModuleActivistList.at(i)->generateInterface();
}


void cPower1Module::startMeas()
{
    m_pMeasProgram->start();
}


void cPower1Module::stopMeas()
{
    m_pMeasProgram->stop();
}


void cPower1Module::activationStart()
{
    m_nActivationIt = 0; // we start with the first
    emit activationContinue();
}


void cPower1Module::activationExec()
{
    m_ModuleActivistList.at(m_nActivationIt)->activate();
}


void cPower1Module::activationDone()
{
    m_nActivationIt++;

    if (m_nActivationIt < m_ModuleActivistList.count())
        emit activationNext(); // and iterate over our list
    else
        emit activationContinue();
}


void cPower1Module::activationFinished()
{
    // if we get informed we have to reconfigure
    connect(m_pPower1ModuleObservation, &cPower1ModuleObservation::moduleReconfigure, this, &cPower1Module::power1ModuleReconfigure);

    m_pModuleValidator->setParameterHash(veinModuleParameterHash);

    // now we still have to export the json interface information
    exportMetaData();

    emit activationReady();
}


void cPower1Module::deactivationStart()
{
    // if we get informed we have to reconfigure
    disconnect(m_pPower1ModuleObservation, &cPower1ModuleObservation::moduleReconfigure, this, &cPower1Module::power1ModuleReconfigure);

    m_nActivationIt = 0; // we start with the first
    emit deactivationContinue();
}


void cPower1Module::deactivationExec()
{
    m_ModuleActivistList.at(m_nActivationIt)->deactivate();
}


void cPower1Module::deactivationDone()
{
    m_nActivationIt++;

    if (m_nActivationIt < m_ModuleActivistList.count())
        emit deactivationNext(); // and iterate over our list
    else
        emit deactivationContinue();
}


void cPower1Module::deactivationFinished()
{
    emit deactivationReady();
}


void cPower1Module::power1ModuleReconfigure()
{
    emit sigConfiguration(); // we configure after our notifier has detected
}

}
