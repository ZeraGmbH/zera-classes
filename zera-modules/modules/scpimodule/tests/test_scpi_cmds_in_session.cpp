#include "test_scpi_cmds_in_session.h"
#include "modulemanagerfortest.h"
#include <scpimodule.h>
#include <QTest>

QTEST_MAIN(test_scpi_cmds_in_session)

void test_scpi_cmds_in_session::initialSession()
{   // First test on ModuleManagerForTest to know it works as expected
    ModuleManagerForTest modman;

    SCPIMODULE::cSCPIModule scpiModule(1, 9999, modman.getStorageSystem());
    modman.addModule(&scpiModule, QStringLiteral(CONFIG_SOURCE_PATH) + "/" + "demo-scpimodule.xml");

    VeinStorage::VeinHash* storageHash = modman.getStorageSystem();
    QList<int> entityList = storageHash->getEntityList();
    QCOMPARE(entityList.count(), 1);
    QList<QString> componentList = storageHash->getEntityComponents(entityList[0]);
    QCOMPARE(componentList.count(), 4); // EntitiyName / Metadata / PAR_SerialScpiActive / ACT_SerialScpiDeviceFile
}
