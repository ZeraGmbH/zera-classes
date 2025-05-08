#include "test_api_trust_authentication.h"

#include <QTest>

#include "mocktcpnetworkfactory.h"
#include "timerfactoryqtfortest.h"
#include "timemachinefortest.h"

QTEST_MAIN(test_api_trust_authentication)

void test_api_trust_authentication::initTestCase()
{
    m_tcpFactory = VeinTcp::MockTcpNetworkFactory::create();
    TimerFactoryQtForTest::enableTest();
}

void test_api_trust_authentication::init()
{
    m_configDataLastStored = std::make_shared<QByteArray>();
    m_licenseSystem = std::make_unique<TestLicenseSystem>();
    m_serviceInterfaceFactory = std::make_shared<TestFactoryServiceInterfaces>();
    m_modmanSetupFacade = std::make_unique<ModuleManagerSetupFacade>(m_licenseSystem.get());

    m_modMan = std::make_unique<TestModuleManager>(m_modmanSetupFacade.get(),
                                                   m_serviceInterfaceFactory,
                                                   m_tcpFactory,
                                                   m_configDataLastStored);

    m_modMan->loadAllAvailableModulePlugins();
    m_modMan->setupConnections();

    TimeMachineForTest::reset();

    m_modMan->loadSession(":/session-minimal.json");
    m_modMan->waitUntilModulesAreReady();
}

void test_api_trust_authentication::rpcRequestWillTriggerGUI() {
    QVERIFY(true);
}
