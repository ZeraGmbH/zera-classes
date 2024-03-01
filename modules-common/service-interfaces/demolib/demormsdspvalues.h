#ifndef DEMORMSDSPVALUES_H
#define DEMORMSDSPVALUES_H

#include <QVector>
#include <QMap>
#include <QStringList>

class DemoRmsDspValues
{
public:
    DemoRmsDspValues(QStringList valueChannelList);
    void setValue(QString valueChannelName, float rmsValue);
    void setAllValuesSymmetric(float voltage, float current);
    QVector<float> getDspValues();
private:
    bool isPhaseNeutralVoltage(QString valueChannelName);
    bool isCurrent(QString valueChannelName);

    QStringList m_valueChannelList;
    QMap<QString, float> m_rmsValues;
};

#endif // DEMORMSDSPVALUES_H
