#include "sfcmodule.h"
#include "sfcmoduleconfiguration.h"
#include <errormessages.h>

namespace SFCMODULE
{

cSfcModule::cSfcModule(ModuleFactoryParam moduleParam) : cBaseMeasModule(moduleParam, std::make_shared<cSfcModuleConfiguration>())
{
    m_sModuleName = QString("%1%2").arg(BaseModuleName).arg(moduleParam.m_moduleNum);
    m_sModuleDescription = QString("This module provides the amount of flanks seen by the FPGA");
    m_sSCPIModuleName = QString("%1%2").arg(BaseSCPIModuleName).arg(moduleParam.m_moduleNum);
}

void cSfcModule::setupModule()
{
    emit addEventSystem(getValidatorEventSystem());

    cBaseMeasModule::setupModule();

    // we need some program that does the measuring on dsp
    m_pMeasProgram = new cSfcModuleMeasProgram(this, m_pConfiguration);
    m_ModuleActivistList.append(m_pMeasProgram);
    connect(m_pMeasProgram, &cSfcModuleMeasProgram::activated, this, &cSfcModule::activationContinue);
    connect(m_pMeasProgram, &cSfcModuleMeasProgram::deactivated, this, &cSfcModule::deactivationContinue);

    for (int i = 0; i < m_ModuleActivistList.count(); i++)
        m_ModuleActivistList.at(i)->generateVeinInterface();
}

void cSfcModule::startMeas()
{
    m_pMeasProgram->start();
}

void cSfcModule::stopMeas()
{
    m_pMeasProgram->stop();
}

}
