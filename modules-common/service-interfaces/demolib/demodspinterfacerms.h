#ifndef DEMODSPINTERFACERMS_H
#define DEMODSPINTERFACERMS_H

#include <mockdspinterface.h>
#include <timertemplateqt.h>

class DemoDspInterfaceRms : public MockDspInterface
{
    Q_OBJECT
public:
    DemoDspInterfaceRms(int interruptNoHandled, QStringList valueChannelList);
private slots:
    void onTimer();
private:
    int m_interruptNoHandled;
    QStringList m_valueChannelList;
    TimerTemplateQtPtr m_periodicTimer;
};

#endif // DEMODSPINTERFACERMS_H
