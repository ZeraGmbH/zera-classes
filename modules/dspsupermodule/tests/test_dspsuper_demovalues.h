#ifndef TEST_DSPSUPER_DEMOVALUES_H
#define TEST_DSPSUPER_DEMOVALUES_H

#include <QObject>

class test_dspsuper_demovalues : public QObject
{
    Q_OBJECT
private slots:
    void addVoltage();
private:
    float getDspPeriodValue(const QString &channelMName, int period, const QVector<float> &dspValues);
    float getDspAvgValue(const QString &channelMName, int maxPeriods, const QVector<float> &dspValues);

    static const QStringList mtChannelLayout;
};

#endif // TEST_DSPSUPER_DEMOVALUES_H
