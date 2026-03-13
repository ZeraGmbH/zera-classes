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
                             int maxPeriodCount,
                             std::function<double(int)> valueGenerator);
private slots:
    void onTimer();
private:
    int m_entityId = 0;
    int m_maxPeriodCount = 0;
    std::function<double(int)> m_valueGenerator;
    TimerTemplateQtPtr m_periodicTimer;
};

#endif // DEMODSPINTERFACEDSPSUPER_H
