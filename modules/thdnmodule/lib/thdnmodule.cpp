#include "thdnmodule.h"
#include "thdnshadowmodulemeasprogram.h"
#include "thdnmoduleconfiguration.h"
#include "thdnmodulemeasprogram.h"

namespace THDNMODULE
{

cThdnModule::cThdnModule(const ModuleFactoryParam &moduleParam) :
    cBaseMeasModule(moduleParam),
    m_configuration(moduleParam.m_configXmlData)
{
    m_sModuleName = QString("%1%2").arg(BaseModuleName).arg(moduleParam.m_moduleNum);
    m_sModuleDescription = QString("This module measures thdn values for configured channels");
    m_sSCPIModuleName = QString("%1%2").arg(BaseSCPIModuleName).arg(moduleParam.m_moduleNum);
}

cThdnModuleConfigData *cThdnModule::getConfigData()
{
    return m_configuration.getConfigData();
}

QByteArray cThdnModule::getConfigXml() const
{
    return m_configuration.exportConfiguration();
}

void cThdnModule::setupModule()
{
    emit addEventSystem(getValidatorEventSystem());
    cBaseMeasModule::setupModule();

    if (m_configuration.getConfigData()->m_thdrSourceEntity == 0)
        m_pMeasProgram = new cThdnModuleMeasProgram(this);
    else
        m_pMeasProgram = new cThdnShadowModuleMeasProgram(this);
    m_ModuleActivistList.append(m_pMeasProgram);

    connect(m_pMeasProgram, &cModuleActivist::activated, this, &BaseModule::activationContinue);
    connect(m_pMeasProgram, &cModuleActivist::deactivated, this, &BaseModule::deactivationContinue);

    for (int i = 0; i < m_ModuleActivistList.count(); i++)
        m_ModuleActivistList.at(i)->generateVeinInterface();
}

void cThdnModule::startMeas()
{
    m_pMeasProgram->start();
}

void cThdnModule::stopMeas()
{
    m_pMeasProgram->stop();
}

}
