#include "adjustmentmodule.h"
#include "adjustmentmoduleconfiguration.h"
#include "adjustmentmodulemeasprogram.h"

cAdjustmentModule::cAdjustmentModule(const ModuleFactoryParam &moduleParam) :
    cBaseMeasModule(moduleParam),
    m_configuration(moduleParam.m_configXmlData),
    m_spRpcEventSystem(std::make_unique<VfRpcEventSystemSimplified>(moduleParam.m_entityId))
{
    m_sModuleName = QString("%1%2").arg(BaseModuleName).arg(moduleParam.m_moduleNum);
    m_sModuleDescription = QString("This module supports commands for adjustment for a configured number of measuring channels");
    m_sSCPIModuleName = QString("%1%2").arg(BaseSCPIModuleName).arg(moduleParam.m_moduleNum);
}

cAdjustmentModuleConfigData *cAdjustmentModule::getConfigData()
{
    return m_configuration.getConfigData();
}

QByteArray cAdjustmentModule::getConfigXml() const
{
    return m_configuration.exportConfiguration();
}

VfRpcEventSystemSimplified *cAdjustmentModule::getRpcEventSystem()
{
    return m_spRpcEventSystem.get();
}

void cAdjustmentModule::activationFinished()
{
    getValidatorEventSystem()->setParameterMap(m_veinModuleParameterMap);
    getRpcEventSystem()->setRPCMap(m_veinModuleRPCMap);

    exportMetaData();
    emit activationReady();
}

void cAdjustmentModule::setupModule()
{
    emit addEventSystem(getValidatorEventSystem());
    emit addEventSystem(getRpcEventSystem());

    cBaseMeasModule::setupModule();

    // we need some program that does the job
    m_pMeasProgram = new cAdjustmentModuleMeasProgram(this);
    m_ModuleActivistList.append(m_pMeasProgram);
    connect(m_pMeasProgram, &cAdjustmentModuleMeasProgram::activated, this, &cAdjustmentModule::activationContinue);
    connect(m_pMeasProgram, &cAdjustmentModuleMeasProgram::deactivated, this, &cAdjustmentModule::deactivationContinue);

    for (int i = 0; i < m_ModuleActivistList.count(); i++)
        m_ModuleActivistList.at(i)->generateVeinInterface();
}

void cAdjustmentModule::startMeas()
{
    m_pMeasProgram->start();
}

void cAdjustmentModule::stopMeas()
{
    m_pMeasProgram->stop();
}
