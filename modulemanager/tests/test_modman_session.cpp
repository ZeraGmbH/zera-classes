#include "test_modman_session.h"
#include "licensesystemmock.h"
#include "modulemanager.h"
#include "modulemanagertest.h"
#include <timemachineobject.h>
#include <QTest>

QTEST_MAIN(test_modman_session)

void test_modman_session::initTestCase()
{
    ModuleManagerSetupFacade::registerMetaTypeStreamOperators();
    ModuleManagerTest::enableTest();
    ModuleManagerTest::pointToInstalledSessionFiles();
    qputenv("QT_FATAL_CRITICALS", "1");
}

void test_modman_session::loadModulePluginsInstalled()
{
    if(!QString(OE_INSTALL_ROOT).isEmpty()) {
        // * OE fails to load module plugins from install folder
        // * For Qt-Creator / CI we want to check if plugins can be found
        //   in install folder
        //
        // => Make this test conditional

        qInfo("Skipping test_modman_session::loadModulePluginsInstalled in OE");
        return;
    }
    ZeraModules::ModuleManager modMan(nullptr, true);

    bool modulesFound = modMan.loadAllAvailableModulePlugins();
    QVERIFY(modulesFound);
}

////////////////////////////////////////////////////////////////////
// From here all plugins are loeaded from build folder for OE's sake
// Means: Our modMan is ModuleManagerTest

void test_modman_session::loadModulePluginsOE()
{
    ModuleManagerTest modMan(nullptr, true);

    bool modulesFound = modMan.loadAllAvailableModulePlugins();
    QVERIFY(modulesFound);
}

void test_modman_session::startSession()
{
    LicenseSystemMock licenseSystem;
    ModuleManagerSetupFacade modManSetupFacade(&licenseSystem);

    ModuleManagerTest modMan(&modManSetupFacade, true);
    QVERIFY(modMan.loadAllAvailableModulePlugins());
    modMan.setupConnections();
    modMan.loadDefaultSession();
    modMan.setMockServices("mt310s2");
    TimeMachineObject::feedEventLoop();

    QVERIFY(modManSetupFacade.getStorageSystem()->hasEntity(0));
    QVERIFY(modManSetupFacade.getStorageSystem()->hasStoredValue(0, "Session"));
    QString currentSession = modManSetupFacade.getStorageSystem()->getStoredValue(0, "Session").toString();
    QCOMPARE(currentSession, "mt310s2-meas-session.json");

    QVERIFY(modManSetupFacade.getStorageSystem()->hasStoredValue(0, "EntityName"));
    QString actDevIface = modManSetupFacade.getStorageSystem()->getStoredValue(9999, "ACT_DEV_IFACE").toString();
    if(actDevIface.isEmpty()) // we have to make module resilient to this situation
        qFatal("ACT_DEV_IFACE empty - local modulemanager running???");

    modMan.destroyModulesAndWaitUntilAllShutdown();
}

void test_modman_session::changeSession()
{
    LicenseSystemMock licenseSystem;
    ModuleManagerSetupFacade modManSetupFacade(&licenseSystem);

    ModuleManagerTest modMan(&modManSetupFacade, true);
    modMan.loadAllAvailableModulePlugins();
    modMan.setupConnections();
    modMan.loadDefaultSession();
    modMan.setMockServices("mt310s2");
    TimeMachineObject::feedEventLoop();

    QString currentSession = modManSetupFacade.getStorageSystem()->getStoredValue(0, "Session").toString();
    QCOMPARE(currentSession, "mt310s2-meas-session.json");

    modMan.changeSessionFile("mt310s2-emob-session.json");
    modMan.destroyModulesAndWaitUntilAllShutdown();
    currentSession = modManSetupFacade.getStorageSystem()->getStoredValue(0, "Session").toString();
    QCOMPARE(currentSession, "mt310s2-emob-session.json");

    modMan.destroyModulesAndWaitUntilAllShutdown();
}
