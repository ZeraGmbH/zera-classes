#include "test_change_session.h"
#include "testfactoryserviceinterfaces.h"
#include "modulemanagertestrunner.h"
#include "modulemanagerconfig.h"
#include "modulemanagerconfigtest.h"
#include "scpimoduleclientblocked.h"
#include "vf_client_component_setter.h"
#include <timemachineobject.h>
#include <QTest>


QTEST_MAIN(test_change_session)

static int constexpr systemEntityId = 0;

void test_change_session::initTestCase()
{
    m_serviceInterfaceFactory = std::make_shared<TestFactoryServiceInterfaces>();
    TestModuleManager::supportOeTests();
    TestModuleManager::pointToInstalledSessionFiles();
    qputenv("QT_FATAL_CRITICALS", "1");
}

void test_change_session::changeToUnavailableSession()
{
    TestLicenseSystem licenseSystem;
    ModuleManagerSetupFacade modManSetupFacade(&licenseSystem);

    TestModuleManager modMan(&modManSetupFacade, m_serviceInterfaceFactory);
    modMan.loadAllAvailableModulePlugins();
    modMan.setupConnections();
    modMan.startAllTestServices("mt310s2", false);
    modMan.changeSessionFile("mt310s2-meas-session.json");
    modMan.waitUntilModulesAreReady();

    QVariant oldValue = modManSetupFacade.getStorageSystem()->getStoredValue(systemEntityId, "Session");
    QEvent* event = VfClientComponentSetter::generateEvent(systemEntityId, "Session", oldValue, "XYZ");
    emit modManSetupFacade.getStorageSystem()->sigSendEvent(event); // could be any event system
    modMan.waitUntilModulesAreReady();
    QCOMPARE(modManSetupFacade.getStorageSystem()->getStoredValue(systemEntityId, "Session").toString(), QString("mt310s2-meas-session.json"));
    modMan.destroyModulesAndWaitUntilAllShutdown();
}

void test_change_session::changeToSameSession()
{
    TestLicenseSystem licenseSystem;
    ModuleManagerSetupFacade modManSetupFacade(&licenseSystem);

    TestModuleManager modMan(&modManSetupFacade, m_serviceInterfaceFactory);
    modMan.loadAllAvailableModulePlugins();
    modMan.setupConnections();
    modMan.startAllTestServices("mt310s2", false);
    modMan.changeSessionFile("mt310s2-meas-session.json");
    modMan.waitUntilModulesAreReady();

    QVariant oldValue = modManSetupFacade.getStorageSystem()->getStoredValue(systemEntityId, "Session");
    QEvent* event = VfClientComponentSetter::generateEvent(systemEntityId, "Session", oldValue, "mt310s2-meas-session.json");
    emit modManSetupFacade.getStorageSystem()->sigSendEvent(event); // could be any event system
    modMan.waitUntilModulesAreReady();
    QCOMPARE(modManSetupFacade.getStorageSystem()->getStoredValue(systemEntityId, "Session").toString(), QString("mt310s2-meas-session.json"));
    modMan.destroyModulesAndWaitUntilAllShutdown();
}

void test_change_session::changeSessionMt310s2FromComponent()
{
    TestLicenseSystem licenseSystem;
    ModuleManagerSetupFacade modManSetupFacade(&licenseSystem);

    TestModuleManager modMan(&modManSetupFacade, m_serviceInterfaceFactory);
    modMan.loadAllAvailableModulePlugins();
    modMan.setupConnections();
    modMan.startAllTestServices("mt310s2", false);
    modMan.changeSessionFile("mt310s2-meas-session.json");
    modMan.waitUntilModulesAreReady();

    QVariant oldValue = modManSetupFacade.getStorageSystem()->getStoredValue(systemEntityId, "Session");
    QEvent* event = VfClientComponentSetter::generateEvent(systemEntityId, "Session", oldValue, "mt310s2-dc-session.json");
    emit modManSetupFacade.getStorageSystem()->sigSendEvent(event); // could be any event system
    modMan.waitUntilModulesAreReady();
    QCOMPARE(modManSetupFacade.getStorageSystem()->getStoredValue(systemEntityId, "Session").toString(), QString("mt310s2-dc-session.json"));
    modMan.destroyModulesAndWaitUntilAllShutdown();
}

void test_change_session::changeSessionMt310s2SCPICmd()
{
    TestLicenseSystem licenseSystem;
    ModuleManagerSetupFacade modManSetupFacade(&licenseSystem);

    TestModuleManager modMan(&modManSetupFacade, m_serviceInterfaceFactory);
    modMan.loadAllAvailableModulePlugins();
    modMan.setupConnections();
    modMan.startAllTestServices("mt310s2", false);
    modMan.changeSessionFile("mt310s2-meas-session.json");
    modMan.waitUntilModulesAreReady();

    ScpiModuleClientBlocked client;
    client.sendReceive("CONFIGURATION:SYST:NAMESESSION EMOB DC;");
    modMan.waitUntilModulesAreReady();
    QCOMPARE(modManSetupFacade.getStorageSystem()->getStoredValue(systemEntityId, "Session").toString(), QString("mt310s2-emob-session-dc.json"));
    modMan.destroyModulesAndWaitUntilAllShutdown();
}

void test_change_session::changeSessionCom5003FromComponent()
{
    ModulemanagerConfig::setDemoDevice("com5003", false);
    TestLicenseSystem licenseSystem;
    ModuleManagerSetupFacade modManSetupFacade(&licenseSystem);

    TestModuleManager modMan(&modManSetupFacade, m_serviceInterfaceFactory);
    modMan.loadAllAvailableModulePlugins();
    modMan.setupConnections();
    modMan.startAllTestServices("com5003", false);
    modMan.changeSessionFile("com5003-meas-session.json");
    modMan.waitUntilModulesAreReady();

    QVariant oldValue = modManSetupFacade.getStorageSystem()->getStoredValue(systemEntityId, "Session");
    QEvent* event = VfClientComponentSetter::generateEvent(systemEntityId, "Session", oldValue, "com5003-ced-session.json");
    emit modManSetupFacade.getStorageSystem()->sigSendEvent(event); // could be any event system
    modMan.waitUntilModulesAreReady();
    QCOMPARE(modManSetupFacade.getStorageSystem()->getStoredValue(systemEntityId, "Session").toString(), QString("com5003-ced-session.json"));
    modMan.destroyModulesAndWaitUntilAllShutdown();
}

void test_change_session::changeSessionCom5003SCPICmd()
{
    ModulemanagerConfig::setDemoDevice("com5003", false);
    TestLicenseSystem licenseSystem;
    ModuleManagerSetupFacade modManSetupFacade(&licenseSystem);

    TestModuleManager modMan(&modManSetupFacade, m_serviceInterfaceFactory);
    modMan.loadAllAvailableModulePlugins();
    modMan.setupConnections();
    modMan.startAllTestServices("com5003", false);
    modMan.changeSessionFile("com5003-meas-session.json");
    modMan.waitUntilModulesAreReady();

    ScpiModuleClientBlocked client;
    client.sendReceive("CONFIGURATION:SYST:NAMESESSION 3 Systems / 2 Wires;");
    modMan.waitUntilModulesAreReady();
    QCOMPARE(modManSetupFacade.getStorageSystem()->getStoredValue(systemEntityId, "Session").toString(), QString("com5003-perphase-session.json"));
    modMan.destroyModulesAndWaitUntilAllShutdown();
}
