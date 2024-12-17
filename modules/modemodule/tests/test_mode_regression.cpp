#include "test_mode_regression.h"
#include "modulemanagertestrunner.h"
#include <testloghelpers.h>
#include <QTest>

QTEST_MAIN(test_mode_regression)

void test_mode_regression::dumpDspSetup()
{
    ModuleManagerTestRunner testRunner(":/sessions/minimal.json");

    const QList<TestDspInterfacePtr>& dspInterfaces = testRunner.getDspInterfaceList();
    QCOMPARE(dspInterfaces.count(), 1);

    QString measProgramDumped = TestLogHelpers::dump(dspInterfaces[0]->dumpAll(true));
    QString measProgramExpected = TestLogHelpers::loadFile(":/dspDumps/dumpMeasProgram.json");
    QVERIFY(TestLogHelpers::compareAndLogOnDiff(measProgramExpected, measProgramDumped));
}
