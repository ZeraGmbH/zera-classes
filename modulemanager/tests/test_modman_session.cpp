#include "test_modman_session.h"
#include "jsonsessionloadertest.h"
#include "licensesystemmock.h"
#include "modulemanager.h"
#include "modulemanagerconfigtest.h"
#include "modulemanagertest.h"
#include "vs_veinhash.h"
#include <QDataStream>
#include <QTest>

QTEST_MAIN(test_modman_session)

void test_modman_session::initTestCase()
{
    if(!QString(ZC_SERVICES_IP).isEmpty())
        qFatal("Running in demo mode and ZC_SERIVCES_IP is set to %s. ZC_SERIVCES_IP must be empty in demo mode!\n", ZC_SERVICES_IP);
    ModuleManagerTest::enableTest();
    ModuleManagerTest::pointToInstalledSessionFiles();
    qRegisterMetaTypeStreamOperators<QList<int> >("QList<int>");
    qRegisterMetaTypeStreamOperators<QList<float> >("QList<float>");
    qRegisterMetaTypeStreamOperators<QList<double> >("QList<double>");
    qRegisterMetaTypeStreamOperators<QList<QString> >("QList<QString>");
    qRegisterMetaTypeStreamOperators<QVector<QString> >("QVector<QString>");
    qRegisterMetaTypeStreamOperators<QList<QVariantMap> >("QList<QVariantMap>");
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
    LicenseSystemMock licenseSystem;
    ModuleManagerSetupFacade modManSetupFacade(&licenseSystem);
    ModuleManagerController *mmController = modManSetupFacade.getModuleManagerController();

    ModulemanagerConfig* mmConfig = ModulemanagerConfig::getInstance();
    const QStringList availableSessionList = mmConfig->getAvailableSessions();

    ModuleManagerTest modMan(availableSessionList, &modManSetupFacade);
    modMan.setDemo(true);
    QVERIFY(modMan.loadAllAvailableModulePlugins());
    modMan.setLicenseSystem();

    JsonSessionLoader sessionLoader;

    QObject::connect(&sessionLoader, &JsonSessionLoader::sigLoadModule, &modMan, &ZeraModules::ModuleManager::startModule);
    QObject::connect(&modMan, &ZeraModules::ModuleManager::sigSessionSwitched, &sessionLoader, &JsonSessionLoader::loadSession);
    modMan.setModuleManagerControllerConnections();

    const QString defaultSessionFile = mmConfig->getDefaultSession();
    modMan.changeSessionFile(defaultSessionFile);
    mmController->initOnce();

    ModuleManagerTest::feedEventLoop();
    QVERIFY(modManSetupFacade.getStorageSystem()->hasEntity(0));
    QVERIFY(modManSetupFacade.getStorageSystem()->hasStoredValue(0, "Session"));
    QString currentSession = modManSetupFacade.getStorageSystem()->getStoredValue(0, "Session").toString();
    QCOMPARE(currentSession, "mt310s2-meas-session.json");

    QVERIFY(modManSetupFacade.getStorageSystem()->hasStoredValue(0, "EntityName"));
    QString actDevIface = modManSetupFacade.getStorageSystem()->getStoredValue(9999, "ACT_DEV_IFACE").toString();
    if(actDevIface.isEmpty()) // we have to make module resilient to this situation
        qFatal("ACT_DEV_IFACE empty - local modulemanager running???");

    modMan.destroyModules();
    do
        ModuleManagerTest::feedEventLoop();
    while(!modMan.areAllModulesShutdown());
}

void test_modman_session::changeSession()
{
    LicenseSystemMock licenseSystem;
    ModuleManagerSetupFacade modManSetupFacade(&licenseSystem);
    ModuleManagerController *mmController = modManSetupFacade.getModuleManagerController();

    ModulemanagerConfig* mmConfig = ModulemanagerConfig::getInstance();
    const QStringList availableSessionList = mmConfig->getAvailableSessions();

    ModuleManagerTest modMan(availableSessionList, &modManSetupFacade);
    modMan.setDemo(true);
    modMan.loadAllAvailableModulePlugins();
    modMan.setLicenseSystem();

    JsonSessionLoader sessionLoader;

    QObject::connect(&sessionLoader, &JsonSessionLoader::sigLoadModule, &modMan, &ZeraModules::ModuleManager::startModule);
    QObject::connect(&modMan, &ZeraModules::ModuleManager::sigSessionSwitched, &sessionLoader, &JsonSessionLoader::loadSession);
    modMan.setModuleManagerControllerConnections();

    const QString defaultSessionFile = mmConfig->getDefaultSession();
    modMan.changeSessionFile(defaultSessionFile);
    mmController->initOnce();

    ModuleManagerTest::feedEventLoop();
    QString currentSession = modManSetupFacade.getStorageSystem()->getStoredValue(0, "Session").toString();
    QCOMPARE(currentSession, "mt310s2-meas-session.json");

    modMan.changeSessionFile("mt310s2-emob-session.json");
    do
        ModuleManagerTest::feedEventLoop();
    while(!modMan.areAllModulesShutdown());
    currentSession = modManSetupFacade.getStorageSystem()->getStoredValue(0, "Session").toString();
    QCOMPARE(currentSession, "mt310s2-emob-session.json");

    modMan.destroyModules();
    do
        ModuleManagerTest::feedEventLoop();
    while(!modMan.areAllModulesShutdown());
}
