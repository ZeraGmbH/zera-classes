#include "scpimodulefortest.h"
#include "scpimoduleconfiguration.h"
#include "scpiserver.h"

namespace SCPIMODULE {

ScpiModuleForTest::ScpiModuleForTest(quint8 modnr, int entityId, VeinEvent::StorageSystem *storagesystem, QObject *parent) :
    cSCPIModule(modnr, entityId, storagesystem, parent)
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
