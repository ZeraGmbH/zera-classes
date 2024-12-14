#include "test_sample_regression.h"
#include "modulemanagertestrunner.h"
#include <vs_dumpjson.h>
#include <testloghelpers.h>
#include <QTest>

QTEST_MAIN(test_sample_regression)

void test_sample_regression::dumpDspSetup()
{
    ModuleManagerTestRunner testRunner(":/session-minimal.json");

    const QList<TestDspInterfacePtr>& dspInterfaces = testRunner.getDspInterfaceList();
    QCOMPARE(dspInterfaces.count(), 1);

    QString measProgramDumped = TestLogHelpers::dump(dspInterfaces[0]->dumpAll());
    QString measProgramExpected = TestLogHelpers::loadFile(":/dspDumps/dumpMeasProgram.json");
    QVERIFY(TestLogHelpers::compareAndLogOnDiff(measProgramExpected, measProgramDumped));
}
