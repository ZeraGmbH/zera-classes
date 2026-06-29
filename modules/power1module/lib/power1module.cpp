#include "power1module.h"

namespace POWER1MODULE
{

cPower1Module::cPower1Module(const ModuleFactoryParam &moduleParam) :
    cBaseMeasModule(moduleParam),
    m_configuration(moduleParam.m_configXmlData)
{
    m_sModuleName = QString("%1%2").arg(BaseModuleName).arg(moduleParam.m_moduleNum);
    m_sModuleDescription = QString("This module measures power with configured measuring and integration modes");
    m_sSCPIModuleName = QString("%1%2").arg(BaseSCPIModuleName).arg(moduleParam.m_moduleNum);
}

cPower1ModuleConfigData *cPower1Module::getConfigData()
{
    return m_configuration.getConfigData();
}

QByteArray cPower1Module::getConfigXml() const
{
    return m_configuration.exportConfiguration();
}

void cPower1Module::setupModule()
{
    emit addEventSystem(getValidatorEventSystem());
    cBaseMeasModule::setupModule();

    // we need some program that does the measuring on dsp
    m_pMeasProgram = new cPower1ModuleMeasProgram(this);
    m_ModuleActivistList.append(m_pMeasProgram);
    connect(m_pMeasProgram, &cModuleActivist::activated, this, &BaseModule::activationContinue);
    connect(m_pMeasProgram, &cModuleActivist::deactivated, this, &BaseModule::deactivationContinue);

    for (int i = 0; i < m_ModuleActivistList.count(); i++)
        m_ModuleActivistList.at(i)->generateVeinInterface();
}

void cPower1Module::startMeas()
{
    m_pMeasProgram->start();
}

void cPower1Module::stopMeas()
{
    m_pMeasProgram->stop();
}

}
