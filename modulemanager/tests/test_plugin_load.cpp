#include "modulemanager.h"
#include "test_plugin_load.h"
#include "testmodulemanager.h"
#include <mocktcpnetworkfactory.h>
#include <timemachineobject.h>
#include <QTest>

QTEST_MAIN(test_plugin_load)

void test_plugin_load::initTestCase()
{
    ModuleManagerSetupFacade::registerMetaTypeStreamOperators();
    qputenv("QT_FATAL_CRITICALS", "1");
}

void test_plugin_load::loadModulePluginsInstalled()
{
    if(!QString(OE_INSTALL_ROOT).isEmpty()) {
        // * OE fails to load module plugins from install folder
        // * For Qt-Creator / CI we want to check if plugins can be found
        //   in install folder
        //
        // => Make this test conditional

        qInfo("Skipping test_plugin_load::loadModulePluginsInstalled in OE");
        return;
    }
    TestModuleManager::enableTests(); // we use production so need to take care of paths
    ZeraModules::ModuleManager modMan(nullptr, nullptr, VeinTcp::MockTcpNetworkFactory::create(), true);

    bool modulesFound = modMan.loadAllAvailableModulePlugins();
    QVERIFY(modulesFound);
}

void test_plugin_load::loadModulePluginsSources()
{
    TestModuleManager modMan(nullptr, nullptr);

    bool modulesFound = modMan.loadAllAvailableModulePlugins();
    QVERIFY(modulesFound);
}
