#ifndef DEMODSPINTERFACERMS_H
#define DEMODSPINTERFACERMS_H

#include <mockdspinterface.h>
#include <timertemplateqt.h>

class DemoDspInterfaceRms : public MockDspInterface
{
    Q_OBJECT
public:
    DemoDspInterfaceRms(int entityId,
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

#endif // DEMODSPINTERFACERMS_H
