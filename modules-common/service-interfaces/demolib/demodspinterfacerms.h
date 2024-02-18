#ifndef DEMODSPINTERFACERMS_H
#define DEMODSPINTERFACERMS_H

#include <mockdspinterface.h>
#include <timerperiodicqt.h>

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
    TimerPeriodicQt m_periodicTimer;
};

#endif // DEMODSPINTERFACERMS_H
