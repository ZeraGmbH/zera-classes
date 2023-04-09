#include "test_scpi_cmds_in_session.h"
#include "modulemanagerfortest.h"
#include <scpimodule.h>
#include <QTest>

QTEST_MAIN(test_scpi_cmds_in_session)

void test_scpi_cmds_in_session::initialSession()
{
    ModuleManagerForTest modman;

    SCPIMODULE::cSCPIModule scpiModule(1, 9999, modman.getStorageSystem());
    modman.addModule(&scpiModule, QStringLiteral(CONFIG_SOURCE_PATH) + "/" + "demo-scpimodule.xml");

}
