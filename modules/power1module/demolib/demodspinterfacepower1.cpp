#include "demodspinterfacepower1.h"
#include <timerfactoryqt.h>

DemoDspInterfacePower1::DemoDspInterfacePower1(MeasModeSelector *measMode,
                                               std::function<double()> valueGenerator) :
    m_measMode(measMode),
    m_periodicTimer(TimerFactoryQt::createPeriodic(500)),
    m_valueGenerator(valueGenerator)
{
  connect(m_periodicTimer.get(), &TimerTemplateQt::sigExpired,
          this, &DemoDspInterfacePower1::onTimer);
  connect(this, &DemoDspInterfacePower1::sigDspStarted, [&](){
      m_periodicTimer->start();
  });
}

void DemoDspInterfacePower1::onTimer()
{
    QVector<float> valuesDemo;
    int current = 10;
    int voltage = 230;
    std::shared_ptr<MeasMode> mode = m_measMode->getCurrMode();
    QString modeName = mode->getName();
    bool is3Wire = modeName.startsWith('3');
    QString phaseMask = mode->getCurrentMask();
    for(int phase=0; phase<MeasPhaseCount; phase++) {
        double val = 0.0;
        if(phaseMask.count() > phase && phaseMask[phase] == '1') {
            double randPlusMinusOne = 2.0 * m_valueGenerator() - 1.0;
            val = current*voltage + randPlusMinusOne;
            if(is3Wire)
                val *= 1.5;
        }
        valuesDemo.append(val);
    }
    valuesDemo.append(valuesDemo[0] + valuesDemo[1] + valuesDemo[2]);
    fireActValInterrupt(valuesDemo,  /* dummy */ 0);
}
