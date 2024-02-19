#ifndef DEMODSPINTERFACEDFT_H
#define DEMODSPINTERFACEDFT_H

#include <mockdspinterface.h>
#include <timerperiodicqt.h>

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
    TimerPeriodicQt m_periodicTimer;
};

#endif // DEMODSPINTERFACEDFT_H
