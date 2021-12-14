#include <veinmoduleactvalue.h>
#include <veinmoduleparameter.h>
#include <modulevalidator.h>
#include <intvalidator.h>
#include <jsonparamvalidator.h>
#include <vf-cpp-rpc.h>

#include "basedspmeasprogram.h"
#include "electronicburdenmoduleprogram.h"
#include "electronicburdenmodule.h"

namespace ELECTRONICBURDENMODULE
{

ElectronicBurdenModuleProgram::ElectronicBurdenModuleProgram(ElectronicBurdenModule* module, std::shared_ptr<cBaseModuleConfiguration> pConfiguration) :
    cBaseMeasWorkProgram(pConfiguration),
    m_pModule(module)
{
}

ElectronicBurdenModuleProgram::~ElectronicBurdenModuleProgram()
{
}

void ElectronicBurdenModuleProgram::generateInterface()
{
}

configuration *ElectronicBurdenModuleProgram::getConfigXMLWrapper()
{
    return static_cast<ElectronicBurdenModuleConfiguration*>(m_pConfiguration.get())->getConfigXMLWrapper();
}

}
