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

void test_measmodecatalog::reminderForThreeWire()
{
    int expectedMeasModeCount = 16;
    int expectedThreeWireModeCount = 2;

    int threeWireModeCount = 0;
    for(int modeId=MeasModeFirst; modeId<MeasModeCount; modeId++) {
        cMeasModeInfo info = MeasModeCatalog::getInfo(measmodes(modeId));
        if(info.isThreeWire())
            threeWireModeCount++;
    }

    if(MeasModeCount != expectedMeasModeCount || expectedThreeWireModeCount != threeWireModeCount)
        qWarning("Don't forget to update three wire mode into 'MeasModeCatalog::getThreeWireModes'.");
    QCOMPARE(MeasModeCount, expectedMeasModeCount);
    QCOMPARE(threeWireModeCount, expectedThreeWireModeCount);
}
