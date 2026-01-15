#include "samplemodule.h"
#include "samplemoduleconfiguration.h"

namespace SAMPLEMODULE
{

cSampleModule::cSampleModule(ModuleFactoryParam moduleParam) :
    cBaseMeasModule(moduleParam, std::make_shared<cSampleModuleConfiguration>())
{
    m_sModuleName = QString("%1%2").arg(BaseModuleName).arg(moduleParam.m_moduleNum);
    m_sModuleDescription = QString("This module is responsible for pll range setting\n, pll channel selection and automatic");
    m_sSCPIModuleName = QString("%1%2").arg(BaseSCPIModuleName).arg(moduleParam.m_moduleNum);
}

void cSampleModule::setupModule()
{
    emit addEventSystem(getValidatorEventSystem());
    cBaseMeasModule::setupModule();

    m_pMeasProgram = new cSampleModuleMeasProgram(this, m_pConfiguration);
    m_ModuleActivistList.append(m_pMeasProgram);
    connect(m_pMeasProgram, &cSampleModuleMeasProgram::activated, this, &cSampleModule::activationContinue);
    connect(m_pMeasProgram, &cSampleModuleMeasProgram::deactivated, this, &cSampleModule::deactivationContinue);

    for (int i = 0; i < m_ModuleActivistList.count(); i++)
        m_ModuleActivistList.at(i)->generateVeinInterface();
}

void cSampleModule::activationFinished()
{
    getValidatorEventSystem()->setParameterMap(m_veinModuleParameterMap);
    exportMetaData();
    emit activationReady();
}

void cSampleModule::deactivationStart()
{
    m_nActivationIt = 0; // we start with the first
    emit deactivationContinue();
}

}
