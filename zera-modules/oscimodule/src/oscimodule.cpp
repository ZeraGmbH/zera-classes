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
    :cBaseMeasModule(modnr, proxy, entityId, storagesystem, new cOsciModuleConfiguration(), parent)
{
    m_sModuleName = QString("%1%2").arg(BaseModuleName).arg(modnr);
    m_sModuleDescription = QString("This module measures oscillograms for configured channels");
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


cOsciModule::~cOsciModule()
{
    delete m_pConfiguration;
    unsetModule();
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
    pConfData = qobject_cast<cOsciModuleConfiguration*>(m_pConfiguration)->getConfigurationData();

    // we need some program that does the measuring on dsp
    m_pMeasProgram = new cOsciModuleMeasProgram(this, m_pProxy, *pConfData);
    m_ModuleActivistList.append(m_pMeasProgram);
    connect(m_pMeasProgram, SIGNAL(activated()), SIGNAL(activationContinue()));
    connect(m_pMeasProgram, SIGNAL(deactivated()), this, SIGNAL(deactivationContinue()));
    connect(m_pMeasProgram, SIGNAL(errMsg(QVariant)), m_pModuleErrorComponent, SLOT(setValue(QVariant)));

    // and module observation in case we have to react to naming changes
    m_pOsciModuleObservation = new cOsciModuleObservation(this, m_pProxy, &(pConfData->m_PCBServerSocket));
    m_ModuleActivistList.append(m_pOsciModuleObservation);
    connect(m_pOsciModuleObservation, SIGNAL(activated()), SIGNAL(activationContinue()));
    connect(m_pOsciModuleObservation, SIGNAL(deactivated()), this, SIGNAL(deactivationContinue()));
    connect(m_pOsciModuleObservation, SIGNAL(errMsg(QVariant)), m_pModuleErrorComponent, SLOT(setValue(QVariant)));

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
    connect(m_pOsciModuleObservation, SIGNAL(moduleReconfigure()), this, SLOT(osciModuleReconfigure()));

    m_pModuleValidator->setParameterHash(veinModuleParameterHash);
    // now we still have to export the json interface information

    exportMetaData();

    emit activationReady();
}


void cOsciModule::deactivationStart()
{
    // if we get informed we have to reconfigure
    disconnect(m_pOsciModuleObservation, SIGNAL(moduleReconfigure()), this, SLOT(osciModuleReconfigure()));

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
