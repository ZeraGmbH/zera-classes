#ifndef DEMODSPVALUESPOWER1_H
#define DEMODSPVALUESPOWER1_H

#include "measmodeselector.h"
#include <QStringList>

class DemoDspValuesPower1
{
public:
    DemoDspValuesPower1();
    DemoDspValuesPower1(int entityId, std::function<double (int)> valueGenerator, MeasModeSelector *measMode);
    void setAllValuesSymmetric(float voltage, float current);
    QVector<float> getDspValues() const;
private:
    int m_entityId = 0;
    std::function<double (int)> m_valueGenerator;
    MeasModeSelector *m_measMode = nullptr;

    QVector<float> m_powerValues;
};

#endif // DEMODSPVALUESPOWER1_H
