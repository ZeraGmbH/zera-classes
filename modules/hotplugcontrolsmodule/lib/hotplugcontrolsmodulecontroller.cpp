#include "hotplugcontrolsmodulecontroller.h"

namespace HOTPLUGCONTROLSMODULE {


HotplugControlsModuleController::HotplugControlsModuleController(cHotplugControlsModule *module) :
    cModuleActivist(module->getVeinModuleName()),
    m_module(module)
{
}

void HotplugControlsModuleController::activate()
{
    emit m_module->activationContinue();
}

void HotplugControlsModuleController::deactivate()
{
    emit m_module->deactivationContinue();
}

void HotplugControlsModuleController::generateVeinInterface()
{
}

void HotplugControlsModuleController::onActivationReady()
{
    m_bActive = true;
    emit activated();
}

void HotplugControlsModuleController::onDeactivationReady()
{
    m_bActive = false;
    emit deactivated();
}

}
