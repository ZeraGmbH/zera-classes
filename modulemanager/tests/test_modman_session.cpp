#include "test_modman_session.h"
#include "jsonsessionloadertest.h"
#include "licensesystemmock.h"
#include "moduleeventhandler.h"
#include "modulemanager.h"
#include "modulemanagerconfigtest.h"
#include "modulemanagertest.h"
#include "modulemanagercontroller.h"
#include "vn_introspectionsystem.h"
#include "vs_veinhash.h"
#include <QDataStream>
#include <QTest>

QTEST_MAIN(test_modman_session)

void test_modman_session::initTestCase()
{
    JsonSessionLoaderTest::enableTests();
    ModulemanagerConfigTest::enableTest();
    ModuleManagerTest::enableTest();
    ModulemanagerConfig::setDemoDevice("demo");
    qRegisterMetaTypeStreamOperators<QList<int> >("QList<int>");
    qRegisterMetaTypeStreamOperators<QList<float> >("QList<float>");
    qRegisterMetaTypeStreamOperators<QList<double> >("QList<double>");
    qRegisterMetaTypeStreamOperators<QList<QString> >("QList<QString>");
    qRegisterMetaTypeStreamOperators<QVector<QString> >("QVector<QString>");
    qRegisterMetaTypeStreamOperators<QList<QVariantMap> >("QList<QVariantMap>");
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
    ZeraModules::ModuleManager modMan(emptyAvailableSessionList);
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
    ModuleManagerTest modMan(emptyAvailableSessionList);
    modMan.setDemo(true);

    bool modulesFound = modMan.loadAllAvailableModulePlugins();
    QVERIFY(modulesFound);
}

void test_modman_session::startSession()
{
    ModuleEventHandler evHandler;
    ModuleManagerController mmController;
    VeinNet::IntrospectionSystem introspectionSystem;
    VeinStorage::VeinHash storSystem;
    LicenseSystemMock licenseSystem;

    QList<VeinEvent::EventSystem*> subSystems;
    subSystems.append(&mmController);
    subSystems.append(&introspectionSystem);
    subSystems.append(&storSystem);
    subSystems.append(&licenseSystem);
    evHandler.setSubsystems(subSystems);

    ModulemanagerConfig* mmConfig = ModulemanagerConfig::getInstance();
    const QStringList availableSessionList = mmConfig->getAvailableSessions();

    ModuleManagerTest modMan(availableSessionList);
    modMan.setDemo(true);
    modMan.setStorage(&storSystem);
    modMan.setLicenseSystem(&licenseSystem);
    modMan.setEventHandler(&evHandler);
    mmController.setStorage(&storSystem);

    JsonSessionLoader sessionLoader;
    QObject::connect(&sessionLoader, &JsonSessionLoader::sigLoadModule, &modMan, &ZeraModules::ModuleManager::startModule);
    QObject::connect(&modMan, &ZeraModules::ModuleManager::sigSessionSwitched, &sessionLoader, &JsonSessionLoader::loadSession);

    bool modulesFound = modMan.loadAllAvailableModulePlugins();
    QVERIFY(modulesFound);

    const QString defaultSessionFile = mmConfig->getDefaultSession();
    modMan.changeSessionFile(defaultSessionFile);
    mmController.initOnce();

    ModuleManagerTest::feedEventLoop();

    QVERIFY(storSystem.hasStoredValue(0, "EntityName"));
    QString actDevIface = storSystem.getStoredValue(9999, "ACT_DEV_IFACE").toString();
    if(actDevIface.isEmpty()) // we have to make module resilient to this situation
        qFatal("ACT_DEV_IFACE empty - local modulemanager running???");
}

