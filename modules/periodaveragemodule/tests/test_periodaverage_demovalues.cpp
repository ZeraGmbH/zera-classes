#include "test_periodaverage_demovalues.h"
#include "periodaveragemoduleconfiguration.h"
#include "demovaluesdspperiodaverage.h"
#include <QTest>

QTEST_MAIN(test_periodaverage_demovalues)

const QStringList test_periodaverage_demovalues::mtChannelLayout = QStringList() << "m0" << "m1" << "m2" << "m3" << "m4" << "m5" << "m6" << "m7";

void test_periodaverage_demovalues::addVoltage()
{
    DemoValuesDspPeriodAverage values(mtChannelLayout);
    values.setValue("m0", 0, 42);
    values.setValue("m0", 5, 49);
    values.setValue("m1", 1, 36);
    values.setValue("m1", 3, 47);

    QVector<float> dspValues = values.getDspValues();
    QCOMPARE(getDspPeriodValue("m0", 0, dspValues), 42);
    QCOMPARE(getDspPeriodValue("m0", 5, dspValues), 49);
    QCOMPARE(getDspAvgValue("m0", dspValues), float(double(42+49) / double(PERIODAVERAGEMODULE::MaxPeriods)));

    QCOMPARE(getDspPeriodValue("m1", 1, dspValues), 36);
    QCOMPARE(getDspPeriodValue("m1", 3, dspValues), 47);
    QCOMPARE(getDspAvgValue("m1", dspValues), float(double(36+47) / double(PERIODAVERAGEMODULE::MaxPeriods)));
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
                                                    const QVector<float> &dspValues)
{
    int channelCount = mtChannelLayout.count();
    int channelIdx = mtChannelLayout.indexOf(channelMName);
    int dspIndex = PERIODAVERAGEMODULE::MaxPeriods * channelCount + channelIdx;
    return dspValues[dspIndex];
}
