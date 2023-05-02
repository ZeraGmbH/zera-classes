#include "power2module.h"
#include "power2moduleconfiguration.h"
#include "power2moduleconfigdata.h"
#include "power2modulemeasprogram.h"
#include <errormessages.h>

namespace POWER2MODULE
{

cPower2Module::cPower2Module(quint8 modnr, int entityId, VeinEvent::StorageSystem* storagesystem, QObject* parent) :
    cBaseMeasModule(modnr, entityId, storagesystem, std::shared_ptr<cBaseModuleConfiguration>(new cPower2ModuleConfiguration()), parent)
{
    m_sModuleName = QString("%1%2").arg(BaseModuleName).arg(modnr);
    m_sModuleDescription = QString("This module measures +/- power with configured measuring and integration modes");
    m_sSCPIModuleName = QString("%1%2").arg(BaseSCPIModuleName).arg(modnr);

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



void cPower2Module::doConfiguration(QByteArray xmlConfigData)
{
    m_pConfiguration->setConfiguration(xmlConfigData);
}


void cPower2Module::setupModule()
{
    emit addEventSystem(m_pModuleValidator);

    cBaseMeasModule::setupModule();

    // we need some program that does the measuring on dsp
    m_pMeasProgram = new cPower2ModuleMeasProgram(this, m_pConfiguration);
    m_ModuleActivistList.append(m_pMeasProgram);
    connect(m_pMeasProgram, &cModuleActivist::activated, this, &cBaseModule::activationContinue);
    connect(m_pMeasProgram, &cModuleActivist::deactivated, this, &cBaseModule::deactivationContinue);
    connect(m_pMeasProgram, &cModuleActivist::errMsg, m_pModuleErrorComponent, &VfModuleErrorComponent::setValue);

    for (int i = 0; i < m_ModuleActivistList.count(); i++)
        m_ModuleActivistList.at(i)->generateInterface();
}


void cPower2Module::startMeas()
{
    m_pMeasProgram->start();
}


void cPower2Module::stopMeas()
{
    m_pMeasProgram->stop();
}


void cPower2Module::activationStart()
{
    m_nActivationIt = 0; // we start with the first
    emit activationContinue();
}


void cPower2Module::activationExec()
{
    m_ModuleActivistList.at(m_nActivationIt)->activate();
}


void cPower2Module::activationDone()
{
    m_nActivationIt++;

    if (m_nActivationIt < m_ModuleActivistList.count())
        emit activationNext(); // and iterate over our list
    else
        emit activationContinue();
}


void cPower2Module::activationFinished()
{
    m_pModuleValidator->setParameterHash(veinModuleParameterHash);

    // now we still have to export the json interface information
    exportMetaData();

    emit activationReady();

}


void cPower2Module::deactivationStart()
{
    m_nActivationIt = 0; // we start with the first
    emit deactivationContinue();
}


void cPower2Module::deactivationExec()
{
    m_ModuleActivistList.at(m_nActivationIt)->deactivate();
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


void cPower2Module::power2ModuleReconfigure()
{
    emit sigConfiguration(); // we configure after our notifier has detected
}

}
