#include "test_measmodecatalog.h"
#include "measmodecatalog.h"
#include "measmodecatalogfortest.h"
#include <QTest>

QTEST_MAIN(test_measmodecatalog)

void test_measmodecatalog::invalidIdReturnsInvalid()
{
    cMeasModeInfo info = MeasModeCatalog::getInfo(measmodes(MeasModeCount));
    QVERIFY(!info.isValid());
}

void test_measmodecatalog::invalidStringReturnsInvalid()
{
    MeasModeCatalogForTest::enableTest();
    cMeasModeInfo info = MeasModeCatalog::getInfo("foo");
    QVERIFY(!info.isValid());
}

void test_measmodecatalog::allModesHaveACatalogEntry()
{
    for(int modeId=MeasModeFirst; modeId<MeasModeCount; modeId++) {
        cMeasModeInfo info = MeasModeCatalog::getInfo(measmodes(modeId));
        QVERIFY(info.isValid());
        qInfo("Mode %s checked", qPrintable(info.getName()));
    }
}
