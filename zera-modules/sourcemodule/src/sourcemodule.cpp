#include <rminterface.h>
#include <dspinterface.h>
#include <proxy.h>
#include <modulevalidator.h>
#include <veinmodulecomponent.h>
#include <veinmoduleerrorcomponent.h>
#include <veinmodulemetadata.h>

#include "sourcemodule.h"
#include "sourcemoduleconfiguration.h"
#include "sourcemoduleconfigdata.h"
#include "sourcemoduleprogram.h"


namespace SOURCEMODULE
{

cSourceModule::cSourceModule(quint8 modnr, Zera::Proxy::cProxy *proxy, int entityId, VeinEvent::StorageSystem* storagesystem, QObject *parent)
    :cBaseMeasModule(modnr, proxy, entityId, storagesystem, std::shared_ptr<cBaseModuleConfiguration>(new cSourceModuleConfiguration()), parent)
{
    m_sModuleName = QString("%1%2").arg(BaseModuleName).arg(modnr);
    m_sModuleDescription = QString("Module to access voltage and current sources");
    m_sSCPIModuleName = QString("%1%2").arg(BaseSCPIModuleName).arg(modnr);

    m_ActivationMachine.setInitialState(&m_ActivationFinishedState);
    connect(&m_ActivationFinishedState, SIGNAL(entered()), SLOT(activationFinished()));

    m_DeactivationMachine.setInitialState(&m_DeactivationFinishedState);
    connect(&m_DeactivationFinishedState, SIGNAL(entered()), SLOT(deactivationFinished()));
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

    // we only have this activist
    m_pProgram = new cSourceModuleProgram(this, m_pProxy, m_pConfiguration);
    m_ModuleActivistList.append(m_pProgram);
    /*connect(m_pProgram, SIGNAL(activated()), SIGNAL(activationContinue()));
    connect(m_pProgram, SIGNAL(deactivated()), this, SIGNAL(deactivationContinue()));
    connect(m_pProgram, SIGNAL(errMsg(QVariant)), m_pModuleErrorComponent, SLOT(setValue(QVariant)));*/

    // For now m_ModuleActivistList is empty
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
