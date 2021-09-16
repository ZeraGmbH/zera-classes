#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>

#include <rminterface.h>
#include <dspinterface.h>
#include <proxy.h>
#include <modulevalidator.h>
#include <veinmodulecomponent.h>
#include <veinmoduleerrorcomponent.h>
#include <veinmodulemetadata.h>

#include "debug.h"
#include "oscimodule.h"
#include "oscimoduleconfiguration.h"
#include "oscimoduleconfigdata.h"
#include "oscimodulemeasprogram.h"
#include "oscimoduleobservation.h"

#include "errormessages.h"

namespace OSCIMODULE
{

cOsciModule::cOsciModule(quint8 modnr, Zera::Proxy::cProxy* proxy, int entityId, VeinEvent::StorageSystem* storagesystem, QObject* parent)
    :cBaseMeasModule(modnr, proxy, entityId, storagesystem, std::shared_ptr<cBaseModuleConfiguration>(new cOsciModuleConfiguration()), parent)
{
    m_sModuleName = QString("%1%2").arg(BaseModuleName).arg(modnr);
    m_sModuleDescription = QString("This module measures oscillograms for configured channels");
    m_sSCPIModuleName = QString("%1%2").arg(BaseSCPIModuleName).arg(modnr);

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



void cOsciModule::doConfiguration(QByteArray xmlConfigData)
{
    m_pConfiguration->setConfiguration(xmlConfigData);
}


void cOsciModule::setupModule()
{
    emit addEventSystem(m_pModuleValidator);
    cBaseMeasModule::setupModule();

    cOsciModuleConfigData* pConfData;
    pConfData = qobject_cast<cOsciModuleConfiguration*>(m_pConfiguration.get())->getConfigurationData();

    // we need some program that does the measuring on dsp
    m_pMeasProgram = new cOsciModuleMeasProgram(this, m_pProxy, m_pConfiguration);
    m_ModuleActivistList.append(m_pMeasProgram);
    connect(m_pMeasProgram, &cOsciModuleMeasProgram::activated, this, &cOsciModule::activationContinue);
    connect(m_pMeasProgram, &cOsciModuleMeasProgram::deactivated, this, &cOsciModule::deactivationContinue);
    connect(m_pMeasProgram, &cOsciModuleMeasProgram::errMsg, m_pModuleErrorComponent, &cVeinModuleErrorComponent::setValue);

    // and module observation in case we have to react to naming changes
    m_pOsciModuleObservation = new cOsciModuleObservation(this, m_pProxy, &(pConfData->m_PCBServerSocket));
    m_ModuleActivistList.append(m_pOsciModuleObservation);
    connect(m_pOsciModuleObservation, &cOsciModuleObservation::activated, this, &cOsciModule::activationContinue);
    connect(m_pOsciModuleObservation, &cOsciModuleObservation::deactivated, this, &cOsciModule::deactivationContinue);
    connect(m_pOsciModuleObservation, &cOsciModuleObservation::errMsg, m_pModuleErrorComponent, &cVeinModuleErrorComponent::setValue);

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
    // if we get informed we have to reconfigure
    connect(m_pOsciModuleObservation, &cOsciModuleObservation::moduleReconfigure, this, &cOsciModule::osciModuleReconfigure);

    m_pModuleValidator->setParameterHash(veinModuleParameterHash);
    // now we still have to export the json interface information

    exportMetaData();

    emit activationReady();
}


void cOsciModule::deactivationStart()
{
    // if we get informed we have to reconfigure
    disconnect(m_pOsciModuleObservation, &cOsciModuleObservation::moduleReconfigure, this, &cOsciModule::osciModuleReconfigure);

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


void cOsciModule::osciModuleReconfigure()
{
    emit sigConfiguration(); // we configure after our notifier has detected
}

}
