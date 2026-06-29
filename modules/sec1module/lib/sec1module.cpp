#include "sec1module.h"
#include "sec1modulemeasprogram.h"

namespace SEC1MODULE
{

cSec1Module::cSec1Module(const ModuleFactoryParam &moduleParam) :
    cBaseMeasModule(moduleParam),
    m_configuration(moduleParam.m_configXmlData)
{
    m_sModuleName = QString("%1%2").arg(BaseModuleName).arg(moduleParam.m_moduleNum);
    m_sModuleDescription = QString("This module provides a configurable error calculator");
    m_sSCPIModuleName = QString(BaseSCPIModuleName);
}

cSec1ModuleConfigData *cSec1Module::getConfigData()
{
    return m_configuration.getConfigData();
}

QByteArray cSec1Module::getConfigXml() const
{
    return m_configuration.exportConfiguration();
}

void cSec1Module::setupModule()
{
    emit addEventSystem(getValidatorEventSystem());
    cBaseMeasModule::setupModule();

    // we only have this activist
    m_pMeasProgram = new cSec1ModuleMeasProgram(this);
    m_ModuleActivistList.append(m_pMeasProgram);
    connect(m_pMeasProgram, &cSec1ModuleMeasProgram::activated, this, &cSec1Module::activationContinue);
    connect(m_pMeasProgram, &cSec1ModuleMeasProgram::deactivated, this, &cSec1Module::deactivationContinue);

    for (int i = 0; i < m_ModuleActivistList.count(); i++)
        m_ModuleActivistList.at(i)->generateVeinInterface();
}

void cSec1Module::startMeas()
{
    m_pMeasProgram->start();
}

void cSec1Module::stopMeas()
{
    m_pMeasProgram->stop();
}

}
