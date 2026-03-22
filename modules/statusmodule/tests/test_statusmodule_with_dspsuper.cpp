#include "test_statusmodule_with_dspsuper.h"
#include "modulemanagertestrunner.h"
#include "dspsupertestsupport.h"
#include <timemachinefortest.h>
#include <QTest>

QTEST_MAIN(test_statusmodule_with_dspsuper)

static constexpr int statusEntityId = 1150;
static int constexpr dspSuperEntityId = 9000;

void test_statusmodule_with_dspsuper::dspStatus()
{
    ModuleManagerTestRunner testRunner(":/status-dsp-session.json");
    TestDspInterfacePtr dspInterface = testRunner.getDspInterface(dspSuperEntityId);

    DspSuperTestSupport::fireInterrupt({12.5, 1, 20}, dspInterface);
    TimeMachineForTest::getInstance()->processTimers(20);
    DspSuperTestSupport::fireInterrupt({11.5, 2, 40}, dspInterface);
    TimeMachineForTest::getInstance()->processTimers(20);
    DspSuperTestSupport::fireInterrupt({11.5, 2, 40}, dspInterface);

    constexpr double expectedBusy = (12.5+11.5)/2;

    TimeMachineForTest::getInstance()->processTimers(2000);
    double busy = testRunner.getVfComponent(statusEntityId, "INF_DSP_BUSY").toDouble();
    QCOMPARE(busy, expectedBusy);
    quint32 period = testRunner.getVfComponent(statusEntityId, "INF_DSP_PERIOD_COUNT").toUInt();
    QCOMPARE(period, 2);
    quint32 timeMs = testRunner.getVfComponent(statusEntityId, "INF_DSP_MS_TIMER").toUInt();
    QCOMPARE(timeMs, 40);

    // Do vein values remain on no DSP interrupts?
    TimeMachineForTest::getInstance()->processTimers(1000);
    busy = testRunner.getVfComponent(statusEntityId, "INF_DSP_BUSY").toDouble();
    QCOMPARE(busy, expectedBusy);
    period = testRunner.getVfComponent(statusEntityId, "INF_DSP_PERIOD_COUNT").toUInt();
    QCOMPARE(period, 2);
    timeMs = testRunner.getVfComponent(statusEntityId, "INF_DSP_MS_TIMER").toUInt();
    QCOMPARE(timeMs, 40);
}
