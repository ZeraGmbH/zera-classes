#include "test_modman_session.h"
#include "jsonsessionloader.h"
#include "licensesystemmock.h"
#include "moduleeventhandler.h"
#include "modulemanager.h"
#include "modulemanagerconfigtest.h"
#include "modulemanagertest.h"
#include "modulemanagercontroller.h"
#include "vn_introspectionsystem.h"
#include "vn_networksystem.h"
#include "vn_tcpsystem.h"
#include "vs_veinhash.h"
#include <QDataStream>
#include <QTest>

QTEST_MAIN(test_modman_session)

static constexpr quint16 veinPort = 12001;

void test_modman_session::initTestCase()
{
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
        qInfo("Skipping test_modman_session::loadModulePluginsInstalled in OE");
        return;
    }
    ModulemanagerConfigTest::enableTest();
    ModulemanagerConfig::setDemoDevice("demo");
    ModuleManagerTest::enableTest();

    const QStringList emptyAvailableSessionList;
    ZeraModules::ModuleManager modMan(emptyAvailableSessionList);
    modMan.setDemo(true);

    bool modulesFound = modMan.loadAllAvailableModulePlugins();
    QVERIFY(modulesFound);
}

void test_modman_session::loadModulePluginsOE()
{
    ModulemanagerConfigTest::enableTest();
    ModulemanagerConfig::setDemoDevice("demo");
    ModuleManagerTest::enableTest();

    const QStringList emptyAvailableSessionList;
    ModuleManagerTest modMan(emptyAvailableSessionList);
    modMan.setDemo(true);

    bool modulesFound = modMan.loadAllAvailableModulePlugins();
    QVERIFY(modulesFound);
}

void test_modman_session::startSession()
{
    ModulemanagerConfigTest::enableTest();
    ModulemanagerConfig::setDemoDevice("demo");
    ModuleManagerTest::enableTest();

    ModulemanagerConfig* mmConfig = ModulemanagerConfig::getInstance();
    const QString defaultSessionFile = mmConfig->getDefaultSession();
    const QStringList availableSessionList = mmConfig->getAvailableSessions();

    ModuleEventHandler evHandler;
    // setup vein modules
    ModuleManagerController mmController;
    VeinNet::IntrospectionSystem introspectionSystem;
    VeinStorage::VeinHash storSystem;

    LicenseSystemMock licenseSystem;

    ModuleManagerTest modMan(availableSessionList);
    modMan.setDemo(true);

    JsonSessionLoader sessionLoader;

    QList<VeinEvent::EventSystem*> subSystems;
    //do not reorder
    subSystems.append(&mmController);
    subSystems.append(&introspectionSystem);
    subSystems.append(&storSystem);
    subSystems.append(&licenseSystem);
    evHandler.setSubsystems(subSystems);

    modMan.setStorage(&storSystem);
    modMan.setLicenseSystem(&licenseSystem);
    modMan.setEventHandler(&evHandler);
    mmController.setStorage(&storSystem);

    QObject::connect(&sessionLoader, &JsonSessionLoader::sigLoadModule, &modMan, &ZeraModules::ModuleManager::startModule);
    QObject::connect(&modMan, &ZeraModules::ModuleManager::sigSessionSwitched, &sessionLoader, &JsonSessionLoader::loadSession);

    bool modulesFound = modMan.loadAllAvailableModulePlugins();
    QVERIFY(modulesFound);

    modMan.changeSessionFile(defaultSessionFile);
    mmController.initOnce();

    ModuleManagerTest::feedEventLoop();

    QVERIFY(storSystem.hasStoredValue(0, "EntityName"));
    QString actDevIface = storSystem.getStoredValue(9999, "ACT_DEV_IFACE").toString();
    if(actDevIface.isEmpty()) // we have to make module resilient to this situation
        qFatal("ACT_DEV_IFACE empty - local modulemanager running???");
}

void test_modman_session::keepCodeForNext()
{
    ModulemanagerConfigTest::enableTest();
    ModulemanagerConfig::setDemoDevice("demo");
    ModuleManagerTest::enableTest();

    ModulemanagerConfig* mmConfig = ModulemanagerConfig::getInstance();
    const QString defaultSessionFile = mmConfig->getDefaultSession();
    const QStringList availableSessionList = mmConfig->getAvailableSessions();

    ModuleEventHandler evHandler;
    // setup vein modules
    ModuleManagerController mmController;
    VeinNet::IntrospectionSystem introspectionSystem;
    VeinStorage::VeinHash storSystem;

    VeinNet::NetworkSystem netSystem;
    netSystem.setOperationMode(VeinNet::NetworkSystem::VNOM_SUBSCRIPTION);

    VeinNet::TcpSystem tcpSystem;
    LicenseSystemMock licenseSystem;

    ModuleManagerTest modMan(availableSessionList);
    modMan.setDemo(true);

    JsonSessionLoader sessionLoader;

    QList<VeinEvent::EventSystem*> subSystems;
    //do not reorder
    subSystems.append(&mmController);
    subSystems.append(&introspectionSystem);
    subSystems.append(&storSystem);
    subSystems.append(&netSystem);
    subSystems.append(&tcpSystem);
    subSystems.append(&licenseSystem);
    evHandler.setSubsystems(subSystems);

    modMan.setStorage(&storSystem);
    modMan.setLicenseSystem(&licenseSystem);
    modMan.setEventHandler(&evHandler);
    mmController.setStorage(&storSystem);

    QObject::connect(&sessionLoader, &JsonSessionLoader::sigLoadModule, &modMan, &ZeraModules::ModuleManager::startModule);
    QObject::connect(&modMan, &ZeraModules::ModuleManager::sigSessionSwitched, &sessionLoader, &JsonSessionLoader::loadSession);

    qRegisterMetaTypeStreamOperators<QList<int> >("QList<int>");
    qRegisterMetaTypeStreamOperators<QList<float> >("QList<float>");
    qRegisterMetaTypeStreamOperators<QList<double> >("QList<double>");
    qRegisterMetaTypeStreamOperators<QList<QString> >("QList<QString>");
    qRegisterMetaTypeStreamOperators<QVector<QString> >("QVector<QString>");
    qRegisterMetaTypeStreamOperators<QList<QVariantMap> >("QList<QVariantMap>");

    bool modulesFound = modMan.loadAllAvailableModulePlugins();
    QVERIFY(modulesFound);

    modMan.changeSessionFile(defaultSessionFile);
    mmController.initOnce();
    tcpSystem.startServer(veinPort);

    ModuleManagerTest::feedEventLoop();

    QVERIFY(storSystem.hasStoredValue(0, "EntityName"));
    QString actDevIface = storSystem.getStoredValue(9999, "ACT_DEV_IFACE").toString();
    if(actDevIface.isEmpty()) // we have to make module resilient to this situation
        qFatal("ACT_DEV_IFACE empty - local modulemanager running???");
}
