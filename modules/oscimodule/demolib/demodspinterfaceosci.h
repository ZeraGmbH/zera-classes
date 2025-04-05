#ifndef DEMODSPINTERFACEOSCI_H
#define DEMODSPINTERFACEOSCI_H

#include <mockdspinterface.h>
#include <timertemplateqt.h>

class DemoDspInterfaceOsci : public MockDspInterface
{
    Q_OBJECT
public:
    DemoDspInterfaceOsci(QStringList valueChannelList,
                         int interpolation,
                         std::function<double()> valueGenerator);
private slots:
    void onTimer();
private:
    QVector<float> generateSineCurve(int sampleCount, int amplitude, float phase);
    QStringList m_valueChannelList;
    int m_interpolation;
    TimerTemplateQtPtr m_periodicTimer;
    std::function<double()> m_valueGenerator;
};

#endif // DEMODSPINTERFACEOSCI_H
