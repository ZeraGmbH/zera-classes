#include "power3module.h"
#include "power3moduleconfiguration.h"

namespace POWER3MODULE
{

cPower3Module::cPower3Module(ModuleFactoryParam moduleParam) :
    cBaseMeasModule(moduleParam, std::shared_ptr<BaseModuleConfiguration>(new cPower3ModuleConfiguration()))
{
    m_sModuleName = QString("%1%2").arg(BaseModuleName).arg(moduleParam.m_moduleNum);
    m_sModuleDescription = QString("This module measures configured number of harmonic power values from configured input values");
    m_sSCPIModuleName = QString("%1%2").arg(BaseSCPIModuleName).arg(moduleParam.m_moduleNum);
}

void cPower3Module::setupModule()
{
    emit addEventSystem(m_pModuleValidator);

    cBaseMeasModule::setupModule();

    // we need some program that does the measuring on dsp
    m_pMeasProgram = new cPower3ModuleMeasProgram(this, m_pConfiguration);
    m_ModuleActivistList.append(m_pMeasProgram);
    connect(m_pMeasProgram, &cPower3ModuleMeasProgram::activated, this, &cPower3Module::activationContinue);
    connect(m_pMeasProgram, &cPower3ModuleMeasProgram::deactivated, this, &cPower3Module::deactivationContinue);

    for (int i = 0; i < m_ModuleActivistList.count(); i++)
        m_ModuleActivistList.at(i)->generateVeinInterface();
}

void cPower3Module::startMeas()
{
    m_pMeasProgram->start();
}

void cPower3Module::stopMeas()
{
    m_pMeasProgram->stop();
}

}
