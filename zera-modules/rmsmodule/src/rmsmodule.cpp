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
#include "rmsmodule.h"
#include "rmsmoduleconfiguration.h"
#include "rmsmoduleconfigdata.h"
#include "rmsmodulemeasprogram.h"
#include "rmsmoduleobservation.h"
#include "errormessages.h"

namespace RMSMODULE
{

cRmsModule::cRmsModule(quint8 modnr, Zera::Proxy::cProxy* proxi, int entityId, VeinEvent::StorageSystem* storagesystem, QObject* parent)
    :cBaseMeasModule(modnr, proxi, entityId, storagesystem, new cRmsModuleConfiguration(), parent)
{
    m_sModuleName = QString("%1%2").arg(BaseModuleName).arg(modnr);
    m_sModuleDescription = QString("This module measures true rms values for configured channels");
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


cRmsModule::~cRmsModule()
{
    delete m_pConfiguration;
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

    cRmsModuleConfigData* pConfData;
    pConfData = qobject_cast<cRmsModuleConfiguration*>(m_pConfiguration)->getConfigurationData();

    // we need some program that does the measuring on dsp
    m_pMeasProgram = new cRmsModuleMeasProgram(this, m_pProxy, *pConfData);
    m_ModuleActivistList.append(m_pMeasProgram);
    connect(m_pMeasProgram, SIGNAL(activated()), SIGNAL(activationContinue()));
    connect(m_pMeasProgram, SIGNAL(deactivated()), this, SIGNAL(deactivationContinue()));
    connect(m_pMeasProgram, SIGNAL(errMsg(QVariant)), m_pModuleErrorComponent, SLOT(setValue(QVariant)));

    // and module observation in case we have to react to naming changes
    m_pRmsModuleObservation = new cRmsModuleObservation(this, m_pProxy, &(pConfData->m_PCBServerSocket));
    m_ModuleActivistList.append(m_pRmsModuleObservation);
    connect(m_pRmsModuleObservation, SIGNAL(activated()), SIGNAL(activationContinue()));
    connect(m_pRmsModuleObservation, SIGNAL(deactivated()), this, SIGNAL(deactivationContinue()));
    connect(m_pRmsModuleObservation, SIGNAL(errMsg(QVariant)), m_pModuleErrorComponent, SLOT(setValue(QVariant)));

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
    // if we get informed we have to reconfigure
    connect(m_pRmsModuleObservation, SIGNAL(moduleReconfigure()), this, SLOT(rmsModuleReconfigure()));

    m_pModuleValidator->setParameterHash(veinModuleParameterHash);

    // now we still have to export the json interface information
    exportMetaData();

    emit activationReady();
}


void cRmsModule::deactivationStart()
{
    // if we get informed we have to reconfigure
    disconnect(m_pRmsModuleObservation, SIGNAL(moduleReconfigure()), this, SLOT(rmsModuleReconfigure()));

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
    unsetModule();
    emit deactivationReady();
}


void cRmsModule::rmsModuleReconfigure()
{
    emit sigConfiguration(); // we configure after our notifier has detected
}

}
