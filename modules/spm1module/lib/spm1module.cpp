#include "spm1module.h"
#include "spm1moduleconfiguration.h"

namespace SPM1MODULE
{

cSpm1Module::cSpm1Module(ModuleFactoryParam moduleParam) :
    cBaseMeasModule(moduleParam, std::make_shared<cSpm1ModuleConfiguration>())
{
    m_sModuleName = QString("%1%2").arg(BaseModuleName).arg(moduleParam.m_moduleNum);
    m_sModuleDescription = QString("This module provides a configurable power error calculator");
    m_sSCPIModuleName = QString(BaseSCPIModuleName);
}

void cSpm1Module::setupModule()
{
    emit addEventSystem(getValidatorEventSystem());
    cBaseMeasModule::setupModule();

    // we only have this activist
    m_pMeasProgram = new cSpm1ModuleMeasProgram(this, m_pConfiguration);
    m_ModuleActivistList.append(m_pMeasProgram);
    connect(m_pMeasProgram, &cSpm1ModuleMeasProgram::activated, this, &cSpm1Module::activationContinue);
    connect(m_pMeasProgram, &cSpm1ModuleMeasProgram::deactivated, this, &cSpm1Module::deactivationContinue);

    for (int i = 0; i < m_ModuleActivistList.count(); i++)
        m_ModuleActivistList.at(i)->generateVeinInterface();
}

void cSpm1Module::startMeas()
{
    m_pMeasProgram->start();
}

void cSpm1Module::stopMeas()
{
    m_pMeasProgram->stop();
}

}
