#include <rminterface.h>
#include <dspinterface.h>
#include <proxy.h>
#include <modulevalidator.h>
#include <veinmodulecomponent.h>
#include <veinmoduleerrorcomponent.h>
#include <veinmodulemetadata.h>

#include "sourcemodule.h"
#include "sourcemoduleconfiguration.h"
#include "sourcemoduleprogram.h"

SourceModule::SourceModule(quint8 modnr, Zera::Proxy::cProxy *proxy, int entityId, VeinEvent::StorageSystem* storagesystem, QObject *parent)
    :cBaseMeasModule(modnr, proxy, entityId, storagesystem, std::shared_ptr<cBaseModuleConfiguration>(new SourceModuleConfiguration()), parent)
{
    m_sModuleName = QString("%1%2").arg(BaseModuleName).arg(modnr);
    m_sModuleDescription = QString("Module to access voltage and current sources");
    m_sSCPIModuleName = QString("%1%2").arg(BaseSCPIModuleName).arg(modnr);

    m_ActivationMachine.setInitialState(&m_ActivationFinishedState);
    connect(&m_ActivationFinishedState, &QState::entered, this, &SourceModule::activationFinished);

    m_DeactivationMachine.setInitialState(&m_DeactivationFinishedState);
    connect(&m_DeactivationFinishedState, &QState::entered, this, &SourceModule::deactivationFinished);
    Q_INIT_RESOURCE(resource);
}

QByteArray SourceModule::getConfiguration() const
{
    return m_pConfiguration->exportConfiguration();
}

void SourceModule::doConfiguration(QByteArray xmlConfigData)
{
    m_pConfiguration->setConfiguration(xmlConfigData);
}


void SourceModule::setupModule()
{
    emit addEventSystem(m_pModuleValidator);
    cBaseModule::setupModule();

    m_pProgram = new SourceModuleProgram(this, m_pConfiguration);
    m_ModuleActivistList.append(m_pProgram);

    for (int i = 0; i < m_ModuleActivistList.count(); i++) {
        m_ModuleActivistList.at(i)->generateInterface();
    }
}


void SourceModule::startMeas()
{
    // nothing to start here
}


void SourceModule::stopMeas()
{
    // also nothing to stop
}


void SourceModule::activationFinished()
{
    // now we still have to export the json interface information, then we are ready
    m_pModuleValidator->setParameterHash(veinModuleParameterHash);
    exportMetaData();

    emit activationReady();
}


void SourceModule::deactivationFinished()
{
    emit deactivationReady();
}
