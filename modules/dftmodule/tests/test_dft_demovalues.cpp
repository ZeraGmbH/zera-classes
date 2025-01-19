#include "test_dft_demovalues.h"
#include "demovaluesdspdft.h"
#include <QTest>

QTEST_MAIN(test_dft_demovalues)

const QStringList test_dft_demovalues::mtRmsLayout = QStringList() << "m0" << "m1" << "m2" << "m0-m1" << "m2-m1" << "m2-m0" << "m3" << "m4" << "m5" << "m6" << "m7";

void test_dft_demovalues::addVoltage()
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

void test_dft_demovalues::addCurrent()
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

void test_dft_demovalues::setSymmetric()
{
    DemoValuesDspDft dftValues(mtRmsLayout, 1);
    const double voltage = 230;
    const double current = 5;
    const double angle = 30;

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

double test_dft_demovalues::gradToRad(double grad)
{
    return grad * M_PI / 180.0;
}

std::complex<float> test_dft_demovalues::getDspValue(QString valueChannelName, QVector<float> dspValues)
{
    int idxValue = mtRmsLayout.indexOf(valueChannelName) * 2;
    std::complex<float> value(dspValues[idxValue], dspValues[idxValue+1]);
    return value;
}

std::complex<float> test_dft_demovalues::getExpectedValue(double amplitude, double angle)
{
    std::complex<double> dblComplex = std::polar<double>(amplitude*M_SQRT2, gradToRad(-angle));
    std::complex<float> fltComplex = std::complex<float>(dblComplex.real(), dblComplex.imag());
    return fltComplex;
}
