#ifndef DEMODSPINTERFACETHDN_H
#define DEMODSPINTERFACETHDN_H

#include <mockdspinterface.h>
#include <timertemplateqt.h>

class DemoDspInterfaceThdn : public MockDspInterface
{
    Q_OBJECT
public:
    DemoDspInterfaceThdn(int entityId,
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

#endif // DEMODSPINTERFACETHDN_H
