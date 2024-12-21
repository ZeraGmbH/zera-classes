#include "sourcemodule.h"
#include "moduleconfigurationnull.h"

SourceModule::SourceModule(ModuleFactoryParam moduleParam) :
    cBaseMeasModule(moduleParam, std::make_shared<ModuleConfigurationNull>())
{
    m_rpcEventSystem = new VfModuleRpc(moduleParam.m_entityId);
    m_sModuleName = QString("%1%2").arg(BaseModuleName).arg(moduleParam.m_moduleNum);
    m_sModuleDescription = QString("Module to access voltage and current sources");
    m_sSCPIModuleName = QString("%1%2").arg(BaseSCPIModuleName).arg(moduleParam.m_moduleNum);

    m_ActivationMachine.setInitialState(&m_ActivationFinishedState);
    Q_INIT_RESOURCE(resource);
}

VfModuleRpc *SourceModule::getRpcEventSystem() const
{
    return m_rpcEventSystem;
}

void SourceModule::setupModule()
{
    emit addEventSystem(m_pModuleValidator);
    emit addEventSystem(m_rpcEventSystem);
    BaseModule::setupModule();

    m_pProgram = new SourceModuleProgram(this, m_pConfiguration);
    m_ModuleActivistList.append(m_pProgram);

    m_DeactivationMachine.addState(&m_stateSwitchAllOff);
    connect(&m_stateSwitchAllOff, &QState::entered, this, [&]() {
        m_pProgram->startDestroy();
    });
    m_stateSwitchAllOff.addTransition(m_pProgram, &SourceModuleProgram::sigLastSourceRemoved, &m_DeactivationFinishedState);
    m_DeactivationMachine.setInitialState(&m_stateSwitchAllOff);

    for (int i = 0; i < m_ModuleActivistList.count(); i++) {
        m_ModuleActivistList.at(i)->generateVeinInterface();
    }
}

void SourceModule::startMeas()
{
    // nothing to start here
}

void SourceModule::stopMeas()
{
    // also nothing to stop
}
