#include "sem1module.h"

namespace SEM1MODULE
{

cSem1Module::cSem1Module(const ModuleFactoryParam &moduleParam) :
    cBaseMeasModule(moduleParam),
    m_configuration(moduleParam.m_configXmlData)
{
    m_sModuleName = QString("%1%2").arg(BaseModuleName).arg(moduleParam.m_moduleNum);
    m_sModuleDescription = QString("This module provides a configurable energy error calculator");
    m_sSCPIModuleName = QString(BaseSCPIModuleName);
}

cSem1ModuleConfigData *cSem1Module::getConfigData()
{
    return m_configuration.getConfigData();
}

QByteArray cSem1Module::getConfigXml() const
{
    return m_configuration.exportConfiguration();
}

void cSem1Module::setupModule()
{
    emit addEventSystem(getValidatorEventSystem());

    cBaseMeasModule::setupModule();

    // we only have this activist
    m_pMeasProgram = new cSem1ModuleMeasProgram(this);
    m_ModuleActivistList.append(m_pMeasProgram);
    connect(m_pMeasProgram, &cSem1ModuleMeasProgram::activated, this, &cSem1Module::activationContinue);
    connect(m_pMeasProgram, &cSem1ModuleMeasProgram::deactivated, this, &cSem1Module::deactivationContinue);

    for (int i = 0; i < m_ModuleActivistList.count(); i++)
        m_ModuleActivistList.at(i)->generateVeinInterface();
}

void cSem1Module::startMeas()
{
    m_pMeasProgram->start();
}

void cSem1Module::stopMeas()
{
    m_pMeasProgram->stop();
}

}
