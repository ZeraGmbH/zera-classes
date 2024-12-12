#ifndef DEMODSPINTERFACETHDN_H
#define DEMODSPINTERFACETHDN_H

#include <mockdspinterface.h>
#include <timertemplateqt.h>

class DemoDspInterfaceThdn : public MockDspInterface
{
    Q_OBJECT
public:
    DemoDspInterfaceThdn(QStringList valueChannelList);
private slots:
    void onTimer();
private:
    QStringList m_valueChannelList;
    TimerTemplateQtPtr m_periodicTimer;
};

#endif // DEMODSPINTERFACETHDN_H
