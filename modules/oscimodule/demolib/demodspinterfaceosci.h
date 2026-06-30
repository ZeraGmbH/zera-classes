#ifndef DEMODSPINTERFACEOSCI_H
#define DEMODSPINTERFACEOSCI_H

#include <mockdspinterface.h>
#include <timertemplateqt.h>

class DemoDspInterfaceOsci : public MockDspInterface
{
    Q_OBJECT
public:
    DemoDspInterfaceOsci(int entityId,
                         const QStringList &valueChannelList,
                         int sampleCount,
                         std::function<double(int)> valueGenerator);
private slots:
    void onTimer();
private:
    int m_entityId;
    QStringList m_valueChannelList;
    int m_sampleCount;
    TimerTemplateQtPtr m_periodicTimer;
    std::function<double(int)> m_valueGenerator;
};

#endif // DEMODSPINTERFACEOSCI_H
