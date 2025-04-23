#include "blemodule.h"
#include "blemoduleconfiguration.h"

namespace BLEMODULE
{

cBleModule::cBleModule(ModuleFactoryParam moduleParam) :
    cBaseMeasModule(moduleParam, std::make_shared<cBleModuleConfiguration>())
{
    m_sModuleName = QString("%1%2").arg(BaseModuleName).arg(moduleParam.m_moduleNum);
    m_sModuleDescription = QString("This module measures configured number of harmonic power values from configured input values");
    m_sSCPIModuleName = QString("%1%2").arg(BaseSCPIModuleName).arg(moduleParam.m_moduleNum);
}

void cBleModule::setupModule()
{
    emit addEventSystem(getValidatorEventSystem());
    cBaseMeasModule::setupModule();

    m_pMeasProgram = new cBleModuleMeasProgram(this, m_pConfiguration);
    m_ModuleActivistList.append(m_pMeasProgram);
    connect(m_pMeasProgram, &cBleModuleMeasProgram::activated, this, &cBleModule::activationContinue);
    connect(m_pMeasProgram, &cBleModuleMeasProgram::deactivated, this, &cBleModule::deactivationContinue);

    for (int i = 0; i < m_ModuleActivistList.count(); i++)
        m_ModuleActivistList.at(i)->generateVeinInterface();
}

void cBleModule::startMeas()
{
    m_pMeasProgram->start();
}

void cBleModule::stopMeas()
{
    m_pMeasProgram->stop();
}

}
