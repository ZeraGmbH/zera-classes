#include "transformer1module.h"

namespace TRANSFORMER1MODULE
{

cTransformer1Module::cTransformer1Module(const ModuleFactoryParam &moduleParam) :
    cBaseMeasModule(moduleParam),
    m_configuration(moduleParam.m_configXmlData)
{
    m_sModuleName = QString("%1%2").arg(BaseModuleName).arg(moduleParam.m_moduleNum);
    m_sModuleDescription = QString("This module measures configured number transformer errors from configured input dft values");
    m_sSCPIModuleName = QString("%1%2").arg(BaseSCPIModuleName).arg(moduleParam.m_moduleNum);
}

cTransformer1ModuleConfigData *cTransformer1Module::getConfigData()
{
    return m_configuration.getConfigData();
}

QByteArray cTransformer1Module::getConfigXml() const
{
    return m_configuration.exportConfiguration();
}

void cTransformer1Module::setupModule()
{
    emit addEventSystem(getValidatorEventSystem());

    cBaseMeasModule::setupModule();

    // we need some program that does the measuring on dsp
    m_pMeasProgram = new cTransformer1ModuleMeasProgram(this);
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
