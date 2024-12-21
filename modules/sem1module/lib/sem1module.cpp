#include "sem1module.h"
#include "sem1moduleconfiguration.h"
#include "sem1modulemeasprogram.h"
#include <vfmodulecomponent.h>
#include <vfmodulemetadata.h>

namespace SEM1MODULE
{

cSem1Module::cSem1Module(ModuleFactoryParam moduleParam) :
    cBaseMeasModule(moduleParam, std::shared_ptr<BaseModuleConfiguration>(new cSem1ModuleConfiguration()))
{
    m_sModuleName = QString("%1%2").arg(BaseModuleName).arg(moduleParam.m_moduleNum);
    m_sModuleDescription = QString("This module povides a configurable error calculator");
    m_sSCPIModuleName = QString(BaseSCPIModuleName);
}

void cSem1Module::setupModule()
{
    emit addEventSystem(m_pModuleValidator);
    cBaseMeasModule::setupModule();

    // we only have this activist
    m_pMeasProgram = new cSem1ModuleMeasProgram(this, m_pConfiguration);
    m_ModuleActivistList.append(m_pMeasProgram);
    connect(m_pMeasProgram, &cSem1ModuleMeasProgram::activated, this, &cSem1Module::activationContinue);
    connect(m_pMeasProgram, &cSem1ModuleMeasProgram::deactivated, this, &cSem1Module::deactivationContinue);

    for (int i = 0; i < m_ModuleActivistList.count(); i++)
        m_ModuleActivistList.at(i)->generateVeinInterface();
}

void cSem1Module::startMeas()
{
    m_pMeasProgram->start();
}

void cSem1Module::stopMeas()
{
    m_pMeasProgram->stop();
}

}
