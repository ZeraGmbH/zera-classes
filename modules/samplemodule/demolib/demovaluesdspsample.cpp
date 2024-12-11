#include "demovaluesdspsample.h"
#include "servicechannelnamehelper.h"
#include <math.h>

DemoValuesDspSample::DemoValuesDspSample(QStringList valueChannelList) :
    m_valueChannelList(valueChannelList)
{
    for(const auto &valueName : valueChannelList)
        m_rmsValues.insert(valueName, 0.0);
}

void DemoValuesDspSample::setValue(QString valueChannelName, float rmsValue)
{
    if(m_rmsValues.contains(valueChannelName))
        m_rmsValues[valueChannelName] = rmsValue;
}

void DemoValuesDspSample::setAllValuesSymmetric(float voltage, float current)
{
    float sqrt_3 = sqrt(3);
    for(auto iter=m_rmsValues.begin(); iter!=m_rmsValues.end(); iter++) {
        const QString valueChannelName = iter.key();
        if(valueChannelName.contains("-"))
            iter.value() = voltage * sqrt_3;
        else if(ServiceChannelNameHelper::isPhaseNeutralVoltage(valueChannelName))
            iter.value() = voltage;
        else if(ServiceChannelNameHelper::isCurrent(valueChannelName))
            iter.value() = current;
    }
}

QVector<float> DemoValuesDspSample::getDspValues()
{
    QVector<float> valueList;
    for(const auto &valueChannelName : qAsConst(m_valueChannelList))
        valueList.append(m_rmsValues[valueChannelName]);
    return valueList;
}
