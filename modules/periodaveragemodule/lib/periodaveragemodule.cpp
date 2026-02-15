#include "periodaveragemodule.h"
#include "periodaveragemoduleconfiguration.h"
#include "periodaveragemodulemeasprogram.h"

namespace PERIODAVERAGEMODULE
{

PeriodAverageModule::PeriodAverageModule(ModuleFactoryParam moduleParam) :
    cBaseMeasModule(moduleParam, std::make_shared<PeriodAverageModuleConfiguration>())
{
    m_sModuleName = QString("%1%2").arg(BaseModuleName).arg(moduleParam.m_moduleNum);
    m_sModuleDescription = QString("This module measures per period average (DC) values)");
    m_sSCPIModuleName = QString("%1%2").arg(BaseSCPIModuleName).arg(moduleParam.m_moduleNum);
}

void PeriodAverageModule::setupModule()
{
    emit addEventSystem(getValidatorEventSystem());
    cBaseMeasModule::setupModule();

    // we need some program that does the measuring on dsp
    m_pMeasProgram = new PeriodAverageModuleMeasProgram(this, m_pConfiguration);
    m_ModuleActivistList.append(m_pMeasProgram);
    connect(m_pMeasProgram, &PeriodAverageModuleMeasProgram::activated, this, &PeriodAverageModule::activationContinue);
    connect(m_pMeasProgram, &PeriodAverageModuleMeasProgram::deactivated, this, &PeriodAverageModule::deactivationContinue);

    for (int i = 0; i < m_ModuleActivistList.count(); i++)
        m_ModuleActivistList.at(i)->generateVeinInterface();
}

void PeriodAverageModule::startMeas()
{
    m_pMeasProgram->start();
}

void PeriodAverageModule::stopMeas()
{
    m_pMeasProgram->stop();
}

}
