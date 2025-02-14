#include "test_power2_module_regression.h"
#include "modulemanagertestrunner.h"
#include <timemachineobject.h>
#include <testloghelpers.h>
#include <QTest>

QTEST_MAIN(test_power2_module_regression)

const int powerEntityId = 1090;

void test_power2_module_regression::dumpDspSetup()
{
    ModuleManagerTestRunner testRunner(":/session-minimal.json");
    TestDspInterfacePtr power2DspInterface = testRunner.getDspInterface(powerEntityId);

    QString measProgramDumped = TestLogHelpers::dump(power2DspInterface->dumpAll());
    QString measProgramExpected = TestLogHelpers::loadFile(":/dspDumps/dumpMeasProgram.json");
    QVERIFY(TestLogHelpers::compareAndLogOnDiff(measProgramExpected, measProgramDumped));
}
