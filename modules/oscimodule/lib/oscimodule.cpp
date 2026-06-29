#include "oscimodule.h"
#include "oscimoduleconfiguration.h"
#include "oscimodulemeasprogram.h"

namespace OSCIMODULE
{

cOsciModule::cOsciModule(const ModuleFactoryParam &moduleParam) :
    cBaseMeasModule(moduleParam),
    m_configuration(moduleParam.m_configXmlData)
{
    m_sModuleName = QString("%1%2").arg(BaseModuleName).arg(moduleParam.m_moduleNum);
    m_sModuleDescription = QString("This module measures oscillograms for configured channels");
    m_sSCPIModuleName = QString("%1%2").arg(BaseSCPIModuleName).arg(moduleParam.m_moduleNum);
}

cOsciModuleConfigData *cOsciModule::getConfigData()
{
    return m_configuration.getConfigData();
}

QByteArray cOsciModule::getConfigXml() const
{
    return m_configuration.exportConfiguration();
}

void cOsciModule::setupModule()
{
    emit addEventSystem(getValidatorEventSystem());
    cBaseMeasModule::setupModule();

    // we need some program that does the measuring on dsp
    m_pMeasProgram = new cOsciModuleMeasProgram(this);
    m_ModuleActivistList.append(m_pMeasProgram);
    connect(m_pMeasProgram, &cOsciModuleMeasProgram::activated, this, &cOsciModule::activationContinue);
    connect(m_pMeasProgram, &cOsciModuleMeasProgram::deactivated, this, &cOsciModule::deactivationContinue);

    for (int i = 0; i < m_ModuleActivistList.count(); i++)
        m_ModuleActivistList.at(i)->generateVeinInterface();
}

void cOsciModule::startMeas()
{
    m_pMeasProgram->start();
}

void cOsciModule::stopMeas()
{
    m_pMeasProgram->stop();
}

}
