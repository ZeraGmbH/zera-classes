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
#include "power3module.h"
#include "power3moduleconfiguration.h"
#include "power3moduleconfigdata.h"
#include "power3modulemeasprogram.h"
#include "errormessages.h"

namespace POWER3MODULE
{

cPower3Module::cPower3Module(quint8 modnr, Zera::Proxy::cProxy* proxy, int entityId, VeinEvent::StorageSystem* storagesystem, QObject* parent)
    :cBaseMeasModule(modnr, proxy, entityId, storagesystem, new cPower3ModuleConfiguration(), parent)
{
    m_sModuleName = QString("%1%2").arg(BaseModuleName).arg(modnr);
    m_sModuleDescription = QString("This module measures configured number of harmonic power values from configured input values");
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


cPower3Module::~cPower3Module()
{
    delete m_pConfiguration;
}


QByteArray cPower3Module::getConfiguration() const
{
    return m_pConfiguration->exportConfiguration();
}



void cPower3Module::doConfiguration(QByteArray xmlConfigData)
{
    m_pConfiguration->setConfiguration(xmlConfigData);
}


void cPower3Module::setupModule()
{
    emit addEventSystem(m_pModuleValidator);

    cBaseMeasModule::setupModule();

    cPower3ModuleConfigData* pConfData;
    pConfData = qobject_cast<cPower3ModuleConfiguration*>(m_pConfiguration)->getConfigurationData();

    // we need some program that does the measuring on dsp
    m_pMeasProgram = new cPower3ModuleMeasProgram(this, *pConfData);
    m_ModuleActivistList.append(m_pMeasProgram);
    connect(m_pMeasProgram, SIGNAL(activated()), SIGNAL(activationContinue()));
    connect(m_pMeasProgram, SIGNAL(deactivated()), this, SIGNAL(deactivationContinue()));
    connect(m_pMeasProgram, SIGNAL(errMsg(QVariant)), m_pModuleErrorComponent, SLOT(setValue(QVariant)));

    emit addEventSystem(m_pMeasProgram->getEventSystem());

    for (int i = 0; i < m_ModuleActivistList.count(); i++)
        m_ModuleActivistList.at(i)->generateInterface();
}


void cPower3Module::startMeas()
{
    m_pMeasProgram->start();
}


void cPower3Module::stopMeas()
{
    m_pMeasProgram->stop();
}


void cPower3Module::activationStart()
{
    m_nActivationIt = 0; // we start with the first
    emit activationContinue();
}


void cPower3Module::activationExec()
{
    m_ModuleActivistList.at(m_nActivationIt)->activate();
}


void cPower3Module::activationDone()
{
    m_nActivationIt++;

    if (m_nActivationIt < m_ModuleActivistList.count())
        emit activationNext(); // and iterate over our list
    else
        emit activationContinue();
}


void cPower3Module::activationFinished()
{
    m_pModuleValidator->setParameterHash(veinModuleParameterHash);

    // now we still have to export the json interface information
    exportMetaData();

    emit activationReady();
}


void cPower3Module::deactivationStart()
{
    m_nActivationIt = 0; // we start with the first
    emit deactivationContinue();
}


void cPower3Module::deactivationExec()
{
    m_ModuleActivistList.at(m_nActivationIt)->deactivate();
}


void cPower3Module::deactivationDone()
{
    m_nActivationIt++;

    if (m_nActivationIt < m_ModuleActivistList.count())
        emit deactivationNext(); // and iterate over our list
    else
        emit deactivationContinue();
}


void cPower3Module::deactivationFinished()
{
    unsetModule();
    emit deactivationReady();
}


void cPower3Module::power3ModuleReconfigure()
{
    emit sigConfiguration(); // we configure after our notifier has detected
}

}
