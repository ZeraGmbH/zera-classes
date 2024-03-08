#ifndef DEMODSPINTERFACEPOWER1_H
#define DEMODSPINTERFACEPOWER1_H

#include <mockdspinterface.h>
#include <timertemplateqt.h>
#include "measmodeselector.h"

class DemoDspInterfacePower1 : public MockDspInterface
{
    Q_OBJECT
public:
    DemoDspInterfacePower1(int interruptNoHandled, MeasModeSelector* measMode);
private slots:
    void onTimer();
private:
    int m_interruptNoHandled;
    MeasModeSelector *m_measMode;
    TimerTemplateQtPtr m_periodicTimer;

};

#endif // DEMODSPINTERFACEPOWER1_H
