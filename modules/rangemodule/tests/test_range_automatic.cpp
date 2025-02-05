#include "test_range_automatic.h"
#include "demovaluesdsprange.h"
#include "vf_client_component_setter.h"
#include <mocktcpnetworkfactory.h>
#include <testfactoryi2cctrl.h>
#include <testfactorydevicenodedsp.h>
#include <tcpnetworkfactory.h>
#include <timemachinefortest.h>
#include <QTest>

QTEST_MAIN(test_range_automatic)

static int constexpr rangeEntityId = 1020;
static QString UL1RangeComponent("PAR_Channel1Range");
static QString IL1RangeComponent("PAR_Channel4Range");
static QString RangeAutomaticComponent("PAR_RangeAutomatic");

void test_range_automatic::init()
{
    m_licenseSystem = std::make_unique<TestLicenseSystem>();
    m_serviceInterfaceFactory = std::make_shared<TestFactoryServiceInterfaces>();
    m_modmanSetupFacade = std::make_unique<ModuleManagerSetupFacade>(m_licenseSystem.get());
    m_modMan = std::make_unique<TestModuleManager>(m_modmanSetupFacade.get(), m_serviceInterfaceFactory);

    m_modMan->loadAllAvailableModulePlugins();
    m_modMan->setupConnections();
    setupServices();
    m_modMan->loadSession(":/session-minimal.json");
    m_modMan->waitUntilModulesAreReady();
}

void test_range_automatic::cleanup()
{
    m_modMan->destroyModulesAndWaitUntilAllShutdown();
    m_modMan = nullptr;
    TimeMachineObject::feedEventLoop();
    m_modmanSetupFacade = nullptr;
    m_serviceInterfaceFactory = nullptr;
    m_licenseSystem = nullptr;

    m_dspServer = nullptr;
    m_secServer = nullptr;
    m_testPcbServer = nullptr;
    TimeMachineObject::feedEventLoop();
    m_resmanServer = nullptr;
    TimeMachineObject::feedEventLoop();
}

void test_range_automatic::defaultRangesAndSetting()
{
    QCOMPARE(getVfComponent(rangeEntityId, UL1RangeComponent), "250V");
    QCOMPARE(getVfComponent(rangeEntityId, IL1RangeComponent), "10A");
    QCOMPARE(getVfComponent(rangeEntityId, RangeAutomaticComponent), 0);
}

void test_range_automatic::testRangeAutomatic()
{
    fireNewRmsValues(4);
    setVfComponent(rangeEntityId, RangeAutomaticComponent, 1);
    QCOMPARE(getVfComponent(rangeEntityId, UL1RangeComponent), "8V");
    QCOMPARE(getVfComponent(rangeEntityId, IL1RangeComponent), "5A");

    fireNewRmsValues(0);
    //After setting new range (above 8V, 5A), all range related processing is disabled for 1 Actual value interrupt cycle.
    //So, fire an extra interrupt.
    fireNewRmsValues(0);
    TimeMachineObject::feedEventLoop();
    QCOMPARE(getVfComponent(rangeEntityId, UL1RangeComponent), "100mV");
    QCOMPARE(getVfComponent(rangeEntityId, IL1RangeComponent), "25mA");

    fireNewRmsValues(15);
    //After setting new range (above 100mV, 25mA), all range related processing is disabled for 1 Actual value interrupt cycle.
    //So, fire an extra interrupt.
    fireNewRmsValues(15);
    QCOMPARE(getVfComponent(rangeEntityId, UL1RangeComponent), "250V");
    QCOMPARE(getVfComponent(rangeEntityId, IL1RangeComponent), "10A");
}

void test_range_automatic::enableAndDisableRangeAutomatic()
{
    fireNewRmsValues(0);

    setVfComponent(rangeEntityId, RangeAutomaticComponent, 1);
    QCOMPARE(getVfComponent(rangeEntityId, UL1RangeComponent), "100mV");
    QCOMPARE(getVfComponent(rangeEntityId, IL1RangeComponent), "25mA");

    setVfComponent(rangeEntityId, RangeAutomaticComponent, 0);
    QCOMPARE(getVfComponent(rangeEntityId, UL1RangeComponent), "100mV"); //unchanged
    QCOMPARE(getVfComponent(rangeEntityId, IL1RangeComponent), "25mA"); //unchanged
}

void test_range_automatic::softOverloadWithRangeAutomatic()
{
    fireNewRmsValues(4);
    setVfComponent(rangeEntityId, RangeAutomaticComponent, 1);
    QCOMPARE(getVfComponent(rangeEntityId, UL1RangeComponent), "8V");
    QCOMPARE(getVfComponent(rangeEntityId, IL1RangeComponent), "5A");

    //Introduce overload condition
    fireNewRmsValues(15);
    QCOMPARE(getVfComponent(rangeEntityId, "PAR_Overload"), 0);
    //After setting new range (above 8V, 5A), all range related processing is disabled for 1 Actual value interrupt cycle
    //So, fire an extra interrupt.
    fireNewRmsValues(15);
    TimeMachineObject::feedEventLoop();
    QCOMPARE(getVfComponent(rangeEntityId, "PAR_Overload"), 1);
    QCOMPARE(getVfComponent(rangeEntityId, UL1RangeComponent), "250V");
    QCOMPARE(getVfComponent(rangeEntityId, IL1RangeComponent), "10A");

    //Remove overload condition
    fireNewRmsValues(0.5);
    QCOMPARE(getVfComponent(rangeEntityId, "PAR_Overload"), 1);
    //After setting new range (above 8V, 5A), all range related processing is disabled for 1 Actual value interrupt cycle
    //So, fire an extra interrupt.
    fireNewRmsValues(0.5);
    QCOMPARE(getVfComponent(rangeEntityId, UL1RangeComponent), "8V");
    QCOMPARE(getVfComponent(rangeEntityId, IL1RangeComponent), "500mA");
}

void test_range_automatic::setupServices()
{
    m_tcpFactory = VeinTcp::TcpNetworkFactory::create();
    m_resmanServer = std::make_unique<ResmanRunFacade>(m_tcpFactory);
    TimeMachineObject::feedEventLoop();
    m_testPcbServer = std::make_unique<MockMt310s2d>(std::make_shared<TestFactoryI2cCtrl>(false), m_tcpFactory);
    m_secServer = std::make_unique<MockSec1000d>(m_tcpFactory);
    m_dspServer = std::make_unique<MockZdsp1d>(std::make_shared<TestFactoryDeviceNodeDsp>(), m_tcpFactory);
    TimeMachineObject::feedEventLoop();
}

enum dspInterfaces{
    RangeObsermatic,
    AdjustManagement,
    RangeModuleMeasProgram
};

static constexpr int rangeChannelCount = 8;

void test_range_automatic::fireNewRmsValues(float rmsValue)
{
    TestDspInterfacePtr dspInterface = m_serviceInterfaceFactory->getInterfaceList()[dspInterfaces::RangeModuleMeasProgram];
    DemoValuesDspRange rangeValues(rangeChannelCount);
    for(int i = 0; i < rangeChannelCount; i++)
        rangeValues.setRmsValue(i, rmsValue);
    dspInterface->fireActValInterrupt(rangeValues.getDspValues(), /* dummy */ 0);
    TimeMachineObject::feedEventLoop();
}

void test_range_automatic::setVfComponent(int entityId, QString componentName, QVariant newValue)
{
    QVariant oldValue = getVfComponent(entityId, componentName);
    QEvent* event = VfClientComponentSetter::generateEvent(entityId, componentName, oldValue, newValue);
    emit m_modmanSetupFacade->getStorageSystem()->sigSendEvent(event); // could be any event system
    TimeMachineObject::feedEventLoop();
}

QVariant test_range_automatic::getVfComponent(int entityId, QString componentName)
{
    return m_modmanSetupFacade->getStorageSystem()->getDb()->getStoredValue(entityId, componentName);
}
