#include "test_sample_module_regression.h"
#include "modulemanagertestrunner.h"
#include <testloghelpers.h>
#include <QSignalSpy>
#include <QBuffer>
#include <QTest>

QTEST_MAIN(test_sample_module_regression)

static int constexpr sampleEntityId = 1030;


void test_sample_module_regression::dumpDspCyclicLists()
{
    ModuleManagerTestRunner testRunner(":/session-sample-test.json");

    const QList<TestDspInterfacePtr>& dspInterfaces = testRunner.getDspInterfaceList();
    QCOMPARE(dspInterfaces.count(), 3);

    QByteArray rangeCyclicListDumped = dspInterfaces[dspInterfaces::RangeObsermatic]->dumpCycListItem();
    QVERIFY(TestLogHelpers::compareAndLogOnDiff("", rangeCyclicListDumped));

    QByteArray adjustCyclicListDumped = dspInterfaces[dspInterfaces::AdjustManagement]->dumpCycListItem();
    QVERIFY(TestLogHelpers::compareAndLogOnDiff("", adjustCyclicListDumped));

    QByteArray rangeMeasCyclicListDumped = dspInterfaces[dspInterfaces::RangeModuleMeasProgram]->dumpCycListItem();
    QFile file(":/dumpDspCyclicRangeMeas.txt");
    QVERIFY(file.open(QFile::ReadOnly));
    QByteArray rangeListExpected = file.readAll();
    QVERIFY(TestLogHelpers::compareAndLogOnDiff(rangeListExpected, rangeMeasCyclicListDumped));

}
