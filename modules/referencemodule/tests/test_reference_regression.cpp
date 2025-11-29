#include "test_reference_regression.h"
#include "modulemanagertestrunner.h"
#include <testloghelpers.h>
#include <QTest>

QTEST_MAIN(test_reference_regression)

static int constexpr refEntityId = 1001;

void test_reference_regression::dumpDspSetup()
{
    ModuleManagerTestRunner testRunner(":/session-minimal.json", false, "com5003");

    TestDspInterfacePtr power2DspInterfaceAdj = testRunner.getDspInterface(refEntityId, ADJUST);
    QVERIFY(power2DspInterfaceAdj);
    TestDspInterfacePtr power2DspInterfaceProg = testRunner.getDspInterface(refEntityId, MODULEPROG);

    QString measProgramDumped = TestLogHelpers::dump(power2DspInterfaceProg->dumpAll(true));
    QString measProgramExpected = TestLogHelpers::loadFile(":/dspDumps/dumpMeasProgram.json");
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJson(measProgramExpected, measProgramDumped));
}
