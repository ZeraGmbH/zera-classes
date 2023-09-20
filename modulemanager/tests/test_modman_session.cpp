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


void test_modman_session::loadSession()
{
    ModulemanagerConfigTest::enableTest();
    ModulemanagerConfig::setDemoDevice("demo");
    ModuleManagerTest::enableTest();

    ModulemanagerConfig* mmConfig = ModulemanagerConfig::getInstance();
    const QString defaultSessionFile = mmConfig->getDefaultSession();
    const QStringList availableSessionList = mmConfig->getAvailableSessions();

    ModuleEventHandler *evHandler = new ModuleEventHandler;
    // setup vein modules
    ModuleManagerController *mmController = new ModuleManagerController();
    VeinNet::IntrospectionSystem *introspectionSystem = new VeinNet::IntrospectionSystem();
    VeinStorage::VeinHash *storSystem = new VeinStorage::VeinHash();

    VeinNet::NetworkSystem *netSystem = new VeinNet::NetworkSystem();
    netSystem->setOperationMode(VeinNet::NetworkSystem::VNOM_SUBSCRIPTION);

    VeinNet::TcpSystem *tcpSystem = new VeinNet::TcpSystem();
    LicenseSystemMock *licenseSystem = new LicenseSystemMock;

    ZeraModules::ModuleManager *modMan = new ModuleManagerTest(availableSessionList);
    modMan->setDemo(true);

    JsonSessionLoader *sessionLoader = new JsonSessionLoader();

    QList<VeinEvent::EventSystem*> subSystems;
    //do not reorder
    subSystems.append(mmController);
    subSystems.append(introspectionSystem);
    subSystems.append(storSystem);
    subSystems.append(netSystem);
    subSystems.append(tcpSystem);
    subSystems.append(licenseSystem);
    evHandler->setSubsystems(subSystems);

    modMan->setStorage(storSystem);
    modMan->setLicenseSystem(licenseSystem);
    modMan->setEventHandler(evHandler);
    mmController->setStorage(storSystem);

    QObject::connect(sessionLoader, &JsonSessionLoader::sigLoadModule, modMan, &ZeraModules::ModuleManager::startModule);
    QObject::connect(modMan, &ZeraModules::ModuleManager::sigSessionSwitched, sessionLoader, &JsonSessionLoader::loadSession);

    qRegisterMetaTypeStreamOperators<QList<int> >("QList<int>");
    qRegisterMetaTypeStreamOperators<QList<float> >("QList<float>");
    qRegisterMetaTypeStreamOperators<QList<double> >("QList<double>");
    qRegisterMetaTypeStreamOperators<QList<QString> >("QList<QString>");
    qRegisterMetaTypeStreamOperators<QVector<QString> >("QVector<QString>");
    qRegisterMetaTypeStreamOperators<QList<QVariantMap> >("QList<QVariantMap>");

    bool modulesFound = modMan->loadModules();
    QVERIFY(modulesFound);

    modMan->changeSessionFile(defaultSessionFile);
    mmController->initOnce();
    tcpSystem->startServer(12000);


}
