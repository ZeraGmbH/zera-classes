#include "electronicburdenmodule.h"
#include "electronicburdenmoduleconfiguration.h"
#include "electronicburdenmoduleprogram.h"

namespace ELECTRONICBURDENMODULE
{

ElectronicBurdenModule::ElectronicBurdenModule(quint8 modnr, int entityId, VeinEvent::StorageSystem* storagesystem, QObject *parent) :
    cBaseMeasModule(modnr, entityId, storagesystem, std::shared_ptr<cBaseModuleConfiguration>(new ElectronicBurdenModuleConfiguration()), parent)
{
    m_sModuleName = QString("%1%2").arg(BaseModuleName).arg(modnr);
    m_sModuleDescription = QString("Module to control voltage and current burden");
    m_sSCPIModuleName = QString("%1%2").arg(BaseSCPIModuleName).arg(modnr);

    m_ActivationMachine.setInitialState(&m_ActivationFinishedState);
    m_DeactivationMachine.setInitialState(&m_DeactivationFinishedState);
}

QByteArray ElectronicBurdenModule::getConfiguration() const
{
    return m_pConfiguration->exportConfiguration();
}

void ElectronicBurdenModule::doConfiguration(QByteArray xmlConfigData)
{
    m_pConfiguration->setConfiguration(xmlConfigData);
}

void ElectronicBurdenModule::setupModule()
{
    emit addEventSystem(m_pModuleValidator);
    cBaseModule::setupModule();

    m_pProgram = new ElectronicBurdenModuleProgram(this, m_pConfiguration);
    m_ModuleActivistList.append(m_pProgram);

    for (int i = 0; i < m_ModuleActivistList.count(); i++) {
        m_ModuleActivistList.at(i)->generateInterface();
    }
}

void ElectronicBurdenModule::startMeas()
{
    // nothing to start here
}

void ElectronicBurdenModule::stopMeas()
{
    // also nothing to stop
}

void ElectronicBurdenModule::activationFinished()
{
    // now we still have to export the json interface information, then we are ready
    m_pModuleValidator->setParameterHash(veinModuleParameterHash);
    exportMetaData();

    emit activationReady();
}


void ElectronicBurdenModule::deactivationFinished()
{
    emit deactivationReady();
}

}
