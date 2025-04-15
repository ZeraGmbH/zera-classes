#include "test_range_automatic.h"
#include "demovaluesdsprange.h"
#include "vf_client_component_setter.h"
#include <mocktcpnetworkfactory.h>
#include <testfactoryi2cctrl.h>
#include <testfactorydevicenodedsp.h>
#include <tcpnetworkfactory.h>
#include <clamp.h>
#include <clampfactorytest.h>
#include <mocki2ceepromiofactory.h>
#include <timemachinefortest.h>
#include <timerfactoryqtfortest.h>
#include <testloghelpers.h>
#include <xmldocumentcompare.h>
#include <vs_dumpjson.h>
#include <QTest>

QTEST_MAIN(test_range_automatic)

static int constexpr rangeEntityId = 1020;
static QString UL1RangeComponent("PAR_Channel1Range");
static QString UL2RangeComponent("PAR_Channel2Range");
static QString UL3RangeComponent("PAR_Channel3Range");
static QString IL1RangeComponent("PAR_Channel4Range");
static QString IL2RangeComponent("PAR_Channel5Range");
static QString IL3RangeComponent("PAR_Channel6Range");
static QString UAUXRangeComponent("PAR_Channel7Range");
static QString IAUXRangeComponent("PAR_Channel8Range");
static QString RangeAutomaticComponent("PAR_RangeAutomatic");
static QString RangeGroupingComponent("PAR_ChannelGrouping");

void test_range_automatic::initTestCase()
{
    ClampFactoryTest::enableTest();
    MockI2cEEpromIoFactory::enableMock();
    m_tcpFactory = VeinTcp::MockTcpNetworkFactory::create();
    TimerFactoryQtForTest::enableTest();
}

void test_range_automatic::init()
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
    m_testPcbServer = nullptr;
    TimeMachineObject::feedEventLoop();
    m_resmanServer = nullptr;
    TimeMachineObject::feedEventLoop();
}

void test_range_automatic::defaultRangesAndSetting()
{
    QCOMPARE(getCurrentRanges(), QStringList() << "250V" << "250V" << "250V" << "10A" << "10A" << "10A" << "250V" << "--");
}

void test_range_automatic::activeGroupingChangeSingleRange()
{
    m_testPcbServer->addClamp(cClamp::CL120A, "IAUX");
    TimeMachineObject::feedEventLoop();

    // Initial state with grouping
    setVfComponent(rangeEntityId, RangeGroupingComponent, 1);
    QCOMPARE(getCurrentRanges(), QStringList() << "250V" << "250V" << "250V" << "10A" << "10A" << "10A" << "250V" << "--");

    setVfComponent(rangeEntityId, UL1RangeComponent, "100mV");
    QCOMPARE(getCurrentRanges(), QStringList() << "100mV" << "100mV" << "100mV" << "10A" << "10A" << "10A" << "250V" << "--");

    setVfComponent(rangeEntityId, UL2RangeComponent, "8V");
    QCOMPARE(getCurrentRanges(), QStringList() << "8V" << "8V" << "8V" << "10A" << "10A" << "10A" << "250V" << "--");

    setVfComponent(rangeEntityId, UAUXRangeComponent, "8V");
    QCOMPARE(getCurrentRanges(), QStringList() << "8V" << "8V" << "8V" << "10A" << "10A" << "10A" << "8V" << "--");

    setVfComponent(rangeEntityId, UL3RangeComponent, "100mV");
    QCOMPARE(getCurrentRanges(), QStringList() << "100mV" << "100mV" << "100mV" << "10A" << "10A" << "10A" << "8V" << "--");

    setVfComponent(rangeEntityId, IAUXRangeComponent, "C100A");
    QCOMPARE(getCurrentRanges(), QStringList() << "100mV" << "100mV" << "100mV" << "10A" << "10A" << "10A" << "8V" << "C100A");

    setVfComponent(rangeEntityId, IL1RangeComponent, "25mA");
    QCOMPARE(getCurrentRanges(), QStringList() << "100mV" << "100mV" << "100mV" << "25mA" << "25mA" << "25mA" << "8V" << "C100A");

    setVfComponent(rangeEntityId, IL2RangeComponent, "100mA");
    QCOMPARE(getCurrentRanges(), QStringList() << "100mV" << "100mV" << "100mV" << "100mA" << "100mA" << "100mA" << "8V" << "C100A");

    setVfComponent(rangeEntityId, IL3RangeComponent, "250mA");
    QCOMPARE(getCurrentRanges(), QStringList() << "100mV" << "100mV" << "100mV" << "250mA" << "250mA" << "250mA" << "8V" << "C100A");

    setVfComponent(rangeEntityId, IAUXRangeComponent, "C50A");
    QCOMPARE(getCurrentRanges(), QStringList() << "100mV" << "100mV" << "100mV" << "250mA" << "250mA" << "250mA" << "8V" << "C50A");
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

constexpr double ovrRejectionFactor = 1.2499999;
constexpr double enterRangeLimit    = 0.9499999;
constexpr double midOfHysteresis    = 0.97; // (keepRangeLimit + enterRangeLimit) / 2;
constexpr double keepRangeLimit     = 0.99;
constexpr double outsideRangeLimit  = 1.0;


void test_range_automatic::testRangeAutomaticIncreaseU()
{
    // Increasing uses hysteresis region
    // Logic of hysteresis to stay in range needs this conditions
    // (rms > Urvalue_range * ovrRejectionFactor * enterRangeLimit) && (rms < Urvalue_range * ovrRejectionFactor * keepRangeLimit)

    fireNewRmsValues(0.1);     // set U-Range to 100mV
    fireNewRmsValues(0.1);     // extra interrupt
    setVfComponent(rangeEntityId, RangeAutomaticComponent, 1);
    QCOMPARE(getVfComponent(rangeEntityId, UL1RangeComponent), "100mV");

    fireNewRmsValues(0.1 * ovrRejectionFactor * midOfHysteresis);      // stay in U-Range 100mV
    fireNewRmsValues(0.1 * ovrRejectionFactor * midOfHysteresis);      // extra interrupt
    QCOMPARE(getVfComponent(rangeEntityId, UL1RangeComponent), "100mV");

    fireNewRmsValues(0.1 * ovrRejectionFactor * outsideRangeLimit);    // switch in U-Range 8V
    fireNewRmsValues(0.1 * ovrRejectionFactor * outsideRangeLimit);    // extra interrupt
    QCOMPARE(getVfComponent(rangeEntityId, UL1RangeComponent), "8V");

    fireNewRmsValues(8 * ovrRejectionFactor * midOfHysteresis);        // stay in U-Range 8V
    fireNewRmsValues(8 * ovrRejectionFactor * midOfHysteresis);        // extra interrupt
    QCOMPARE(getVfComponent(rangeEntityId, UL1RangeComponent), "8V");

    fireNewRmsValues(8 * ovrRejectionFactor * outsideRangeLimit);      // switch in U-Range to 250V
    fireNewRmsValues(8 * ovrRejectionFactor * outsideRangeLimit);
    QCOMPARE(getVfComponent(rangeEntityId, UL1RangeComponent), "250V");
}


void test_range_automatic::testRangeAutomaticDecreaseU()
{
    // Decreasing uses NOT hysteresis region
    // Logic of hysteresis to stay in range needs this conditions
    // (rms > Urvalue_range * ovrRejectionFactor * enterRangeLimit) && (rms < Urvalue_range * ovrRejectionFactor * keepRangeLimit)

    fireNewRmsValues(250);     // switch to 250V set U-Range to 250V
    fireNewRmsValues(250);     // extra interrupt
    setVfComponent(rangeEntityId, RangeAutomaticComponent, 1);
    QCOMPARE(getVfComponent(rangeEntityId, UL1RangeComponent), "250V");

    fireNewRmsValues(8 * ovrRejectionFactor *  midOfHysteresis);       // stay in U-Range 250V
    fireNewRmsValues(8 * ovrRejectionFactor *  midOfHysteresis);       // extra interrupt
    QCOMPARE(getVfComponent(rangeEntityId, UL1RangeComponent), "250V");

    fireNewRmsValues(8 * ovrRejectionFactor * enterRangeLimit);        // switch in U-Range 8V
    fireNewRmsValues(8 * ovrRejectionFactor * enterRangeLimit);
    QCOMPARE(getVfComponent(rangeEntityId, UL1RangeComponent), "8V");

    fireNewRmsValues(0.1 * ovrRejectionFactor * midOfHysteresis);      // stay in U-Range 8V
    fireNewRmsValues(0.1 * ovrRejectionFactor * midOfHysteresis);
    QCOMPARE(getVfComponent(rangeEntityId, UL1RangeComponent), "8V");

    fireNewRmsValues(0.1 * ovrRejectionFactor * enterRangeLimit);      // switch in U-Range 0.1V
    fireNewRmsValues(0.1 * ovrRejectionFactor * enterRangeLimit);
    QCOMPARE(getVfComponent(rangeEntityId, UL1RangeComponent), "100mV");
}


void test_range_automatic::testRangeAutomaticIncreaseLowRangesI()
{
    // Logic of hysteresis to stay in range needs this conditions
    // (rms > Urvalue_range * ovrRejectionFactor * enterRangeLimit) && (rms < Urvalue_range * ovrRejectionFactor * keepRangeLimit)

    // test hysteresis from range 25mA up to 100mA
    fireNewRmsValues(0);         // switch to 25mA range
    fireNewRmsValues(0);         // extra interrupt
    setVfComponent(rangeEntityId, RangeAutomaticComponent, 1);
    QCOMPARE(getVfComponent(rangeEntityId, IL1RangeComponent), "25mA");

    fireNewRmsValues(0.025 * ovrRejectionFactor * midOfHysteresis);    // stay in I-Range 25mA
    fireNewRmsValues(0.025 * ovrRejectionFactor * midOfHysteresis);
    QCOMPARE(getVfComponent(rangeEntityId, IL1RangeComponent), "25mA");

    fireNewRmsValues(0.025 * ovrRejectionFactor * outsideRangeLimit);  // switch in I-Range 50mA
    fireNewRmsValues(0.025 * ovrRejectionFactor * outsideRangeLimit);
    QCOMPARE(getVfComponent(rangeEntityId, IL1RangeComponent), "50mA");

    fireNewRmsValues(0.05 * ovrRejectionFactor * midOfHysteresis);     // stay in I-Range 50mA
    fireNewRmsValues(0.05 * ovrRejectionFactor * midOfHysteresis);
    QCOMPARE(getVfComponent(rangeEntityId, IL1RangeComponent), "50mA");

    fireNewRmsValues(0.05 * ovrRejectionFactor * outsideRangeLimit);   // switch in I-Range 100mA
    fireNewRmsValues(0.05 * ovrRejectionFactor * outsideRangeLimit);
    QCOMPARE(getVfComponent(rangeEntityId, IL1RangeComponent), "100mA");
}


void test_range_automatic::testRangeAutomaticIncreaseHighRangesI()
{
    // Logic of hysteresis to stay in range needs this conditions
    // (rms > Urvalue_range * ovrRejectionFactor * enterRangeLimit) && (rms < Urvalue_range * ovrRejectionFactor * keepRangeLimit)

    // test hysteresis from range 2.5A up to 10A
    fireNewRmsValues(2);     // switch to 2.5A range
    fireNewRmsValues(2);     // extra interrupt
    setVfComponent(rangeEntityId, RangeAutomaticComponent, 1);
    QCOMPARE(getVfComponent(rangeEntityId, IL1RangeComponent), "2.5A");

    fireNewRmsValues(2.5 * ovrRejectionFactor * midOfHysteresis);      // stay in I-Range 2.5A
    fireNewRmsValues(2.5 * ovrRejectionFactor * midOfHysteresis);
    QCOMPARE(getVfComponent(rangeEntityId, IL1RangeComponent), "2.5A");

    fireNewRmsValues(2.5 * ovrRejectionFactor * outsideRangeLimit);    // switch in I-Range 5A
    fireNewRmsValues(2.5 * ovrRejectionFactor * outsideRangeLimit);
    QCOMPARE(getVfComponent(rangeEntityId, IL1RangeComponent), "5A");

    fireNewRmsValues(5 * ovrRejectionFactor * midOfHysteresis);        // stay in I-Range 5A
    fireNewRmsValues(5 * ovrRejectionFactor * midOfHysteresis);
    QCOMPARE(getVfComponent(rangeEntityId, IL1RangeComponent), "5A");

    fireNewRmsValues(5 * ovrRejectionFactor * outsideRangeLimit);      // switch in I-Range 10A
    fireNewRmsValues(5 * ovrRejectionFactor * outsideRangeLimit);
    QCOMPARE(getVfComponent(rangeEntityId, IL1RangeComponent), "10A");
}


void test_range_automatic::testRangeAutomaticDecreaseI()
{
    // Decreasing uses NOT hysteresis region
    // Logic of hysteresis to stay in range needs this conditions
    // (rms > Urvalue_range * ovrRejectionFactor * enterRangeLimit) && (rms < Urvalue_range * ovrRejectionFactor * keepRangeLimit)

    fireNewRmsValues(10);      // switch to 10A set I-Range to 10A
    fireNewRmsValues(10);      // extra interrupt
    setVfComponent(rangeEntityId, RangeAutomaticComponent, 1);
    QCOMPARE(getVfComponent(rangeEntityId, IL1RangeComponent), "10A");

    fireNewRmsValues(5 * ovrRejectionFactor * midOfHysteresis);        // stay in I-Range 10A
    fireNewRmsValues(5 * ovrRejectionFactor * midOfHysteresis);
    QCOMPARE(getVfComponent(rangeEntityId, IL1RangeComponent), "10A");

    fireNewRmsValues(5 * ovrRejectionFactor * enterRangeLimit);        // switch in I-Range 5A
    fireNewRmsValues(5 * ovrRejectionFactor * enterRangeLimit);
    QCOMPARE(getVfComponent(rangeEntityId, IL1RangeComponent), "5A");

    fireNewRmsValues(2.5 * ovrRejectionFactor * midOfHysteresis);      // stay in I-Range 5A
    fireNewRmsValues(2.5 * ovrRejectionFactor * midOfHysteresis);
    QCOMPARE(getVfComponent(rangeEntityId, IL1RangeComponent), "5A");

    fireNewRmsValues(2.5 * ovrRejectionFactor * enterRangeLimit);      // switch in I-Range 2.5A
    fireNewRmsValues(2.5 * ovrRejectionFactor * enterRangeLimit);
    QCOMPARE(getVfComponent(rangeEntityId, IL1RangeComponent), "2.5A");
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

void test_range_automatic::addAndSelectClamp()
{
    setVfComponent(rangeEntityId, RangeGroupingComponent, 0);
    m_testPcbServer->addClamp(cClamp::CL120A, "IL1");
    TimeMachineObject::feedEventLoop();

    setVfComponent(rangeEntityId, IL1RangeComponent, "100mA");
    QCOMPARE(getVfComponent(rangeEntityId, IL1RangeComponent), "100mA");
    QCOMPARE(getVfComponent(rangeEntityId, IL2RangeComponent), "10A"); //default

    setVfComponent(rangeEntityId, IL1RangeComponent, "C10A");
    QCOMPARE(getVfComponent(rangeEntityId, IL1RangeComponent), "C10A");
    QCOMPARE(getVfComponent(rangeEntityId, IL2RangeComponent), "10A");
}

void test_range_automatic::selectClampThenRangeAutomatic()
{
    setVfComponent(rangeEntityId, RangeGroupingComponent, 0);
    m_testPcbServer->addClamp(cClamp::CL120A, "IL1");
    TimeMachineObject::feedEventLoop();
    setVfComponent(rangeEntityId, IL1RangeComponent, "C10A");

    fireNewRmsValues(4);
    setVfComponent(rangeEntityId, RangeAutomaticComponent, 1);
    QCOMPARE(getVfComponent(rangeEntityId, UL1RangeComponent), "8V");
    QCOMPARE(getVfComponent(rangeEntityId, IL1RangeComponent), "C5A");

    fireNewRmsValues(25); //overload
    //After setting new range (above 8V, C5A), all range related processing is disabled for 1 Actual value interrupt cycle
    //So, fire an extra interrupt.
    fireNewRmsValues(25);
    QCOMPARE(getVfComponent(rangeEntityId, "PAR_Overload"), 1);
    QCOMPARE(getVfComponent(rangeEntityId, UL1RangeComponent), "250V");
    QCOMPARE(getVfComponent(rangeEntityId, IL1RangeComponent), "C100A");

    fireNewRmsValues(25);
    //After setting new range (above 250V, C100A), all range related processing is disabled for 1 Actual value interrupt cycle
    //So, fire an extra interrupt.
    fireNewRmsValues(25);
    QCOMPARE(getVfComponent(rangeEntityId, UL1RangeComponent), "250V");
    QCOMPARE(getVfComponent(rangeEntityId, IL1RangeComponent), "C50A");
}

void test_range_automatic::addRemoveClamp()
{
    setVfComponent(rangeEntityId, RangeGroupingComponent, 0);

    // range automatic OFF
    m_testPcbServer->addClamp(cClamp::CL120A, "IL1");
    TimeMachineObject::feedEventLoop();
    setVfComponent(rangeEntityId, IL1RangeComponent, "C50mA");
    QCOMPARE(getVfComponent(rangeEntityId, IL1RangeComponent), "C50mA");

    m_testPcbServer->removeAllClamps();
    TimeMachineObject::feedEventLoop();
    QCOMPARE(getVfComponent(rangeEntityId, IL1RangeComponent), "10A");//switches to max range

    // range automatic ON
    m_testPcbServer->addClamp(cClamp::CL120A, "IL1");
    TimeMachineObject::feedEventLoop();
    setVfComponent(rangeEntityId, IL1RangeComponent, "C10A");

    fireNewRmsValues(4);
    setVfComponent(rangeEntityId, RangeAutomaticComponent, 1);
    QCOMPARE(getVfComponent(rangeEntityId, IL1RangeComponent), "C5A");

    m_testPcbServer->removeAllClamps();
    TimeMachineObject::feedEventLoop();
    QCOMPARE(getVfComponent(rangeEntityId, IL1RangeComponent), "10A");//first switches to max

    fireNewRmsValues(4);//this interrupt is ignored
    fireNewRmsValues(4);//one more interrupt, as RangeAutomatic is called only after interrupt
    QCOMPARE(getVfComponent(rangeEntityId, IL1RangeComponent), "5A");
}

void test_range_automatic::checkPersitency()
{
    // At the time of writing range module writes config file on change of
    // range-grouping, change of range-automatic and user changing range
    XmlDocumentCompare compare;
    fireNewRmsValues(4); // necessary to get reproducible results on range automatic on below

    QCOMPARE(getVfComponent(rangeEntityId, RangeGroupingComponent), 1);
    setVfComponent(rangeEntityId, RangeGroupingComponent, 0); // this causes config save
    QByteArray expected = TestLogHelpers::loadFile(":/configDumps/dumpGroupingSet.xml");
    QByteArray dumped = *m_configDataLastStored;
    if(!compare.compareXml(dumped, expected))
        QVERIFY(TestLogHelpers::compareAndLogOnDiff(expected, dumped));

    fireNewRmsValues(4); // necessary to get reproducible results on range automatic on
    QCOMPARE(getVfComponent(rangeEntityId, RangeAutomaticComponent), 0);
    setVfComponent(rangeEntityId, RangeAutomaticComponent, 1); // this causes config save
    expected = TestLogHelpers::loadFile(":/configDumps/dumpAutomaticSet.xml");
    dumped = *m_configDataLastStored;
    if(!compare.compareXml(dumped, expected))
        QVERIFY(TestLogHelpers::compareAndLogOnDiff(expected, dumped));

    // Following is about range alias "--" / name "0A" insanity:
    // shipped configuration has "0A" / config stored gets "--"
    setVfComponent(rangeEntityId, RangeAutomaticComponent, 0); // with automatic on we cannot change range
    QCOMPARE(getCurrentRanges(), QStringList() << "8V" << "8V" << "8V" << "5A" << "5A" << "5A" << "8V" << "--");
    m_testPcbServer->addClamp(cClamp::CL120A, "IAUX");
    TimeMachineObject::feedEventLoop();

    // We are interested in vein valid ranges PAR_Channel8Range -> "--"
    QByteArray jsonExpected = TestLogHelpers::loadFile(":/veinDumps/dumpInitialClamp.json");
    VeinStorage::AbstractEventSystem* veinStorage = m_modmanSetupFacade->getStorageSystem();
    QByteArray jsonDumped = VeinStorage::DumpJson::dumpToByteArray(veinStorage->getDb(), QList<int>() << rangeEntityId);
    QVERIFY(TestLogHelpers::compareAndLogOnDiff(jsonExpected, jsonDumped));

    // set valid / unset valid "--"
    setVfComponent(rangeEntityId, IAUXRangeComponent, "C50A");
    QCOMPARE(getCurrentRanges(), QStringList() << "8V" << "8V" << "8V" << "5A" << "5A" << "5A" << "8V" << "C50A");
    expected = TestLogHelpers::loadFile(":/configDumps/dumpIAUXSet.xml");
    dumped = *m_configDataLastStored;
    if(!compare.compareXml(dumped, expected))
        QVERIFY(TestLogHelpers::compareAndLogOnDiff(expected, dumped));
    setVfComponent(rangeEntityId, IAUXRangeComponent, "--");
    QCOMPARE(getCurrentRanges(), QStringList() << "8V" << "8V" << "8V" << "5A" << "5A" << "5A" << "8V" << "--");
    expected = TestLogHelpers::loadFile(":/configDumps/dumpIAUXUnset.xml");
    dumped = *m_configDataLastStored;
    if(!compare.compareXml(dumped, expected))
        QVERIFY(TestLogHelpers::compareAndLogOnDiff(expected, dumped));

    // set valid / unset invalid "0A"
    setVfComponent(rangeEntityId, IAUXRangeComponent, "C50A");
    QCOMPARE(getCurrentRanges(), QStringList() << "8V" << "8V" << "8V" << "5A" << "5A" << "5A" << "8V" << "C50A");
    setVfComponent(rangeEntityId, IAUXRangeComponent, "0A");
    QCOMPARE(getCurrentRanges(), QStringList() << "8V" << "8V" << "8V" << "5A" << "5A" << "5A" << "8V" << "C50A");
    expected = TestLogHelpers::loadFile(":/configDumps/dumpIAUXSet.xml"); // Still set
    dumped = *m_configDataLastStored;
    if(!compare.compareXml(dumped, expected))
        QVERIFY(TestLogHelpers::compareAndLogOnDiff(expected, dumped));
}

void test_range_automatic::setupServices()
{
    TimeMachineForTest::reset();
    m_resmanServer = std::make_unique<ResmanRunFacade>(m_tcpFactory);
    TimeMachineObject::feedEventLoop();
    m_testPcbServer = std::make_unique<TestServerForSenseInterfaceMt310s2>(std::make_shared<TestFactoryI2cCtrl>(true), m_tcpFactory);
    m_dspServer = std::make_unique<MockZdsp1d>(std::make_shared<TestFactoryDeviceNodeDsp>(), m_tcpFactory);
    TimeMachineObject::feedEventLoop();
}

static constexpr int rangeChannelCount = 8;

void test_range_automatic::fireNewRmsValues(float rmsValue)
{
    TestDspInterfacePtr dspInterface =
        m_serviceInterfaceFactory->getInterface(rangeEntityId, TestFactoryServiceInterfaces::MODULEPROG);
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

QStringList test_range_automatic::getCurrentRanges()
{
    QStringList ranges;
    for(int i = 0; i < rangeChannelCount; i++) {
        const QString componentName = QString("PAR_Channel%1Range").arg(i+1);
        ranges.append(getVfComponent(rangeEntityId, componentName).toString());
    }
    return ranges;
}
