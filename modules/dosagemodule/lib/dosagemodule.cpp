#include "dosagemodule.h"
#include "dosagemoduleconfiguration.h"
#include <errormessages.h>

namespace DOSAGEMODULE
{

cDosageModule::cDosageModule(ModuleFactoryParam moduleParam) : cBaseMeasModule(moduleParam, std::make_shared<cDosageModuleConfiguration>())
{
    m_sModuleName = QString("%1%2").arg(BaseModuleName).arg(moduleParam.m_moduleNum);
    m_sModuleDescription = QString("This module provides the amount of flanks seen by the FPGA");
    m_sSCPIModuleName = QString("%1%2").arg(BaseSCPIModuleName).arg(moduleParam.m_moduleNum);
}

void cDosageModule::setupModule()
{
    emit addEventSystem(getValidatorEventSystem());
    cBaseMeasModule::setupModule();

    // we need some program that does the measuring on dsp
    m_pMeasProgram = new cDosageModuleMeasProgram(this, m_pConfiguration);
    m_ModuleActivistList.append(m_pMeasProgram);
    connect(m_pMeasProgram, &cDosageModuleMeasProgram::activated, this, &cDosageModule::activationContinue);
    connect(m_pMeasProgram, &cDosageModuleMeasProgram::deactivated, this, &cDosageModule::deactivationContinue);

    for (int i = 0; i < m_ModuleActivistList.count(); i++)
        m_ModuleActivistList.at(i)->generateVeinInterface();
}

void cDosageModule::startMeas()
{
    m_pMeasProgram->start();
}

void cDosageModule::stopMeas()
{
    m_pMeasProgram->stop();
}

}
