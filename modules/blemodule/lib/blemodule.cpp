#include "blemodule.h"

namespace BLEMODULE
{

cBleModule::cBleModule(const ModuleFactoryParam &moduleParam) :
    cBaseMeasModule(moduleParam),
    m_configuration(moduleParam.m_configXmlData)
{
    m_sModuleName = QString("%1%2").arg(BaseModuleName).arg(moduleParam.m_moduleNum);
    m_sModuleDescription = QString("This module measures configured number of harmonic power values from configured input values");
    m_sSCPIModuleName = QString("%1%2").arg(BaseSCPIModuleName).arg(moduleParam.m_moduleNum);
}

cBleModuleConfigData *cBleModule::getConfigData()
{
    return m_configuration.getConfigData();
}

QByteArray cBleModule::getConfigXml() const
{
    return m_configuration.exportConfiguration();
}

void cBleModule::setupModule()
{
    emit addEventSystem(getValidatorEventSystem());
    cBaseMeasModule::setupModule();

    m_pMeasProgram = new cBleModuleMeasProgram(this);
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
