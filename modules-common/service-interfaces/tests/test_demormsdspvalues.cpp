#include "test_demormsdspvalues.h"
#include "demormsdspvalues.h"
#include <QTest>
#include <math.h>

QTEST_MAIN(test_demormsdspvalues)

const QStringList test_demormsdspvalues::mtRmsLayout = QStringList() << "m0" << "m1" << "m2" << "m0-m1" << "m2-m1" << "m2-m0" << "m3" << "m4" << "m5" << "m6" << "m7";

void test_demormsdspvalues::addVoltage()
{
    DemoRmsDspValues rmsValues(mtRmsLayout);
    rmsValues.setValue("m0", 42);

    QVector<float> dspValue = rmsValues.getDspValues();
    QCOMPARE(dspValue[0], 42);
    QCOMPARE(dspValue[1], 0);
    QCOMPARE(dspValue[2], 0);

    rmsValues.setValue("m1", 43);
    dspValue = rmsValues.getDspValues();
    QCOMPARE(dspValue[0], 42);
    QCOMPARE(dspValue[1], 43);
    QCOMPARE(dspValue[2], 0);
}

void test_demormsdspvalues::setSymmetric()
{
    DemoRmsDspValues rmsValues(mtRmsLayout);
    const float voltage = 230;
    const float current = 5;
    rmsValues.setAllValuesSymmetric(voltage, current);

    QVector<float> dspValue = rmsValues.getDspValues();
    QCOMPARE(dspValue[0], voltage);
    QCOMPARE(dspValue[1], voltage);
    QCOMPARE(dspValue[2], voltage);

    float sqrt_3 = sqrt(3);
    QCOMPARE(dspValue[3], voltage*sqrt_3);
    QCOMPARE(dspValue[4], voltage*sqrt_3);
    QCOMPARE(dspValue[5], voltage*sqrt_3);

    QCOMPARE(dspValue[6], current);
    QCOMPARE(dspValue[7], current);
    QCOMPARE(dspValue[8], current);

    QCOMPARE(dspValue[9], voltage);
    QCOMPARE(dspValue[10], current);
}
