#include "test_act_values_rms.h"
#include <timemachineobject.h>
#include <QBuffer>
#include <QTest>

QTEST_MAIN(test_act_values_rms)

void test_act_values_rms::cleanup()
{
    m_modMan->destroyModulesAndWaitUntilAllShutdown();
    m_modMan = nullptr;
    TimeMachineObject::feedEventLoop();
    m_modmanFacade = nullptr;
    m_licenseSystem = nullptr;
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
    m_modMan->waitUntilModulesAreReady();
}
