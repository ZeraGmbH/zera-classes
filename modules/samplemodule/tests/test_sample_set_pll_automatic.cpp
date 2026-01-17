#include "test_sample_set_pll_automatic.h"
#include "modulemanagertestrunner.h"
#include "pcbserviceconnection.h"
#include "pllautomatic.h"
#include "taskpllchannelget.h"
#include <testloghelpers.h>
#include <timemachineobject.h>
#include <QTest>

QTEST_MAIN(test_sample_set_pll_automatic)

static int constexpr sampleEntityId = 1030;

void test_sample_set_pll_automatic::rejectOnFixedFrequency()
{
    ModuleManagerTestRunner testRunner(":/sessions/sample-only-fixed-freq.json");
    QCOMPARE(testRunner.getVfComponent(sampleEntityId, "PAR_PllAutomaticOnOff"), 0);

    testRunner.setVfComponent(sampleEntityId, "PAR_PllAutomaticOnOff", "1");
    QCOMPARE(testRunner.getVfComponent(sampleEntityId, "PAR_PllAutomaticOnOff"), 0);
}

constexpr int phaseCountMt = 8;
constexpr float epsilon = 1e-6;

constexpr float RangeUMt = 250;
constexpr float UMtLimit = RangeUMt * PllAutomatic::RelativeAmplitudeMinForPllChannelSelection;
constexpr float UMtAbove = UMtLimit + epsilon;
constexpr float UMtBelow = UMtLimit - epsilon;

void test_sample_set_pll_automatic::startActivatedAndFireValues()
{
    ModuleManagerTestRunner testRunner(":/sessions/range-and-sample.json");
    QCOMPARE(testRunner.getVfComponent(sampleEntityId, "PAR_PllAutomaticOnOff"), 1);

    DemoValuesDspRange dspRangeValues(phaseCountMt);
    fireRangeValues(testRunner, UMtBelow, QList<int>() << 0, dspRangeValues);
    fireRangeValues(testRunner, UMtBelow, QList<int>() << 1, dspRangeValues);
    fireRangeValues(testRunner, UMtAbove, QList<int>() << 2, dspRangeValues);
    QCOMPARE(testRunner.getVfComponent(sampleEntityId, "PAR_PllChannel"), "UL3");
}

void test_sample_set_pll_automatic::activateOnNoValuesFired()
{
    ModuleManagerTestRunner testRunner(":/sessions/range-and-sample.json");
    testRunner.setVfComponent(sampleEntityId, "PAR_PllAutomaticOnOff", "0");
    QCOMPARE(testRunner.getVfComponent(sampleEntityId, "PAR_PllAutomaticOnOff"), 0);
    testRunner.setVfComponent(sampleEntityId, "PAR_PllChannel", "IL1");
    QCOMPARE(testRunner.getVfComponent(sampleEntityId, "PAR_PllChannel"), "IL1");

    testRunner.setVfComponent(sampleEntityId, "PAR_PllAutomaticOnOff", "1");
    QCOMPARE(testRunner.getVfComponent(sampleEntityId, "PAR_PllAutomaticOnOff"), 1);
    QCOMPARE(testRunner.getVfComponent(sampleEntityId, "PAR_PllChannel"), "UL1");

    TaskContainerQueue taskQueue;
    VeinTcp::AbstractTcpNetworkFactoryPtr networkFactory = VeinTcp::MockTcpNetworkFactory::create();
    PcbServiceConnection pcbConnection(NetworkConnectionInfo("127.0.0.1", 6307), networkFactory);
    std::shared_ptr<QString> pllChannelMName = std::make_unique<QString>();
    taskQueue.addSub(pcbConnection.createConnectionTask());
    taskQueue.addSub(TaskPllChannelGet::create(pcbConnection.getInterface(),
                                               pllChannelMName,
                                               TRANSACTION_TIMEOUT));
    TimeMachineObject::feedEventLoop();
    QCOMPARE(*pllChannelMName, "m0");
}

void test_sample_set_pll_automatic::fireRangeValues(ModuleManagerTestRunner &testRunner,
                                                    float rmsValue,
                                                    QList<int> activePhaseIdxs,
                                                    DemoValuesDspRange &dspRangeValues)
{
    TestDspInterfacePtr dspInterface = testRunner.getDspInterface(RangeModuleEntityId, MODULEPROG);
    for(int i = 0; i < phaseCountMt; i++)
        if (activePhaseIdxs.contains(i))
            dspRangeValues.setRmsPeakDCValue(i, rmsValue);
    dspInterface->fireActValInterrupt(dspRangeValues.getDspValues(), /* dummy */ 0);
    TimeMachineObject::feedEventLoop();
}
