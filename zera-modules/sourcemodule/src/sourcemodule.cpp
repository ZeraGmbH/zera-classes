#include <rminterface.h>
#include <dspinterface.h>
#include <proxy.h>
#include <modulevalidator.h>
#include <veinmodulecomponent.h>
#include <veinmoduleerrorcomponent.h>
#include <veinmodulemetadata.h>

#include "sourcemodule.h"
#include "sourcemoduleconfiguration.h"

namespace SOURCEMODULE
{

cSourceModule::cSourceModule(quint8 modnr, Zera::Proxy::cProxy *proxy, int entityId, VeinEvent::StorageSystem* storagesystem, QObject *parent)
    :cBaseMeasModule(modnr, proxy, entityId, storagesystem, std::shared_ptr<cBaseModuleConfiguration>(new cSourceModuleConfiguration()), parent)
{
    m_sModuleName = QString("%1%2").arg(BaseModuleName).arg(modnr);
    m_sModuleDescription = QString("Module to access voltage and current sources");
    m_sSCPIModuleName = QString("%1%2").arg(BaseSCPIModuleName).arg(modnr);

    m_ActivationMachine.setInitialState(&m_ActivationFinishedState);
    connect(&m_ActivationFinishedState, &QState::entered, this, &cSourceModule::activationFinished);

    m_DeactivationMachine.setInitialState(&m_DeactivationFinishedState);
    connect(&m_DeactivationFinishedState, &QState::entered, this, &cSourceModule::deactivationFinished);
}

QByteArray cSourceModule::getConfiguration() const
{
    return m_pConfiguration->exportConfiguration();
}


void cSourceModule::doConfiguration(QByteArray xmlConfigData)
{
    m_pConfiguration->setConfiguration(xmlConfigData);
}


void cSourceModule::setupModule()
{
    emit addEventSystem(m_pModuleValidator);
    cBaseModule::setupModule();

    // no modules to activate yet - TODO after demo implementation
    for (int i = 0; i < m_ModuleActivistList.count(); i++) {
        m_ModuleActivistList.at(i)->generateInterface();
    }
}


void cSourceModule::startMeas()
{
    // nothing to start here
}


void cSourceModule::stopMeas()
{
    // also nothing to stop
}


void cSourceModule::activationFinished()
{
    // now we still have to export the json interface information, then we are ready
    m_pModuleValidator->setParameterHash(veinModuleParameterHash);
    exportMetaData();

    emit activationReady();
}


void cSourceModule::deactivationFinished()
{
    emit deactivationReady();
}

}
