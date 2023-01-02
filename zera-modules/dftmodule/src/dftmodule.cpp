#include "dftmodule.h"
#include "dftmoduleconfiguration.h"
#include "dftmoduleconfigdata.h"
#include "dftmodulemeasprogram.h"
#include "dftmoduleobservation.h"
#include <errormessages.h>

namespace DFTMODULE
{

cDftModule::cDftModule(quint8 modnr, Zera::Proxy::cProxy* proxi, int entityId, VeinEvent::StorageSystem* storagesystem, QObject* parent)
    :cBaseMeasModule(modnr, proxi, entityId, storagesystem, std::shared_ptr<cBaseModuleConfiguration>(new cDftModuleConfiguration()), parent)
{
    m_sModuleName = QString("%1%2").arg(BaseModuleName).arg(modnr);
    m_sModuleDescription = QString("This module measures configured order dft values for configured channels");
    m_sSCPIModuleName = QString("%1%2").arg(BaseSCPIModuleName).arg(modnr);

    m_ActivationStartState.addTransition(this, &cDftModule::activationContinue, &m_ActivationExecState);
    m_ActivationExecState.addTransition(this, &cDftModule::activationContinue, &m_ActivationDoneState);
    m_ActivationDoneState.addTransition(this, &cDftModule::activationNext, &m_ActivationExecState);
    m_ActivationDoneState.addTransition(this, &cDftModule::activationContinue, &m_ActivationFinishedState);
    m_ActivationMachine.addState(&m_ActivationStartState);
    m_ActivationMachine.addState(&m_ActivationExecState);
    m_ActivationMachine.addState(&m_ActivationDoneState);
    m_ActivationMachine.addState(&m_ActivationFinishedState);
    m_ActivationMachine.setInitialState(&m_ActivationStartState);
    connect(&m_ActivationStartState, &QState::entered, this, &cDftModule::activationStart);
    connect(&m_ActivationExecState, &QState::entered, this, &cDftModule::activationExec);
    connect(&m_ActivationDoneState, &QState::entered, this, &cDftModule::activationDone);
    connect(&m_ActivationFinishedState, &QState::entered, this, &cDftModule::activationFinished);

    m_DeactivationStartState.addTransition(this, &cDftModule::deactivationContinue, &m_DeactivationExecState);
    m_DeactivationExecState.addTransition(this, &cDftModule::deactivationContinue, &m_DeactivationDoneState);
    m_DeactivationDoneState.addTransition(this, &cDftModule::deactivationNext, &m_DeactivationExecState);
    m_DeactivationDoneState.addTransition(this, &cDftModule::deactivationContinue, &m_DeactivationFinishedState);
    m_DeactivationMachine.addState(&m_DeactivationStartState);
    m_DeactivationMachine.addState(&m_DeactivationExecState);
    m_DeactivationMachine.addState(&m_DeactivationDoneState);
    m_DeactivationMachine.addState(&m_DeactivationFinishedState);
    m_DeactivationMachine.setInitialState(&m_DeactivationStartState);
    connect(&m_DeactivationStartState, &QState::entered, this, &cDftModule::deactivationStart);
    connect(&m_DeactivationExecState, &QState::entered, this, &cDftModule::deactivationExec);
    connect(&m_DeactivationDoneState, &QState::entered, this, &cDftModule::deactivationDone);
    connect(&m_DeactivationFinishedState, &QState::entered, this, &cDftModule::deactivationFinished);

}


QByteArray cDftModule::getConfiguration() const
{
    return m_pConfiguration->exportConfiguration();
}



void cDftModule::doConfiguration(QByteArray xmlConfigData)
{
    m_pConfiguration->setConfiguration(xmlConfigData);
}


void cDftModule::setupModule()
{
    emit addEventSystem(m_pModuleValidator);
    cBaseMeasModule::setupModule();

    cDftModuleConfigData* pConfData;
    pConfData = qobject_cast<cDftModuleConfiguration*>(m_pConfiguration.get())->getConfigurationData();

    // we need some program that does the measuring on dsp
    m_pMeasProgram = new cDftModuleMeasProgram(this, m_pProxy, m_pConfiguration);
    m_ModuleActivistList.append(m_pMeasProgram);
    connect(m_pMeasProgram, &cDftModuleMeasProgram::activated, this, &cDftModule::activationContinue);
    connect(m_pMeasProgram, &cDftModuleMeasProgram::deactivated, this, &cDftModule::deactivationContinue);
    connect(m_pMeasProgram, &cDftModuleMeasProgram::errMsg, m_pModuleErrorComponent, &VfModuleErrorComponent::setValue);

    // and module observation in case we have to react to naming changes
    m_pDftModuleObservation = new cDftModuleObservation(this, m_pProxy, &(pConfData->m_PCBServerSocket));
    m_ModuleActivistList.append(m_pDftModuleObservation);
    connect(m_pDftModuleObservation, &cDftModuleObservation::activated, this, &cDftModule::activationContinue);
    connect(m_pDftModuleObservation, &cDftModuleObservation::deactivated, this, &cDftModule::deactivationContinue);
    connect(m_pDftModuleObservation, &cDftModuleObservation::errMsg, m_pModuleErrorComponent, &VfModuleErrorComponent::setValue);

    for (int i = 0; i < m_ModuleActivistList.count(); i++)
        m_ModuleActivistList.at(i)->generateInterface();
}


void cDftModule::startMeas()
{
    m_pMeasProgram->start();
}


void cDftModule::stopMeas()
{
    m_pMeasProgram->stop();
}


void cDftModule::activationStart()
{
    m_nActivationIt = 0; // we start with the first
    emit activationContinue();
}


void cDftModule::activationExec()
{
    m_ModuleActivistList.at(m_nActivationIt)->activate();
}


void cDftModule::activationDone()
{
    m_nActivationIt++;

    if (m_nActivationIt < m_ModuleActivistList.count())
        emit activationNext(); // and iterate over our list
    else
        emit activationContinue();
}


void cDftModule::activationFinished()
{
    // if we get informed we have to reconfigure
    connect(m_pDftModuleObservation, &cDftModuleObservation::moduleReconfigure, this, &cDftModule::dftModuleReconfigure);

    m_pModuleValidator->setParameterHash(veinModuleParameterHash);

    // now we still have to export the json interface information
    exportMetaData();

    emit activationReady();
}


void cDftModule::deactivationStart()
{
    // if we get informed we have to reconfigure
    disconnect(m_pDftModuleObservation, &cDftModuleObservation::moduleReconfigure, this, &cDftModule::dftModuleReconfigure);

    m_nActivationIt = 0; // we start with the first
    emit deactivationContinue();
}


void cDftModule::deactivationExec()
{
    m_ModuleActivistList.at(m_nActivationIt)->deactivate();
}


void cDftModule::deactivationDone()
{
    m_nActivationIt++;

    if (m_nActivationIt < m_ModuleActivistList.count())
        emit deactivationNext(); // and iterate over our list
    else
        emit deactivationContinue();
}


void cDftModule::deactivationFinished()
{
    emit deactivationReady();
}


void cDftModule::dftModuleReconfigure()
{
    emit sigConfiguration(); // we configure after our notifier has detected
}

}
