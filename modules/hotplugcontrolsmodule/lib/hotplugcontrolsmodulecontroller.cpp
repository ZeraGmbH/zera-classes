#include "hotplugcontrolsmodulecontroller.h"
#include "rpcreadlockstate.h"
#include <scpi.h>

namespace HOTPLUGCONTROLSMODULE {


HotplugControlsModuleController::HotplugControlsModuleController(cHotplugControlsModule *module) :
    cModuleActivist(module->getVeinModuleName()),
    m_module(module),
    m_pcbConnection(m_module->getNetworkConfig())
{
    connect(&m_activationTasks, &TaskTemplate::sigFinish, this, &HotplugControlsModuleController::onActivateContinue);
}

void HotplugControlsModuleController::activate()
{
    m_activationTasks.addSub(m_pcbConnection.createConnectionTask());
    m_activationTasks.start();
}

void HotplugControlsModuleController::deactivate()
{
    emit m_module->deactivationContinue();
}

void HotplugControlsModuleController::generateVeinInterface()
{
    VfRpcEventSystemSimplified *rpcEventSystem = m_module->getRpcEventSystem();

    std::shared_ptr<RPCReadLockState> rpcEmobReadLockState = std::make_shared<RPCReadLockState>(m_pcbConnection.getInterface(), rpcEventSystem, m_module->getEntityId());
    rpcEventSystem->addRpc(rpcEmobReadLockState);
    m_pEmobLockStateRpc = std::make_shared<VfModuleRpc>(rpcEmobReadLockState,
                                                        "EMOB plug lock state: 0:unknown 1:open 2:locking 3:locked 4:error");
    m_pEmobLockStateRpc->setRPCScpiInfo("EMOB",
                                        QString("EMLOCKSTATE"),
                                        SCPI::isQuery,
                                        rpcEmobReadLockState->getSignature());
    m_module->m_veinModuleRPCMap[rpcEmobReadLockState->getSignature()] = m_pEmobLockStateRpc; // for modules use
}

void HotplugControlsModuleController::onActivateContinue(bool ok)
{
    if(ok) {
        emit m_module->activationContinue();
    }
}

}
