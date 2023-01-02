#include "thdnmodule.h"
#include "thdnmoduleconfiguration.h"
#include "thdnmoduleconfigdata.h"
#include "thdnmodulemeasprogram.h"
#include "thdnmoduleobservation.h"
#include "errormessages.h"
#include <proxy.h>
#include <vfmodulecomponent.h>
#include <vfmoduleerrorcomponent.h>
#include <vfmodulemetadata.h>
#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>

namespace THDNMODULE
{

cThdnModule::cThdnModule(quint8 modnr, Zera::Proxy::cProxy *proxy, int entityId, VeinEvent::StorageSystem *storagesystem, QObject *parent)
    :cBaseMeasModule(modnr, proxy, entityId, storagesystem, std::shared_ptr<cBaseModuleConfiguration>(new cThdnModuleConfiguration()), parent)
{
    m_sModuleName = QString("%1%2").arg(BaseModuleName).arg(modnr);
    m_sModuleDescription = QString("This module measures thdn values for configured channels");
    m_sSCPIModuleName = QString("%1%2").arg(BaseSCPIModuleName).arg(modnr);

    m_ActivationStartState.addTransition(this, SIGNAL(activationContinue()), &m_ActivationExecState);
    m_ActivationExecState.addTransition(this, SIGNAL(activationContinue()), &m_ActivationDoneState);
    m_ActivationDoneState.addTransition(this, SIGNAL(activationNext()), &m_ActivationExecState);
    m_ActivationDoneState.addTransition(this, SIGNAL(activationContinue()), &m_ActivationFinishedState);
    m_ActivationMachine.addState(&m_ActivationStartState);
    m_ActivationMachine.addState(&m_ActivationExecState);
    m_ActivationMachine.addState(&m_ActivationDoneState);
    m_ActivationMachine.addState(&m_ActivationFinishedState);
    m_ActivationMachine.setInitialState(&m_ActivationStartState);
    connect(&m_ActivationStartState, SIGNAL(entered()), SLOT(activationStart()));
    connect(&m_ActivationExecState, SIGNAL(entered()), SLOT(activationExec()));
    connect(&m_ActivationDoneState, SIGNAL(entered()), SLOT(activationDone()));
    connect(&m_ActivationFinishedState, SIGNAL(entered()), SLOT(activationFinished()));

    m_DeactivationStartState.addTransition(this, SIGNAL(deactivationContinue()), &m_DeactivationExecState);
    m_DeactivationExecState.addTransition(this, SIGNAL(deactivationContinue()), &m_DeactivationDoneState);
    m_DeactivationDoneState.addTransition(this, SIGNAL(deactivationNext()), &m_DeactivationExecState);
    m_DeactivationDoneState.addTransition(this, SIGNAL(deactivationContinue()), &m_DeactivationFinishedState);
    m_DeactivationMachine.addState(&m_DeactivationStartState);
    m_DeactivationMachine.addState(&m_DeactivationExecState);
    m_DeactivationMachine.addState(&m_DeactivationDoneState);
    m_DeactivationMachine.addState(&m_DeactivationFinishedState);
    m_DeactivationMachine.setInitialState(&m_DeactivationStartState);
    connect(&m_DeactivationStartState, SIGNAL(entered()), SLOT(deactivationStart()));
    connect(&m_DeactivationExecState, SIGNAL(entered()), SLOT(deactivationExec()));
    connect(&m_DeactivationDoneState, SIGNAL(entered()), SLOT(deactivationDone()));
    connect(&m_DeactivationFinishedState, SIGNAL(entered()), SLOT(deactivationFinished()));

}


QByteArray cThdnModule::getConfiguration() const
{
    return m_pConfiguration->exportConfiguration();
}



void cThdnModule::doConfiguration(QByteArray xmlConfigData)
{
    m_pConfiguration->setConfiguration(xmlConfigData);
}


void cThdnModule::setupModule()
{
    emit addEventSystem(m_pModuleValidator);
    cBaseMeasModule::setupModule();

    cThdnModuleConfigData* pConfData;
    pConfData = qobject_cast<cThdnModuleConfiguration*>(m_pConfiguration.get())->getConfigurationData();

    // we need some program that does the measuring on dsp
    m_pMeasProgram = new cThdnModuleMeasProgram(this, m_pProxy, m_pConfiguration);
    m_ModuleActivistList.append(m_pMeasProgram);
    connect(m_pMeasProgram, SIGNAL(activated()), SIGNAL(activationContinue()));
    connect(m_pMeasProgram, SIGNAL(deactivated()), this, SIGNAL(deactivationContinue()));
    connect(m_pMeasProgram, SIGNAL(errMsg(QVariant)), m_pModuleErrorComponent, SLOT(setValue(QVariant)));

    // and module observation in case we have to react to naming changes
    m_pThdnModuleObservation = new cThdnModuleObservation(this, m_pProxy, &(pConfData->m_PCBServerSocket));
    m_ModuleActivistList.append(m_pThdnModuleObservation);
    connect(m_pThdnModuleObservation, SIGNAL(activated()), SIGNAL(activationContinue()));
    connect(m_pThdnModuleObservation, SIGNAL(deactivated()), this, SIGNAL(deactivationContinue()));
    connect(m_pThdnModuleObservation, SIGNAL(errMsg(QVariant)), m_pModuleErrorComponent, SLOT(setValue(QVariant)));

    for (int i = 0; i < m_ModuleActivistList.count(); i++)
        m_ModuleActivistList.at(i)->generateInterface();
}


void cThdnModule::startMeas()
{
    m_pMeasProgram->start();
}


void cThdnModule::stopMeas()
{
    m_pMeasProgram->stop();
}


void cThdnModule::activationStart()
{
    m_nActivationIt = 0; // we start with the first
    emit activationContinue();
}


void cThdnModule::activationExec()
{
    m_ModuleActivistList.at(m_nActivationIt)->activate();
}


void cThdnModule::activationDone()
{
    m_nActivationIt++;

    if (m_nActivationIt < m_ModuleActivistList.count())
        emit activationNext(); // and iterate over our list
    else
        emit activationContinue();
}


void cThdnModule::activationFinished()
{
    // if we get informed we have to reconfigure
    connect(m_pThdnModuleObservation, SIGNAL(moduleReconfigure()), this, SLOT(thdnModuleReconfigure()));

    m_pModuleValidator->setParameterHash(veinModuleParameterHash);
    // now we still have to export the json interface information

    exportMetaData();

    emit activationReady();
}


void cThdnModule::deactivationStart()
{
    // if we get informed we have to reconfigure
    disconnect(m_pThdnModuleObservation, SIGNAL(moduleReconfigure()), this, SLOT(thdnModuleReconfigure()));

    m_nActivationIt = 0; // we start with the first
    emit deactivationContinue();
}


void cThdnModule::deactivationExec()
{
    m_ModuleActivistList.at(m_nActivationIt)->deactivate();
}


void cThdnModule::deactivationDone()
{
    m_nActivationIt++;

    if (m_nActivationIt < m_ModuleActivistList.count())
        emit deactivationNext(); // and iterate over our list
    else
        emit deactivationContinue();
}


void cThdnModule::deactivationFinished()
{
    emit deactivationReady();
}


void cThdnModule::thdnModuleReconfigure()
{
    emit sigConfiguration(); // we configure after our notifier has detected
}

}
