#include "test_demovaluesdspdft.h"
#include "demovaluesdspdft.h"
#include <QTest>

QTEST_MAIN(test_demovaluesdspdft)

const QStringList test_demovaluesdspdft::mtRmsLayout = QStringList() << "m0" << "m1" << "m2" << "m0-m1" << "m2-m1" << "m2-m0" << "m3" << "m4" << "m5" << "m6" << "m7";

void test_demovaluesdspdft::addVoltage()
{
    DemoValuesDspDft dftValues(mtRmsLayout, 1);
    dftValues.setValue("m0", std::complex<float>(42, 0));

    QVector<float> dspValue = dftValues.getDspValues();
    QCOMPARE(getValue("m0", dspValue), std::complex<float>(42, 0));
    QCOMPARE(getValue("m1", dspValue), std::complex<float>(0, 0));
    QCOMPARE(getValue("m2", dspValue), std::complex<float>(0, 0));

    dftValues.setValue("m1", std::complex<float>(47, 11));
    dspValue = dftValues.getDspValues();
    QCOMPARE(getValue("m0", dspValue), std::complex<float>(42, 0));
    QCOMPARE(getValue("m1", dspValue), std::complex<float>(47, 11));
    QCOMPARE(getValue("m2", dspValue), std::complex<float>(0, 0));
}

void test_demovaluesdspdft::addCurrent()
{
    DemoValuesDspDft dftValues(mtRmsLayout, 1);
    dftValues.setValue("m3", std::complex<float>(42, 0));

    QVector<float> dspValue = dftValues.getDspValues();
    QCOMPARE(getValue("m3", dspValue), std::complex<float>(42, 0));
    QCOMPARE(getValue("m4", dspValue), std::complex<float>(0, 0));
    QCOMPARE(getValue("m5", dspValue), std::complex<float>(0, 0));

    dftValues.setValue("m4", std::complex<float>(47, 11));
    dspValue = dftValues.getDspValues();
    QCOMPARE(getValue("m3", dspValue), std::complex<float>(42, 0));
    QCOMPARE(getValue("m4", dspValue), std::complex<float>(47, 11));
    QCOMPARE(getValue("m5", dspValue), std::complex<float>(0, 0));
}

void test_demovaluesdspdft::setSymmetric()
{

}

std::complex<float> test_demovaluesdspdft::getValue(QString valueChannelName, QVector<float> dspValues)
{
    int idxValue = mtRmsLayout.indexOf(valueChannelName) * 2;
    std::complex<float> value(dspValues[idxValue], dspValues[idxValue+1]);
    return value;
}
