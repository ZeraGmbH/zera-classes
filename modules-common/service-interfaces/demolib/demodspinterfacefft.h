#ifndef DEMODSPINTERFACEFFT_H
#define DEMODSPINTERFACEFFT_H

#include <mockdspinterface.h>
#include <timerperiodicqt.h>

class DemoDspInterfaceFft : public MockDspInterface
{
    Q_OBJECT
public:
    DemoDspInterfaceFft(int interruptNoHandled, QStringList valueChannelList, int fftOrder);
private slots:
    void onTimer();
private:
    int m_interruptNoHandled;
    QStringList m_valueChannelList;
    int m_fftOrder;
    TimerPeriodicQt m_periodicTimer;
};

#endif // DEMODSPINTERFACEFFT_H
