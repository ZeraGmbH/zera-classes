#include "demovaluesdsppower1.h"

DemoValuesDspPower1::DemoValuesDspPower1(int entityId, std::function<double (int)> valueGenerator, MeasModeSelector *measMode) :
    m_entityId(entityId),
    m_valueGenerator(valueGenerator),
    m_measMode(measMode)
{
}

void DemoValuesDspPower1::setAllValuesSymmetric(float voltage, float current)
{
    QVector<float> valuesDemo;
    std::shared_ptr<MeasMode> mode = m_measMode->getCurrMode();
    QString modeName = mode->getName();
    bool is3Wire = modeName.startsWith('3');
    QString phaseMask = mode->getCurrentMask();
    for(int phase=0; phase<MeasPhaseCount; phase++) {
        double val = 0.0;
        if(phaseMask.count() > phase && phaseMask[phase] == '1') {
            double randPlusMinusOne = 2.0 * m_valueGenerator(m_entityId) - 1.0;
            val = current*voltage + randPlusMinusOne;
            if(is3Wire)
                val *= 1.5;
        }
        valuesDemo.append(val);
    }
    valuesDemo.append(valuesDemo[0] + valuesDemo[1] + valuesDemo[2]);
    m_powerValues = valuesDemo;
}

QVector<float> DemoValuesDspPower1::getDspValues() const
{
    return m_powerValues;
}
