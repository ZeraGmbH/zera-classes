#include "test_mode_regression.h"
#include "modulemanagertestrunner.h"
#include <testloghelpers.h>
#include <QTest>

QTEST_MAIN(test_mode_regression)

static int constexpr modeEntityId = 1000;

void test_mode_regression::dumpDspSetup()
{
    ModuleManagerTestRunner testRunner(":/sessions/minimal.json");
    TestDspInterfacePtr modeDspInterface = testRunner.getDspInterface(modeEntityId);

    QString measProgramDumped = TestLogHelpers::dump(modeDspInterface->dumpAll(true));
    QString measProgramExpected = TestLogHelpers::loadFile(":/dspDumps/dumpMeasProgram.json");
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJson(measProgramExpected, measProgramDumped));
}
