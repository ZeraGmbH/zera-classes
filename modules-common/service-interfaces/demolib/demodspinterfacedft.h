#ifndef DEMODSPINTERFACEDFT_H
#define DEMODSPINTERFACEDFT_H

#include <mockdspinterface.h>
#include <timertemplateqt.h>

class DemoDspInterfaceDft : public MockDspInterface
{
    Q_OBJECT
public:
    DemoDspInterfaceDft(int interruptNoHandled, QStringList valueChannelList);
private slots:
    void onTimer();
private:
    int m_interruptNoHandled;
    QStringList m_valueChannelList;
    TimerTemplateQtPtr m_periodicTimer;
};

#endif // DEMODSPINTERFACEDFT_H
