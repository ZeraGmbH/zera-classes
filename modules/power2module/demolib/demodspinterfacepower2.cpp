#include "demodspinterfacepower2.h"
#include <timerfactoryqt.h>

DemoDspInterfacePower2::DemoDspInterfacePower2(MeasModeSelector *measMode,
                                               std::function<double()> valueGenerator) :
    m_measMode(measMode),
    m_periodicTimer(TimerFactoryQt::createPeriodic(500)),
    m_valueGenerator(valueGenerator)
{
    connect(m_periodicTimer.get(), &TimerTemplateQt::sigExpired,
            this, &DemoDspInterfacePower2::onTimer);
    connect(this, &DemoDspInterfacePower2::sigDspStarted, [&](){
        m_periodicTimer->start();
    });
}

void DemoDspInterfacePower2::onTimer()
{
    QVector<float> valuesDemo;
    int current = 10;
    int voltage = 230;
    constexpr int powerTypes = 3;
    double sumVal[powerTypes];
    for(int powerType=0; powerType < powerTypes; powerType++) {
        double phaseVal[MeasPhaseCount];
        for(int phase=0; phase<MeasPhaseCount; phase++) {
            double randPlusMinusOne = 2.0 * m_valueGenerator() - 1.0;
            phaseVal[phase] = current*voltage + randPlusMinusOne;
            valuesDemo.append(phaseVal[phase]);
        }
        sumVal[powerType] = phaseVal[0] + phaseVal[1] + phaseVal[2];
    }
    for(int powerType=0; powerType < powerTypes; powerType++)
        valuesDemo.append(sumVal[powerType]);

    fireActValInterrupt(valuesDemo, 7 /* that is what module expects currently */);
}
