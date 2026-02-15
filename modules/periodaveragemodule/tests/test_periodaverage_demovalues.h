#ifndef TEST_PERIODAVERAGE_DEMOVALUES_H
#define TEST_PERIODAVERAGE_DEMOVALUES_H

#include <QObject>

class test_periodaverage_demovalues : public QObject
{
    Q_OBJECT
private slots:
    void addVoltage();
private:
    float getDspPeriodValue(const QString &channelMName, int period, const QVector<float> &dspValues);
    float getDspAvgValue(const QString &channelMName, const QVector<float> &dspValues);

    static const QStringList mtChannelLayout;
};

#endif // TEST_DEMOVALUESDSPPERIODAVERAGE_H
