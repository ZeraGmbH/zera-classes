#include "statusmodule.h"
#include "statusmoduleconfiguration.h"

namespace STATUSMODULE
{

cStatusModule::cStatusModule(ModuleFactoryParam moduleParam) :
    cBaseMeasModule(moduleParam, std::shared_ptr<BaseModuleConfiguration>(new cStatusModuleConfiguration()))
{
    m_sModuleName = QString("%1%2").arg(BaseModuleName).arg(moduleParam.m_moduleNum);
    m_sModuleDescription = QString("This module is responsible for reading and providing system Status information");
    m_sSCPIModuleName = QString("%1%2").arg(BaseSCPIModuleName).arg(moduleParam.m_moduleNum);
}

void cStatusModule::setupModule()
{
    emit addEventSystem(m_pModuleValidator);
    cBaseMeasModule::setupModule();

    cStatusModuleConfigData *pConfData;
    pConfData = qobject_cast<cStatusModuleConfiguration*>(m_pConfiguration.get())->getConfigurationData();

    // we only have to read some status information from pcb- and dspserver
    m_pStatusModuleInit = new cStatusModuleInit(this, *pConfData);
    m_ModuleActivistList.append(m_pStatusModuleInit);
    connect(m_pStatusModuleInit, &cStatusModuleInit::activated, this, &cStatusModule::activationContinue);
    connect(m_pStatusModuleInit, &cStatusModuleInit::deactivated, this, &cStatusModule::deactivationContinue);

    for (int i = 0; i < m_ModuleActivistList.count(); i++)
        m_ModuleActivistList.at(i)->generateVeinInterface();
}

void cStatusModule::startMeas()
{
    // nothing to start here
}

void cStatusModule::stopMeas()
{
    // also nothing to stop
}

}
