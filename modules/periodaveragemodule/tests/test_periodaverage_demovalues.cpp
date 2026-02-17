#include "test_periodaverage_demovalues.h"
#include "demovaluesdspperiodaverage.h"
#include <QTest>

QTEST_MAIN(test_periodaverage_demovalues)

const QStringList test_periodaverage_demovalues::mtChannelLayout = QStringList() << "m0" << "m1" << "m2" << "m3" << "m4" << "m5" << "m6" << "m7";

void test_periodaverage_demovalues::addVoltage()
{
    constexpr int maxPeriods = 10;
    constexpr int measPeriods = 5;
    DemoValuesDspPeriodAverage values(mtChannelLayout, maxPeriods, measPeriods);
    values.setValue("m0", 0, 42);
    values.setValue("m0", 5, 49);
    values.setValue("m1", 1, 36);
    values.setValue("m1", 3, 47);

    QVector<float> dspValues = values.getDspValues();
    QCOMPARE(getDspPeriodValue("m0", 0, dspValues), 42);
    QCOMPARE(getDspPeriodValue("m0", 5, dspValues), 49);
    QCOMPARE(getDspAvgValue("m0", maxPeriods, dspValues), float(double(42) / double(measPeriods))); // maybe academeic but 5,49 is out of meas period

    QCOMPARE(getDspPeriodValue("m1", 1, dspValues), 36);
    QCOMPARE(getDspPeriodValue("m1", 3, dspValues), 47);
    QCOMPARE(getDspAvgValue("m1", maxPeriods, dspValues), float(double(36+47) / double(measPeriods)));
}

float test_periodaverage_demovalues::getDspPeriodValue(const QString &channelMName,
                                                       int period,
                                                       const QVector<float> &dspValues)
{
    int channelCount = mtChannelLayout.count();
    int channelIdx = mtChannelLayout.indexOf(channelMName);
    int dspIndex = period * channelCount + channelIdx;
    return dspValues[dspIndex];
}

float test_periodaverage_demovalues::getDspAvgValue(const QString &channelMName,
                                                    int maxPeriods,
                                                    const QVector<float> &dspValues)
{
    int channelCount = mtChannelLayout.count();
    int channelIdx = mtChannelLayout.indexOf(channelMName);
    int dspIndex = maxPeriods * channelCount + channelIdx;
    return dspValues[dspIndex];
}
