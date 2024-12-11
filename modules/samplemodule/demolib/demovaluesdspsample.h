#ifndef DEMOVALUESDSPSAMPLE_H
#define DEMOVALUESDSPSAMPLE_H

#include <QVector>
#include <QMap>
#include <QStringList>

class DemoValuesDspSample
{
public:
    DemoValuesDspSample(QStringList valueChannelList);
    void setValue(QString valueChannelName, float rmsValue);
    void setAllValuesSymmetric(float voltage, float current);
    QVector<float> getDspValues();
private:
    QStringList m_valueChannelList;
    QMap<QString, float> m_rmsValues;
};

#endif // DEMOVALUESDSPSAMPLE_H
