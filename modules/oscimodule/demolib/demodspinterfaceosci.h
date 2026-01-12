#ifndef DEMODSPINTERFACEOSCI_H
#define DEMODSPINTERFACEOSCI_H

#include <mockdspinterface.h>
#include <timertemplateqt.h>

class DemoDspInterfaceOsci : public MockDspInterface
{
    Q_OBJECT
public:
    DemoDspInterfaceOsci(int entityId,
                         QStringList valueChannelList,
                         int interpolation,
                         std::function<double(int)> valueGenerator);
private slots:
    void onTimer();
private:
    QVector<float> generateSineCurve(int sampleCount, int amplitude, float phase);

    int m_entityId;
    QStringList m_valueChannelList;
    int m_interpolation;
    TimerTemplateQtPtr m_periodicTimer;
    std::function<double(int)> m_valueGenerator;
};

#endif // DEMODSPINTERFACEOSCI_H
