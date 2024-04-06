#include "test_burden1_module_regression.h"
#include "demovaluesdspdft.h"
#include "modulemanagertestrunner.h"
#include "burden1measdelegate.h"
#include <timemachineobject.h>
#include <testdumpreporter.h>
#include <QBuffer>
#include <QTest>

QTEST_MAIN(test_burden1_module_regression)

static int constexpr dftEntityId = 1050;
static int constexpr dftBurdenCurrentId = 1160;
static int constexpr dftBurdenVoltageId = 1161;

void test_burden1_module_regression::minimalSession()
{
    ModuleManagerTestRunner testRunner(":/session-minimal.json");
    VeinEvent::StorageSystem* veinStorage = testRunner.getVeinStorageSystem();
    QList<int> entityList = veinStorage->getEntityList();
    QCOMPARE(entityList.count(), 4);
    QVERIFY(veinStorage->hasEntity(dftEntityId));
    QVERIFY(veinStorage->hasEntity(dftBurdenCurrentId));
    QVERIFY(veinStorage->hasEntity(dftBurdenVoltageId));
}

void test_burden1_module_regression::veinDumpInitial()
{
    QFile file(":/dumpInitial.json");
    QVERIFY(file.open(QFile::ReadOnly));
    QByteArray jsonExpected = file.readAll();

    ModuleManagerTestRunner testRunner(":/session-minimal.json");
    VeinEvent::StorageSystem* veinStorage = testRunner.getVeinStorageSystem();
    QByteArray jsonDumped;
    QBuffer buff(&jsonDumped);
    veinStorage->dumpToFile(&buff, QList<int>() << dftBurdenCurrentId << dftBurdenVoltageId);

    QVERIFY(TestDumpReporter::compareAndLogOnDiff(jsonExpected, jsonDumped));
}

void test_burden1_module_regression::resistanceCalculation()
{
    constexpr double conductanceCopper = 56.0; // from doc

    double length = 100.0;
    double crossSection = 2.5;
    double expected = length / (crossSection * conductanceCopper); // from doc
    QCOMPARE(BURDEN1MODULE::cBurden1MeasDelegate::calcWireResistence(length, crossSection), expected);

    length = 10.0;
    crossSection = 10.0;
    expected = length / (crossSection * conductanceCopper);
    QCOMPARE(BURDEN1MODULE::cBurden1MeasDelegate::calcWireResistence(length, crossSection), expected);
}

void test_burden1_module_regression::voltageBurden230V1ADefaultSettings()
{
    ModuleManagerTestRunner testRunner(":/session-minimal.json");

    const QList<TestDspInterfacePtr>& dspInterfaces = testRunner.getDspInterfaceList();
    QCOMPARE(dspInterfaces.count(), 1);
    QStringList valueList = dspInterfaces[0]->getValueList();
    DemoValuesDspDft dspValues(valueList, 1);
    constexpr double voltage = 230;
    constexpr double current = 5;
    constexpr double angle = 0;
    dspValues.setAllValuesSymmetric(voltage, current, angle);
    dspInterfaces[0]->fireActValInterrupt(dspValues.getDspValues(), 0);
    TimeMachineObject::feedEventLoop();

    VeinEvent::StorageSystem* veinStorage = testRunner.getVeinStorageSystem();

    QVariant burden;
    burden = veinStorage->getStoredValue(dftBurdenVoltageId, "ACT_Burden1");
    QCOMPARE(burden.toFloat(), calcUBurdenSb(voltage, current, 1, 1, 1));
    burden = veinStorage->getStoredValue(dftBurdenVoltageId, "ACT_Burden2");
    QCOMPARE(burden.toFloat(), calcUBurdenSb(voltage, current, 1, 1, 1));
    burden = veinStorage->getStoredValue(dftBurdenVoltageId, "ACT_Burden3");
    QCOMPARE(burden.toFloat(), calcUBurdenSb(voltage, current, 1, 1, 1));

    QVariant burdenRel;
    burdenRel = veinStorage->getStoredValue(dftBurdenVoltageId, "ACT_Ratio1");
    QCOMPARE(burdenRel.toFloat(), calcUBurdenSn(1, voltage, current, 1, 1, 1));
    burdenRel = veinStorage->getStoredValue(dftBurdenVoltageId, "ACT_Ratio2");
    QCOMPARE(burdenRel.toFloat(), calcUBurdenSn(1, voltage, current, 1, 1, 1));
    burdenRel = veinStorage->getStoredValue(dftBurdenVoltageId, "ACT_Ratio3");
    QCOMPARE(burdenRel.toFloat(), calcUBurdenSn(1, voltage, current, 1, 1, 1));

    QVariant calcAngle;
    calcAngle = veinStorage->getStoredValue(dftBurdenVoltageId, "ACT_PFactor1");
    QCOMPARE(calcAngle.toFloat(), 1.0);
    calcAngle = veinStorage->getStoredValue(dftBurdenVoltageId, "ACT_PFactor2");
    QCOMPARE(calcAngle.toFloat(), 1.0);
    calcAngle = veinStorage->getStoredValue(dftBurdenVoltageId, "ACT_PFactor3");
    QCOMPARE(calcAngle.toFloat(), 1.0);
}

void test_burden1_module_regression::voltageBurden100V1A60DefaultSettings()
{
    ModuleManagerTestRunner testRunner(":/session-minimal.json");

    const QList<TestDspInterfacePtr>& dspInterfaces = testRunner.getDspInterfaceList();
    QStringList valueList = dspInterfaces[0]->getValueList();
    DemoValuesDspDft dspValues(valueList, 1);
    constexpr double voltage = 100;
    constexpr double current = 1;
    constexpr double angle = 60;
    dspValues.setAllValuesSymmetric(voltage, current, angle);
    dspInterfaces[0]->fireActValInterrupt(dspValues.getDspValues(), 0);
    TimeMachineObject::feedEventLoop();

    VeinEvent::StorageSystem* veinStorage = testRunner.getVeinStorageSystem();

    QVariant burden;
    burden = veinStorage->getStoredValue(dftBurdenVoltageId, "ACT_Burden1");
    QCOMPARE(burden.toFloat(), calcUBurdenSb(voltage, current, 1, 1, 1));
    burden = veinStorage->getStoredValue(dftBurdenVoltageId, "ACT_Burden2");
    QCOMPARE(burden.toFloat(), calcUBurdenSb(voltage, current, 1, 1, 1));
    burden = veinStorage->getStoredValue(dftBurdenVoltageId, "ACT_Burden3");
    QCOMPARE(burden.toFloat(), calcUBurdenSb(voltage, current, 1, 1, 1));

    QVariant burdenRel;
    burdenRel = veinStorage->getStoredValue(dftBurdenVoltageId, "ACT_Ratio1");
    QCOMPARE(burdenRel.toFloat(), calcUBurdenSn(1, voltage, current, 1, 1, 1));
    burdenRel = veinStorage->getStoredValue(dftBurdenVoltageId, "ACT_Ratio2");
    QCOMPARE(burdenRel.toFloat(), calcUBurdenSn(1, voltage, current, 1, 1, 1));
    burdenRel = veinStorage->getStoredValue(dftBurdenVoltageId, "ACT_Ratio3");
    QCOMPARE(burdenRel.toFloat(), calcUBurdenSn(1, voltage, current, 1, 1, 1));

    QVariant calcAngle;
    calcAngle = veinStorage->getStoredValue(dftBurdenVoltageId, "ACT_PFactor1");
    QCOMPARE(calcAngle.toFloat(), calcCos(angle));
    calcAngle = veinStorage->getStoredValue(dftBurdenVoltageId, "ACT_PFactor2");
    QCOMPARE(calcAngle.toFloat(), calcCos(angle));
    calcAngle = veinStorage->getStoredValue(dftBurdenVoltageId, "ACT_PFactor3");
    QCOMPARE(calcAngle.toFloat(), calcCos(angle));
}

float test_burden1_module_regression::calcUBurdenSb(double voltage, double current, double nominalVoltage, double wireLen, double crossSection)
{
    double cableResistance = BURDEN1MODULE::cBurden1MeasDelegate::calcWireResistence(wireLen, crossSection);
    double nominator = current * nominalVoltage * nominalVoltage;
    double denominator = voltage + (current * cableResistance);
    return nominator / denominator;
}

float test_burden1_module_regression::calcUBurdenSn(double nominalBurden, double voltage, double current, double nominalVoltage, double wireLen, double crossSection)
{
    double burden = calcUBurdenSb(voltage, current, nominalVoltage, wireLen, crossSection);
    return burden * 100.0 / nominalBurden;
}

float test_burden1_module_regression::calcCos(double angle)
{
    return cos(angle * M_PI / 180.0);
}
