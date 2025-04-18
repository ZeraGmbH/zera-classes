#ifndef DEMODSPINTERFACERANGE_H
#define DEMODSPINTERFACERANGE_H

#include <mockdspinterface.h>
#include <timertemplateqt.h>

class DemoDspInterfaceRange : public MockDspInterface
{
    Q_OBJECT
public:
    DemoDspInterfaceRange(QStringList valueChannelList,
                          bool isReference,
                          std::function<double()> valueGenerator);
private slots:
    void onTimer();
private:
    bool demoChannelIsVoltage(int channel);
    QVector<float> demoChannelRms();
    double demoFrequency();
    QStringList m_valueChannelList;
    bool m_isReference;
    TimerTemplateQtPtr m_periodicTimer;
    std::function<double()> m_valueGenerator;
};

#endif // DEMODSPINTERFACERANGE_H
