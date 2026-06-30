#ifndef DEMODSPVALUESRMS_H
#define DEMODSPVALUESRMS_H

#include <QVector>
#include <QMap>
#include <QStringList>

class DemoDspValuesRms
{
public:
    DemoDspValuesRms(const QStringList &valueChannelList);
    void setValue(QString valueChannelName, float rmsValue);
    void setAllValuesSymmetric(float voltage, float current);
    QVector<float> getDspValues();
private:
    QStringList m_valueChannelList;
    QMap<QString, float> m_rmsValues;
};

#endif // DEMODSPVALUESRMS_H
