#ifndef DEMODSPINTERFACERMS_H
#define DEMODSPINTERFACERMS_H

#include <mockdspinterface.h>
#include <timertemplateqt.h>

class DemoDspInterfaceRms : public MockDspInterface
{
    Q_OBJECT
public:
    DemoDspInterfaceRms(QStringList valueChannelList,
                        std::function<double()> valueGenerator);
private slots:
    void onTimer();
private:
    QStringList m_valueChannelList;
    TimerTemplateQtPtr m_periodicTimer;
    std::function<double()> m_valueGenerator;
};

#endif // DEMODSPINTERFACERMS_H
