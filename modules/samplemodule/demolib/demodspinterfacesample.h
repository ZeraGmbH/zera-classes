#ifndef DEMODSPINTERFACESAMPLE_H
#define DEMODSPINTERFACESAMPLE_H

#include <mockdspinterface.h>
#include <timertemplateqt.h>

class DemoDspInterfaceSample : public MockDspInterface
{
    Q_OBJECT
public:
    DemoDspInterfaceSample(int entityId,
                           QStringList valueChannelList,
                           std::function<double(int)> valueGenerator);
private slots:
    void onTimer();
private:
    int m_entityId;
    QStringList m_valueChannelList;
    TimerTemplateQtPtr m_periodicTimer;
    std::function<double(int)> m_valueGenerator;
};

#endif // DEMODSPINTERFACESAMPLE_H
