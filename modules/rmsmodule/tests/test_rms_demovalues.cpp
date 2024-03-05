#include "test_rms_demovalues.h"
#include "demovaluesdsprms.h"
#include <QTest>
#include <math.h>

QTEST_MAIN(test_rms_demovalues)

const QStringList test_rms_demovalues::mtRmsLayout = QStringList() << "m0" << "m1" << "m2" << "m0-m1" << "m2-m1" << "m2-m0" << "m3" << "m4" << "m5" << "m6" << "m7";

void test_rms_demovalues::addVoltage()
{
    DemoValuesDspRms rmsValues(mtRmsLayout);
    rmsValues.setValue("m0", 42);

    QVector<float> dspValue = rmsValues.getDspValues();
    QCOMPARE(dspValue[mtRmsLayout.indexOf("m0")], 42);
    QCOMPARE(dspValue[mtRmsLayout.indexOf("m1")], 0);
    QCOMPARE(dspValue[mtRmsLayout.indexOf("m2")], 0);

    rmsValues.setValue("m1", 43);
    dspValue = rmsValues.getDspValues();
    QCOMPARE(dspValue[mtRmsLayout.indexOf("m0")], 42);
    QCOMPARE(dspValue[mtRmsLayout.indexOf("m1")], 43);
    QCOMPARE(dspValue[mtRmsLayout.indexOf("m2")], 0);
}

void test_rms_demovalues::addCurrent()
{
    DemoValuesDspRms rmsValues(mtRmsLayout);
    rmsValues.setValue("m3", 42);

    QVector<float> dspValue = rmsValues.getDspValues();
    QCOMPARE(dspValue[mtRmsLayout.indexOf("m3")], 42);
    QCOMPARE(dspValue[mtRmsLayout.indexOf("m4")], 0);
    QCOMPARE(dspValue[mtRmsLayout.indexOf("m5")], 0);

    rmsValues.setValue("m4", 43);
    dspValue = rmsValues.getDspValues();
    QCOMPARE(dspValue[mtRmsLayout.indexOf("m3")], 42);
    QCOMPARE(dspValue[mtRmsLayout.indexOf("m4")], 43);
    QCOMPARE(dspValue[mtRmsLayout.indexOf("m5")], 0);
}

void test_rms_demovalues::setSymmetric()
{
    DemoValuesDspRms rmsValues(mtRmsLayout);
    const float voltage = 230;
    const float current = 5;
    rmsValues.setAllValuesSymmetric(voltage, current);

    QVector<float> dspValue = rmsValues.getDspValues();
    QCOMPARE(dspValue[mtRmsLayout.indexOf("m0")], voltage);
    QCOMPARE(dspValue[mtRmsLayout.indexOf("m1")], voltage);
    QCOMPARE(dspValue[mtRmsLayout.indexOf("m2")], voltage);

    float sqrt_3 = sqrt(3);
    QCOMPARE(dspValue[mtRmsLayout.indexOf("m0-m1")], voltage*sqrt_3);
    QCOMPARE(dspValue[mtRmsLayout.indexOf("m2-m1")], voltage*sqrt_3);
    QCOMPARE(dspValue[mtRmsLayout.indexOf("m2-m0")], voltage*sqrt_3);

    QCOMPARE(dspValue[mtRmsLayout.indexOf("m3")], current);
    QCOMPARE(dspValue[mtRmsLayout.indexOf("m4")], current);
    QCOMPARE(dspValue[mtRmsLayout.indexOf("m5")], current);

    QCOMPARE(dspValue[mtRmsLayout.indexOf("m6")], voltage);
    QCOMPARE(dspValue[mtRmsLayout.indexOf("m7")], current);
}
