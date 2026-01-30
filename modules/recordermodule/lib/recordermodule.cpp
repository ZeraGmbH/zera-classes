#include "recordermodule.h"
#include "recordermoduleconfiguration.h"
#include "recordermoduleinit.h"

RecorderModule::RecorderModule(ModuleFactoryParam moduleParam) :
    cBaseMeasModule(moduleParam, std::make_shared<RecorderModuleConfiguration>()),
    m_spRpcEventSystem(std::make_unique<VfRpcEventSystemSimplified>(moduleParam.m_entityId))
{
    m_sModuleName = QString("%1%2").arg(BaseModuleName).arg(moduleParam.m_moduleNum);
    m_sModuleDescription = QString("Recorder module");
    m_sSCPIModuleName = QString("%1%2").arg(BaseSCPIModuleName).arg(moduleParam.m_moduleNum);
}

VfRpcEventSystemSimplified *RecorderModule::getRpcEventSystem()
{
    return m_spRpcEventSystem.get();
}

void RecorderModule::activationFinished()
{
    getValidatorEventSystem()->setParameterMap(m_veinModuleParameterMap);
    getRpcEventSystem()->setRPCMap(m_veinModuleRPCMap);

    exportMetaData();
    emit activationReady();
}

void RecorderModule::setupModule()
{
    emit addEventSystem(getValidatorEventSystem());
    emit addEventSystem(getRpcEventSystem());

    BaseModule::setupModule();

    RecorderModuleInit* moduleActivist = new RecorderModuleInit(this, m_pConfiguration);
    m_ModuleActivistList.append(moduleActivist);
    connect(moduleActivist, &RecorderModuleInit::activated, this, &RecorderModule::activationContinue);
    connect(moduleActivist, &RecorderModuleInit::deactivated, this, &RecorderModule::deactivationContinue);

    for (int i = 0; i < m_ModuleActivistList.count(); i++)
        m_ModuleActivistList.at(i)->generateVeinInterface();
    emit activationContinue();
}

void RecorderModule::startMeas()
{
    // nothing to start here
}

void RecorderModule::stopMeas()
{
    // nothing to stop
}
