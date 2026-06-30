#include "demodspvaluespower1.h"

DemoDspValuesPower1::DemoDspValuesPower1() :
    m_valueGenerator([](int) { return 0.5; })
{
}

DemoDspValuesPower1::DemoDspValuesPower1(int entityId, std::function<double (int)> valueGenerator, MeasModeSelector *measMode) :
    m_entityId(entityId),
    m_valueGenerator(valueGenerator),
    m_measMode(measMode)
{
}

void DemoDspValuesPower1::setAllValuesSymmetric(float voltage, float current)
{
    QVector<float> valuesDemo;
    QString phaseMask = "111";
    bool is3Wire = false;
    if (m_measMode != nullptr) {
        std::shared_ptr<MeasMode> mode = m_measMode->getCurrMode();
        QString modeName = mode->getName();
        is3Wire = modeName.startsWith('3');
        phaseMask = mode->getCurrentMask();
    }
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
    m_dspValues = valuesDemo;
}

QVector<float> DemoDspValuesPower1::getDspValues() const
{
    return m_dspValues;
}
