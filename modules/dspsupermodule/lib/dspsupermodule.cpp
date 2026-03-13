#include "dspsupermodule.h"
#include "dspsupermoduleconfiguration.h"
#include "dspsupermodulemeasprogram.h"

namespace DSPSUPERMODULE
{

DspSuperModule::DspSuperModule(const ModuleFactoryParam &moduleParam) :
    cBaseMeasModule(moduleParam, std::make_shared<DspSuperModuleConfiguration>())
{
    m_sModuleName = QString("%1%2").arg(BaseModuleName).arg(moduleParam.m_moduleNum);
    m_sModuleDescription = QString("DSP support on timestamps for other modules");
    m_sSCPIModuleName = QString("%1%2").arg(BaseSCPIModuleName).arg(moduleParam.m_moduleNum);
}

void DspSuperModule::setupModule()
{
    emit addEventSystem(getValidatorEventSystem());
    cBaseMeasModule::setupModule();

    // we need some program that does the measuring on dsp
    m_pMeasProgram = new DspSuperModuleMeasProgram(this, m_pConfiguration);
    m_ModuleActivistList.append(m_pMeasProgram);
    connect(m_pMeasProgram, &DspSuperModuleMeasProgram::activated, this, &DspSuperModule::activationContinue);
    connect(m_pMeasProgram, &DspSuperModuleMeasProgram::deactivated, this, &DspSuperModule::deactivationContinue);

    for (int i = 0; i < m_ModuleActivistList.count(); i++)
        m_ModuleActivistList.at(i)->generateVeinInterface();
}

void DspSuperModule::startMeas()
{
    m_pMeasProgram->start();
}

void DspSuperModule::stopMeas()
{
    m_pMeasProgram->stop();
}

}
