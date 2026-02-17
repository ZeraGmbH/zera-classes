#include "demovaluesdspperiodaverage.h"

DemoValuesDspPeriodAverage::DemoValuesDspPeriodAverage(QStringList channelMNameList,
                                                       int maxPeriodCount, int periodCount) :
    m_channelMNameList(channelMNameList),
    m_maxPeriodCount(maxPeriodCount),
    m_periodCount(periodCount)
{
    for(const auto &valueName : channelMNameList)
        m_values.insert(valueName, QVector<double>(m_maxPeriodCount));
}

void DemoValuesDspPeriodAverage::setValue(QString channelMName, int period, double value)
{
    if(m_values.contains(channelMName))
        m_values[channelMName][period] = value;
}

QVector<float> DemoValuesDspPeriodAverage::getDspValues()
{
    int channelCount = m_channelMNameList.count();
    QVector<float> valueList;
    QVector<double> avgList(channelCount);
    for (int periodNo=0; periodNo<m_maxPeriodCount; ++periodNo) {
        for (int channelNo=0; channelNo<channelCount; ++channelNo) {
            const QString &channelMName = m_channelMNameList[channelNo];
            double value = m_values[channelMName][periodNo];
            valueList.append(value);
            if (periodNo < m_periodCount)
                avgList[channelNo] += value;
        }
    }
    for (int channelNo=0; channelNo<channelCount; ++channelNo) {
        double avg = avgList[channelNo] / double(m_periodCount);
        valueList.append(avg);
    }
    return valueList;
}
