#ifndef DEMODSPINTERFACEPOWER2_H
#define DEMODSPINTERFACEPOWER2_H

#include <mockdspinterface.h>
#include <timertemplateqt.h>
#include "measmodeselector.h"

class DemoDspInterfacePower2 : public MockDspInterface
{
    Q_OBJECT
public:
    DemoDspInterfacePower2(MeasModeSelector* measMode,
                           std::function<double()> valueGenerator);
private slots:
    void onTimer();
private:
    MeasModeSelector *m_measMode;
    TimerTemplateQtPtr m_periodicTimer;
    std::function<double()> m_valueGenerator;
};

#endif // DEMODSPINTERFACEPOWER2_H
