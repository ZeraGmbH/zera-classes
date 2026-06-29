#include "dosagemodule.h"
#include "dosagemodulemeasprogram.h"
#include <errormessages.h>

namespace DOSAGEMODULE
{

cDosageModule::cDosageModule(const ModuleFactoryParam &moduleParam) :
    cBaseMeasModule(moduleParam),
    m_configuration(moduleParam.m_configXmlData)
{
    m_sModuleName = QString("%1%2").arg(BaseModuleName).arg(moduleParam.m_moduleNum);
    m_sModuleDescription = QString("This module provides electrical values to control a dosage");
    m_sSCPIModuleName = QString("%1%2").arg(BaseSCPIModuleName).arg(moduleParam.m_moduleNum);
}

cDosageModuleConfigData* cDosageModule::getConfigData()
{
    return m_configuration.getConfigData();
}

QByteArray cDosageModule::getConfigXml() const
{
    return m_configuration.exportConfiguration();
}

void cDosageModule::setupModule()
{
    emit addEventSystem(getValidatorEventSystem());

    cBaseMeasModule::setupModule();

    // we need some program that does the measuring on dsp
    m_pMeasProgram = new cDosageModuleMeasProgram(this);
    m_ModuleActivistList.append(m_pMeasProgram);
    connect(m_pMeasProgram, &cDosageModuleMeasProgram::activated, this, &cDosageModule::activationContinue);
    connect(m_pMeasProgram, &cDosageModuleMeasProgram::deactivated, this, &cDosageModule::deactivationContinue);
    for (int i = 0; i < m_ModuleActivistList.count(); i++)
        m_ModuleActivistList.at(i)->generateVeinInterface();

    m_pMeasProgram->setupMeasureProgram();
}

void cDosageModule::startMeas()
{
    m_pMeasProgram->start();
}

void cDosageModule::stopMeas()
{
    m_pMeasProgram->stop();
}

}
