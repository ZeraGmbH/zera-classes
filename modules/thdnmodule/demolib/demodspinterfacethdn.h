#ifndef DEMODSPINTERFACETHDN_H
#define DEMODSPINTERFACETHDN_H

#include <mockdspinterface.h>
#include <timertemplateqt.h>

class DemoDspInterfaceThdn : public MockDspInterface
{
    Q_OBJECT
public:
    DemoDspInterfaceThdn(QStringList valueChannelList,
                         std::function<double()> valueGenerator);
private slots:
    void onTimer();
private:
    QStringList m_valueChannelList;
    TimerTemplateQtPtr m_periodicTimer;
    std::function<double()> m_valueGenerator;
};

#endif // DEMODSPINTERFACETHDN_H
