#include "transformer1module.h"
#include "transformer1moduleconfiguration.h"

namespace TRANSFORMER1MODULE
{

cTransformer1Module::cTransformer1Module(ModuleFactoryParam moduleParam) :
    cBaseMeasModule(moduleParam, std::shared_ptr<BaseModuleConfiguration>(new cTransformer1ModuleConfiguration()))
{
    m_sModuleName = QString("%1%2").arg(BaseModuleName).arg(moduleParam.m_moduleNum);
    m_sModuleDescription = QString("This module measures configured number transformer errors from configured input dft values");
    m_sSCPIModuleName = QString("%1%2").arg(BaseSCPIModuleName).arg(moduleParam.m_moduleNum);
}

void cTransformer1Module::setupModule()
{
    emit addEventSystem(m_pModuleValidator);

    cBaseMeasModule::setupModule();

    // we need some program that does the measuring on dsp
    m_pMeasProgram = new cTransformer1ModuleMeasProgram(this, m_pConfiguration);
    m_ModuleActivistList.append(m_pMeasProgram);
    connect(m_pMeasProgram, &cTransformer1ModuleMeasProgram::activated, this, &cTransformer1Module::activationContinue);
    connect(m_pMeasProgram, &cTransformer1ModuleMeasProgram::deactivated, this, &cTransformer1Module::deactivationContinue);

    for (int i = 0; i < m_ModuleActivistList.count(); i++)
        m_ModuleActivistList.at(i)->generateVeinInterface();
}

void cTransformer1Module::startMeas()
{
    m_pMeasProgram->start();
}

void cTransformer1Module::stopMeas()
{
    m_pMeasProgram->stop();
}

}
