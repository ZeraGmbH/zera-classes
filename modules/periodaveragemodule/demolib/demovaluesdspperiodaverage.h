#ifndef DEMOVALUESDSPPERIODAVERAGE_H
#define DEMOVALUESDSPPERIODAVERAGE_H

#include <QStringList>
#include <QHash>
#include <QVector>

class DemoValuesDspPeriodAverage
{
public:
    DemoValuesDspPeriodAverage(QStringList channelMNameList, int maxPeriodCount, int periodCount);
    void setValue(QString channelMName, int period, double value);
    QVector<float> getDspValues();
private:
    QStringList m_channelMNameList;
    int m_maxPeriodCount;
    int m_periodCount;
    QHash<QString, QVector<double>> m_values;
};

#endif // DEMOVALUESDSPPERIODAVERAGE_H
