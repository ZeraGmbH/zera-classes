#ifndef DEMODSPINTERFACESAMPLE_H
#define DEMODSPINTERFACESAMPLE_H

#include <mockdspinterface.h>
#include <timertemplateqt.h>

class DemoDspInterfaceSample : public MockDspInterface
{
    Q_OBJECT
public:
    DemoDspInterfaceSample(QStringList valueChannelList,
                           std::function<double()> valueGenerator);
private slots:
    void onTimer();
private:
    QStringList m_valueChannelList;
    TimerTemplateQtPtr m_periodicTimer;
    std::function<double()> m_valueGenerator;
};

#endif // DEMODSPINTERFACESAMPLE_H
