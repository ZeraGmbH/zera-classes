#include "test_thdn_module_regression.h"
#include "modulemanagertestrunner.h"
#include <testloghelpers.h>
#include <QTest>

QTEST_MAIN(test_thdn_module_regression)

static int constexpr thdnEntityId = 1110;

void test_thdn_module_regression::dumpDspSetup()
{
    ModuleManagerTestRunner testRunner(":/session-minimal.json");
    TestDspInterfacePtr osciDspInterface = testRunner.getDspInterface(thdnEntityId);

    QString measProgramDumped = TestLogHelpers::dump(osciDspInterface->dumpAll());
    QString measProgramExpected = TestLogHelpers::loadFile(":/dspDumps/dumpMeasProgram.json");
    QVERIFY(TestLogHelpers::compareAndLogOnDiff(measProgramExpected, measProgramDumped));
}
