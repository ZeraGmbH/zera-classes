#include "modemodule.h"
#include "modemoduleconfiguration.h"

namespace MODEMODULE
{

cModeModule::cModeModule(ModuleFactoryParam moduleParam) :
    cBaseMeasModule(moduleParam, std::make_shared<cModeModuleConfiguration>())
{
    m_sModuleName = QString("%1%2").arg(BaseModuleName).arg(moduleParam.m_moduleNum);
    m_sModuleDescription = QString("This module is responsible for setting measuring mode and resetting dsp adjustment data");
    m_sSCPIModuleName = QString("%1%2").arg(BaseSCPIModuleName).arg(moduleParam.m_moduleNum);
}

void cModeModule::setupModule()
{
    emit addEventSystem(getValidatorEventSystem());
    cBaseMeasModule::setupModule();

    cModeModuleConfigData *pConfData;
    pConfData = qobject_cast<cModeModuleConfiguration*>(m_pConfiguration.get())->getConfigurationData();

    // we only have to initialize the pcb's measuring mode
    m_pModeModuleInit = new cModeModuleInit(this, *pConfData);
    m_ModuleActivistList.append(m_pModeModuleInit);
    connect(m_pModeModuleInit, &cModeModuleInit::activated, this, &cModeModule::activationContinue);
    connect(m_pModeModuleInit, &cModeModuleInit::deactivated, this, &cModeModule::deactivationContinue);

    for (int i = 0; i < m_ModuleActivistList.count(); i++)
        m_ModuleActivistList.at(i)->generateVeinInterface();
}

void cModeModule::startMeas()
{
    // nothing to start here
}

void cModeModule::stopMeas()
{
    // also nothing to stop
}

void cModeModule::activationFinished()
{
    // now we still have to export the json interface information, then we are ready
    exportMetaData();
    emit activationReady();
}

}
