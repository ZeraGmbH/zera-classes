#include "scpimodulefortest.h"
#include "scpimoduleconfiguration.h"
#include "scpiserver.h"

namespace SCPIMODULE {

ScpiModuleForTest::ScpiModuleForTest(ModuleFactoryParam moduleParam) :
    cSCPIModule(moduleParam)
{
}

cSCPIModuleConfigData *ScpiModuleForTest::getConfigData() const
{
    return qobject_cast<cSCPIModuleConfiguration*>(m_pConfiguration.get())->getConfigurationData();
}

cSCPIInterface *ScpiModuleForTest::getScpiInterface() const
{
    return m_pSCPIServer->getScpiInterface();
}

}
