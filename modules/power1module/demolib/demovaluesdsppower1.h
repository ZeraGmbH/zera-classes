#ifndef DEMOVALUESDSPPOWER1_H
#define DEMOVALUESDSPPOWER1_H

#include "measmodeselector.h"
#include <QStringList>

class DemoValuesDspPower1
{
public:
    DemoValuesDspPower1();
    DemoValuesDspPower1(int entityId, std::function<double (int)> valueGenerator, MeasModeSelector *measMode);
    void setAllValuesSymmetric(float voltage, float current);
    QVector<float> getDspValues() const;
private:
    int m_entityId = 0;
    std::function<double (int)> m_valueGenerator;
    MeasModeSelector *m_measMode = nullptr;

    QVector<float> m_powerValues;
};

#endif // DEMOVALUESDSPPOWER1_H
