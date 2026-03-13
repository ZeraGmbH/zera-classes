#ifndef DEMOVALUESDSPDSPSUPER_H
#define DEMOVALUESDSPDSPSUPER_H

#include <QStringList>
#include <QHash>
#include <QVector>

class DemoValuesDspDspSuper
{
public:
    DemoValuesDspDspSuper(QStringList channelMNameList, int maxPeriodCount, int periodCount);
    void setValue(QString channelMName, int period, double value);
    QVector<float> getDspValues();
private:
    QStringList m_channelMNameList;
    int m_maxPeriodCount;
    int m_periodCount;
    QHash<QString, QVector<double>> m_values;
};

#endif // DEMOVALUESDSPDSPSUPER_H
