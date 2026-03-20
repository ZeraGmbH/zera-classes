#ifndef DEMODSPINTERFACEDSPSUPER_H
#define DEMODSPINTERFACEDSPSUPER_H

#include <mockdspinterface.h>
#include <timertemplateqt.h>
#include <functional>

class DemoDspInterfaceDspSuper : public MockDspInterface
{
    Q_OBJECT
public:
    DemoDspInterfaceDspSuper(int entityId,
                             std::function<double(int)> valueGenerator);
private slots:
    void onTimer();
private:
    int genNextInterruptWithinPeriods() const;
    void sendInterrupt();

    int m_entityId = 0;
    int m_currPeriodCount = 0;
    int m_lastInterruptPeriod = 0;
    int m_nextInterruptPeriod = 0;
    std::function<double(int)> m_valueGenerator;
    TimerTemplateQtPtr m_periodicTimer;
};

#endif // DEMODSPINTERFACEDSPSUPER_H
