#include "semspmmodule.h"

SemSpmModule::SemSpmModule(const ModuleFactoryParam &moduleParam,
                           const QString &moduleDescription,
                           const QString &moduleName,
                           const QString &scpiModuleName) :
    cBaseMeasModule(moduleParam),
    m_configuration(moduleParam.m_configXmlData)
{
    m_sModuleName = QString("%1%2").arg(moduleName).arg(moduleParam.m_moduleNum);
    m_sModuleDescription = moduleDescription;
    m_sSCPIModuleName = scpiModuleName;
}

SemSpmModuleConfigData *SemSpmModule::getConfigData()
{
    return m_configuration.getConfigData();
}

QByteArray SemSpmModule::getConfigXml() const
{
    return m_configuration.exportConfiguration();
}

void SemSpmModule::startMeas()
{
    m_pMeasProgram->start();
}

void SemSpmModule::stopMeas()
{
    m_pMeasProgram->stop();
}

