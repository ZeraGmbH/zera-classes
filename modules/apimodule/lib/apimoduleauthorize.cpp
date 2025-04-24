#include "apimoduleauthorize.h"

namespace APIMODULE
{
cApiModuleAuthorize::cApiModuleAuthorize(cApiModule *module): cModuleActivist(module->getVeinModuleName()), m_module(module) {}

void cApiModuleAuthorize::generateVeinInterface()
{

}

void cApiModuleAuthorize::activate()
{
    emit m_module->activationContinue();
}

void cApiModuleAuthorize::deactivate()
{
    emit m_module->deactivationContinue();
}
}
