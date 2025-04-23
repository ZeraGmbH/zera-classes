#include "burden1module.h"
#include "burden1moduleconfiguration.h"
#include "burden1modulemeasprogram.h"

namespace BURDEN1MODULE
{

cBurden1Module::cBurden1Module(ModuleFactoryParam moduleParam) :
    cBaseMeasModule(moduleParam, std::make_shared<cBurden1ModuleConfiguration>())
{
    m_sModuleName = QString("%1%2").arg(BaseModuleName).arg(moduleParam.m_moduleNum);
    m_sModuleDescription = QString("This module measures configured number burden and powerfactor from configured input dft values");
    m_sSCPIModuleName = QString("%1%2").arg(BaseSCPIModuleName).arg(moduleParam.m_moduleNum);
}

void cBurden1Module::setupModule()
{
    emit addEventSystem(getValidatorEventSystem());

    cBaseMeasModule::setupModule();

    // we need some program that does the measuring on dsp
    m_pMeasProgram = new cBurden1ModuleMeasProgram(this, m_pConfiguration);
    m_ModuleActivistList.append(m_pMeasProgram);
    connect(m_pMeasProgram, &cBurden1ModuleMeasProgram::activated, this, &cBurden1Module::activationContinue);
    connect(m_pMeasProgram, &cBurden1ModuleMeasProgram::deactivated, this, &cBurden1Module::deactivationContinue);

    for (int i = 0; i < m_ModuleActivistList.count(); i++)
        m_ModuleActivistList.at(i)->generateVeinInterface();
}

void cBurden1Module::startMeas()
{
    m_pMeasProgram->start();
}

void cBurden1Module::stopMeas()
{
    m_pMeasProgram->stop();
}

}
