#include "demormsdspvalues.h"
#include <math.h>

DemoRmsDspValues::DemoRmsDspValues(QStringList valueChannelList) :
    m_valueChannelList(valueChannelList)
{
    for(const auto &valueName : valueChannelList)
        m_rmsValues.insert(valueName, 0.0);
}

void DemoRmsDspValues::setValue(QString valueChannelName, float rmsValue)
{
    if(m_rmsValues.contains(valueChannelName))
        m_rmsValues[valueChannelName] = rmsValue;
}

void DemoRmsDspValues::setAllValuesSymmetric(float voltage, float current)
{
    float sqrt_3 = sqrt(3);
    for(auto iter=m_rmsValues.begin(); iter!=m_rmsValues.end(); iter++) {
        const QString valueChannelName = iter.key();
        if(valueChannelName.contains("-"))
            iter.value() = voltage * sqrt_3;
        else if(isPhaseNeutralVoltage(valueChannelName))
            iter.value() = voltage;
        else if(isCurrent(valueChannelName))
            iter.value() = current;
    }
}

QVector<float> DemoRmsDspValues::getDspValues()
{
    QVector<float> valueList;
    for(const auto &valueChannelName : qAsConst(m_valueChannelList))
        valueList.append(m_rmsValues[valueChannelName]);
    return valueList;
}

bool DemoRmsDspValues::isPhaseNeutralVoltage(QString valueChannelName)
{
    QStringList voltagePhases = QStringList() << "m0" << "m1" << "m2" << "m6";
    return voltagePhases.contains(valueChannelName);
}

bool DemoRmsDspValues::isCurrent(QString valueChannelName)
{
    QStringList currentPhases = QStringList() << "m3" << "m4" << "m5" << "m7";
    return currentPhases.contains(valueChannelName);
}

