#ifndef DEMODSPINTERFACEPERIODAVERAGE_H
#define DEMODSPINTERFACEPERIODAVERAGE_H

#include <mockdspinterface.h>
#include <timertemplateqt.h>
#include <functional>

class DemoDspInterfacePeriodAverage : public MockDspInterface
{
    Q_OBJECT
public:
    DemoDspInterfacePeriodAverage(int entityId,
                                  QStringList valueChannelList,
                                  int maxPeriodCount, int periodCount,
                                  std::function<double(int)> valueGenerator);
private slots:
    void onTimer();
private:
    int m_entityId;
    QStringList m_valueChannelList;
    int m_maxPeriodCount;
    int m_periodCount;
    std::function<double(int)> m_valueGenerator;
    TimerTemplateQtPtr m_periodicTimer;
};

#endif // DEMODSPINTERFACEPERIODAVERAGE_H
