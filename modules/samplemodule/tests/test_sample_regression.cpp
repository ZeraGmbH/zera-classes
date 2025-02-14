#include "test_sample_regression.h"
#include "modulemanagertestrunner.h"
#include <vs_dumpjson.h>
#include <testloghelpers.h>
#include <QTest>

QTEST_MAIN(test_sample_regression)

static int constexpr sampleEntityId = 1030;

void test_sample_regression::dumpDspSetup()
{
    ModuleManagerTestRunner testRunner(":/session-minimal.json");
    TestDspInterfacePtr sampleDspInterface = testRunner.getDspInterface(sampleEntityId);

    QString measProgramDumped = TestLogHelpers::dump(sampleDspInterface->dumpAll(true));
    QString measProgramExpected = TestLogHelpers::loadFile(":/dspDumps/dumpMeasProgram.json");
    QVERIFY(TestLogHelpers::compareAndLogOnDiff(measProgramExpected, measProgramDumped));
}
