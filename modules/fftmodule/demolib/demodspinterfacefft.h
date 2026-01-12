#ifndef DEMODSPINTERFACEFFT_H
#define DEMODSPINTERFACEFFT_H

#include <mockdspinterface.h>
#include <timertemplateqt.h>

class DemoDspInterfaceFft : public MockDspInterface
{
    Q_OBJECT
public:
    DemoDspInterfaceFft(int entityId,
                        QStringList valueChannelList,
                        int fftOrder,
                        std::function<double(int)> valueGenerator);
private slots:
    void onTimer();
private:
    int m_entityId;
    QStringList m_valueChannelList;
    int m_fftOrder;
    TimerTemplateQtPtr m_periodicTimer;
    std::function<double(int)> m_valueGenerator;
};

#endif // DEMODSPINTERFACEFFT_H
