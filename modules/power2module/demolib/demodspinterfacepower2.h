#ifndef DEMODSPINTERFACEPOWER2_H
#define DEMODSPINTERFACEPOWER2_H

#include <mockdspinterface.h>
#include <timertemplateqt.h>
#include "measmodeselector.h"

class DemoDspInterfacePower2 : public MockDspInterface
{
    Q_OBJECT
public:
    DemoDspInterfacePower2(int entityId,
                           MeasModeSelector* measMode,
                           std::function<double(int)> valueGenerator);
private slots:
    void onTimer();
private:
    int m_entityId;
    MeasModeSelector *m_measMode;
    TimerTemplateQtPtr m_periodicTimer;
    std::function<double(int)> m_valueGenerator;
};

#endif // DEMODSPINTERFACEPOWER2_H
