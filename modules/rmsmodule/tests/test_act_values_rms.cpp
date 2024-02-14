#include "test_act_values_rms.h"
#include <timemachineobject.h>
#include <QTest>

QTEST_MAIN(test_act_values_rms)

void test_act_values_rms::cleanup()
{
    m_modMan->destroyModulesAndWaitUntilAllShutdown();
    m_modMan = nullptr;
    m_modmanFacade = nullptr;
    m_licenseSystem = nullptr;
}

void test_act_values_rms::minimalSession()
{
    setupServices(":/minimal-session-rms.json");

    VeinStorage::VeinHash* storageHash = m_modmanFacade->getStorageSystem();
    QList<int> entityList = storageHash->getEntityList();
    QCOMPARE(entityList.count(), 2);
    QVERIFY(storageHash->hasEntity(1040));
}

void test_act_values_rms::setupServices(QString sessionFileName)
{
    m_licenseSystem = std::make_unique<LicenseSystemMock>();
    m_modmanFacade = std::make_unique<ModuleManagerSetupFacade>(m_licenseSystem.get());
    m_modMan = std::make_unique<TestModuleManager>(m_modmanFacade.get(), true);
    m_modMan->loadAllAvailableModulePlugins();
    m_modMan->setupConnections();
    m_modMan->startAllServiceMocks("mt310s2");
    m_modMan->loadSession(sessionFileName);
    TimeMachineObject::feedEventLoop();
}
