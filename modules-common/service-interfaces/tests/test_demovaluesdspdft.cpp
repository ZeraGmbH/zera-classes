#include "test_demovaluesdspdft.h"
#include "demovaluesdspdft.h"
#include <QTest>

QTEST_MAIN(test_demovaluesdspdft)

const QStringList test_demovaluesdspdft::mtRmsLayout = QStringList() << "m0" << "m1" << "m2" << "m0-m1" << "m2-m1" << "m2-m0" << "m3" << "m4" << "m5" << "m6" << "m7";

static constexpr float sqrt2 = float(M_SQRT2);

void test_demovaluesdspdft::addVoltage()
{
    DemoValuesDspDft dftValues(mtRmsLayout, 1);
    dftValues.setValue("m0", std::complex<float>(42, 0));

    QVector<float> dspValue = dftValues.getDspValues();
    QCOMPARE(getDspValue("m0", dspValue), std::complex<float>(42, 0));
    QCOMPARE(getDspValue("m1", dspValue), std::complex<float>(0, 0));
    QCOMPARE(getDspValue("m2", dspValue), std::complex<float>(0, 0));

    dftValues.setValue("m1", std::complex<float>(47, 11));
    dspValue = dftValues.getDspValues();
    QCOMPARE(getDspValue("m0", dspValue), std::complex<float>(42, 0));
    QCOMPARE(getDspValue("m1", dspValue), std::complex<float>(47, 11));
    QCOMPARE(getDspValue("m2", dspValue), std::complex<float>(0, 0));
}

void test_demovaluesdspdft::addCurrent()
{
    DemoValuesDspDft dftValues(mtRmsLayout, 1);
    dftValues.setValue("m3", std::complex<float>(42, 0));

    QVector<float> dspValue = dftValues.getDspValues();
    QCOMPARE(getDspValue("m3", dspValue), std::complex<float>(42, 0));
    QCOMPARE(getDspValue("m4", dspValue), std::complex<float>(0, 0));
    QCOMPARE(getDspValue("m5", dspValue), std::complex<float>(0, 0));

    dftValues.setValue("m4", std::complex<float>(47, 11));
    dspValue = dftValues.getDspValues();
    QCOMPARE(getDspValue("m3", dspValue), std::complex<float>(42, 0));
    QCOMPARE(getDspValue("m4", dspValue), std::complex<float>(47, 11));
    QCOMPARE(getDspValue("m5", dspValue), std::complex<float>(0, 0));
}

void test_demovaluesdspdft::setSymmetric()
{
    DemoValuesDspDft dftValues(mtRmsLayout, 1);
    const float voltage = 230;
    const float current = 5;
    const float angle = 30;

    dftValues.setAllValuesSymmetric(voltage, current, angle);
    QVector<float> dspValue = dftValues.getDspValues();

    QCOMPARE(getDspValue("m0", dspValue), getExpectedValue(voltage, 0));
    QCOMPARE(getDspValue("m1", dspValue), getExpectedValue(voltage, 120));
    QCOMPARE(getDspValue("m2", dspValue), getExpectedValue(voltage, 240));
    QCOMPARE(getDspValue("m6", dspValue), getExpectedValue(voltage, 0));

    QCOMPARE(getDspValue("m3", dspValue), getExpectedValue(current, 30));
    QCOMPARE(getDspValue("m4", dspValue), getExpectedValue(current, 150));
    QCOMPARE(getDspValue("m5", dspValue), getExpectedValue(current, 270));
    QCOMPARE(getDspValue("m7", dspValue), getExpectedValue(current, 30));

    // Currently demo does not create phase-phase module since
    // cDftModuleMeasProgram::turnVectorsToRefChannel() calculates phase-phase
    QCOMPARE(getDspValue("m0-m1", dspValue), std::complex<float>(0,0));
    QCOMPARE(getDspValue("m2-m1", dspValue), std::complex<float>(0,0));
    QCOMPARE(getDspValue("m2-m0", dspValue), std::complex<float>(0,0));
}

float test_demovaluesdspdft::gradToRad(float grad)
{
    return grad * float(M_PI) / 180.0;
}

std::complex<float> test_demovaluesdspdft::getDspValue(QString valueChannelName, QVector<float> dspValues)
{
    int idxValue = mtRmsLayout.indexOf(valueChannelName) * 2;
    std::complex<float> value(dspValues[idxValue], dspValues[idxValue+1]);
    return value;
}

std::complex<float> test_demovaluesdspdft::getExpectedValue(float amplitude, float angle)
{
    return std::polar<float>(amplitude*sqrt2, gradToRad(-angle));
}
