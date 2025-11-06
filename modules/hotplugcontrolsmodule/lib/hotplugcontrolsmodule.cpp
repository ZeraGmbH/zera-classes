#include "hotplugcontrolsmodule.h"
#include "hotplugcontrolsmoduleconfiguration.h"
#include "hotplugcontrolsmodulecontroller.h"

namespace HOTPLUGCONTROLSMODULE
{

cHotplugControlsModule::cHotplugControlsModule(ModuleFactoryParam moduleParam) :
    BaseModule(moduleParam, std::make_shared<cHotplugControlsModuleConfiguration>()),
    m_spModuleValidator(std::make_shared<VfEventSytemModuleParam>(moduleParam.m_entityId, moduleParam.m_moduleSharedData->m_storagesystem))
{
    m_sModuleName = QString("%1%2").arg(BaseModuleName).arg(moduleParam.m_moduleNum);
    m_sModuleDescription = QString("EMOB module");
    m_sSCPIModuleName = QString("%1%2").arg(BaseSCPIModuleName).arg(moduleParam.m_moduleNum);
    m_pModuleEventSystem = new VfEventSytemModuleParam(moduleParam.m_entityId, moduleParam.m_moduleSharedData->m_storagesystem);
}

void cHotplugControlsModule::activationFinished()
{
    getValidatorEventSystem()->setParameterMap(m_veinModuleParameterMap);

    exportMetaData();
    emit activationReady();
}

VfEventSytemModuleParam *cHotplugControlsModule::getValidatorEventSystem()
{
    return m_spModuleValidator.get();
}

void cHotplugControlsModule::setupModule()
{
    emit addEventSystem(getValidatorEventSystem());
    BaseModule::setupModule();
    emit addEventSystem(m_pModuleEventSystem);

    HotplugControlsModuleController* moduleActivist = new HotplugControlsModuleController(this);
    m_ModuleActivistList.append(moduleActivist);

    for (int i = 0; i < m_ModuleActivistList.count(); i++)
        m_ModuleActivistList.at(i)->generateVeinInterface();
    emit activationContinue();
}

void cHotplugControlsModule::startMeas()
{
    // nothing to start here
}

void cHotplugControlsModule::stopMeas()
{
    // also nothing to stop
}

}
