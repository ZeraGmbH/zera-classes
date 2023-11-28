#include "test_modman_session.h"
#include "licensesystemmock.h"
#include "modulemanager.h"
#include "modulemanagertest.h"
#include "modulemanagerconfigtest.h"
#include "jsonsessionloadertest.h"
#include "vs_veinhash.h"
#include <QTest>

QTEST_MAIN(test_modman_session)

void test_modman_session::initTestCase()
{
    if(!QString(ZC_SERVICES_IP).isEmpty())
        qFatal("Running in demo mode and ZC_SERIVCES_IP is set to %s. ZC_SERIVCES_IP must be empty in demo mode!\n", ZC_SERVICES_IP);
    ModuleManagerTest::enableTest();
    ModuleManagerTest::pointToInstalledSessionFiles();
    qputenv("QT_FATAL_CRITICALS", "1"); \
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
    const QStringList emptyAvailableSessionList;
    ZeraModules::ModuleManager modMan(emptyAvailableSessionList, nullptr);
    modMan.setDemo(true);

    bool modulesFound = modMan.loadAllAvailableModulePlugins();
    QVERIFY(modulesFound);
}

////////////////////////////////////////////////////////////////////
// From here all plugins are loeaded from build folder for OE's sake
// Means: Our modMan is ModuleManagerTest

void test_modman_session::loadModulePluginsOE()
{
    const QStringList emptyAvailableSessionList;
    ModuleManagerTest modMan(emptyAvailableSessionList, nullptr);
    modMan.setDemo(true);

    bool modulesFound = modMan.loadAllAvailableModulePlugins();
    QVERIFY(modulesFound);
}

void test_modman_session::startSession()
{
    ModuleManagerSetupFacade modManSetupFacade;
    ModuleManagerController *mmController = modManSetupFacade.getModuleManagerController();
    VeinStorage::VeinHash storSystem;
    LicenseSystemMock licenseSystem;

    modManSetupFacade.addSubsystem(&storSystem);
    modManSetupFacade.addSubsystem(&licenseSystem);

    ModulemanagerConfig* mmConfig = ModulemanagerConfig::getInstance();
    const QStringList availableSessionList = mmConfig->getAvailableSessions();

    ModuleManagerTest modMan(availableSessionList, &modManSetupFacade);
    modMan.setDemo(true);
    QVERIFY(modMan.loadAllAvailableModulePlugins());
    modMan.setStorage(&storSystem);
    modMan.setLicenseSystem(&licenseSystem);
    mmController->setStorage(&storSystem);

    JsonSessionLoader sessionLoader;

    QObject::connect(&sessionLoader, &JsonSessionLoader::sigLoadModule, &modMan, &ZeraModules::ModuleManager::startModule);
    QObject::connect(&modMan, &ZeraModules::ModuleManager::sigSessionSwitched, &sessionLoader, &JsonSessionLoader::loadSession);

    QObject::connect(&modMan, &ZeraModules::ModuleManager::sigModulesLoaded, mmController, &ModuleManagerController::initializeEntity);
    QObject::connect(mmController, &ModuleManagerController::sigChangeSession, &modMan, &ZeraModules::ModuleManager::changeSessionFile);
    QObject::connect(mmController, &ModuleManagerController::sigModulesPausedChanged, &modMan, &ZeraModules::ModuleManager::setModulesPaused);

    const QString defaultSessionFile = mmConfig->getDefaultSession();
    modMan.changeSessionFile(defaultSessionFile);
    mmController->initOnce();

    ModuleManagerTest::feedEventLoop();
    QVERIFY(storSystem.hasEntity(0));
    QVERIFY(storSystem.hasStoredValue(0, "Session"));
    QString currentSession = storSystem.getStoredValue(0, "Session").toString();
    QCOMPARE(currentSession, "mt310s2-meas-session.json");

    QVERIFY(storSystem.hasStoredValue(0, "EntityName"));
    QString actDevIface = storSystem.getStoredValue(9999, "ACT_DEV_IFACE").toString();
    if(actDevIface.isEmpty()) // we have to make module resilient to this situation
        qFatal("ACT_DEV_IFACE empty - local modulemanager running???");

    modMan.destroyModules();
    do
        ModuleManagerTest::feedEventLoop();
    while(!modMan.areAllModulesShutdown());
}

void test_modman_session::changeSession()
{
    ModuleManagerSetupFacade modManSetupFacade;
    ModuleManagerController *mmController = modManSetupFacade.getModuleManagerController();
    VeinStorage::VeinHash storSystem;
    LicenseSystemMock licenseSystem;

    modManSetupFacade.addSubsystem(&storSystem);
    modManSetupFacade.addSubsystem(&licenseSystem);

    ModulemanagerConfig* mmConfig = ModulemanagerConfig::getInstance();
    const QStringList availableSessionList = mmConfig->getAvailableSessions();

    ModuleManagerTest modMan(availableSessionList, &modManSetupFacade);
    modMan.setDemo(true);
    modMan.loadAllAvailableModulePlugins();
    modMan.setStorage(&storSystem);
    modMan.setLicenseSystem(&licenseSystem);
    mmController->setStorage(&storSystem);

    JsonSessionLoader sessionLoader;

    QObject::connect(&sessionLoader, &JsonSessionLoader::sigLoadModule, &modMan, &ZeraModules::ModuleManager::startModule);
    QObject::connect(&modMan, &ZeraModules::ModuleManager::sigSessionSwitched, &sessionLoader, &JsonSessionLoader::loadSession);

    QObject::connect(&modMan, &ZeraModules::ModuleManager::sigModulesLoaded, mmController, &ModuleManagerController::initializeEntity);
    QObject::connect(mmController, &ModuleManagerController::sigChangeSession, &modMan, &ZeraModules::ModuleManager::changeSessionFile);
    QObject::connect(mmController, &ModuleManagerController::sigModulesPausedChanged, &modMan, &ZeraModules::ModuleManager::setModulesPaused);

    const QString defaultSessionFile = mmConfig->getDefaultSession();
    modMan.changeSessionFile(defaultSessionFile);
    mmController->initOnce();

    ModuleManagerTest::feedEventLoop();
    QString currentSession = storSystem.getStoredValue(0, "Session").toString();
    QCOMPARE(currentSession, "mt310s2-meas-session.json");

    modMan.changeSessionFile("mt310s2-emob-session.json");
    do
        ModuleManagerTest::feedEventLoop();
    while(!modMan.areAllModulesShutdown());
    currentSession = storSystem.getStoredValue(0, "Session").toString();
    QCOMPARE(currentSession, "mt310s2-emob-session.json");

    modMan.destroyModules();
    do
        ModuleManagerTest::feedEventLoop();
    while(!modMan.areAllModulesShutdown());
}
