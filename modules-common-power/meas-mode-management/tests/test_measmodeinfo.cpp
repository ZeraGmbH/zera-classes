#include "test_measmodeinfo.h"
#include "measmodecatalog.h"
#include <QTest>

QTEST_MAIN(test_measmodeinfo)

void test_measmodeinfo::fourWireIsNotThreeWire()
{
    cMeasModeInfo info = MeasModeCatalog::getInfo("4LW");
    QVERIFY(!info.isThreeWire());
    info = MeasModeCatalog::getInfo(m4lw);
    QVERIFY(!info.isThreeWire());
}

void test_measmodeinfo::threeWireActiveIsThreeWire()
{
    cMeasModeInfo info = MeasModeCatalog::getInfo("3LW");
    QVERIFY(info.isThreeWire());
    info = MeasModeCatalog::getInfo(m3lw);
    QVERIFY(info.isThreeWire());
}

void test_measmodeinfo::threeWireReactiveIsThreeWire()
{
    cMeasModeInfo info = MeasModeCatalog::getInfo("3LB");
    QVERIFY(info.isThreeWire());
    info = MeasModeCatalog::getInfo(m3lb);
    QVERIFY(info.isThreeWire());
}

void test_measmodeinfo::threeWireApparentIsThreeWire()
{
    cMeasModeInfo info = MeasModeCatalog::getInfo("3LS");
    QVERIFY(info.isThreeWire());
    info = MeasModeCatalog::getInfo(m3ls);
    QVERIFY(info.isThreeWire());
}

