#include "demovaluesdspperiodaverage.h"
#include "periodaveragemoduleconfiguration.h"

DemoValuesDspPeriodAverage::DemoValuesDspPeriodAverage(QStringList channelMNameList) :
    m_channelMNameList(channelMNameList)
{
    for(const auto &valueName : channelMNameList)
        m_values.insert(valueName, QVector<double>(PERIODAVERAGEMODULE::MaxPeriods));
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
    for (int periodNo=0; periodNo<PERIODAVERAGEMODULE::MaxPeriods; ++periodNo) {
        for (int channelNo=0; channelNo<channelCount; ++channelNo) {
            const QString &channelMName = m_channelMNameList[channelNo];
            double value = m_values[channelMName][periodNo];
            valueList.append(value);
            avgList[channelNo] += value;
        }
    }
    for (int channelNo=0; channelNo<channelCount; ++channelNo) {
        double avg = avgList[channelNo] / double(PERIODAVERAGEMODULE::MaxPeriods);
        valueList.append(avg);
    }
    return valueList;
}
