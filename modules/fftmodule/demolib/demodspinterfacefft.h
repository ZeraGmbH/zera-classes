#ifndef DEMODSPINTERFACEFFT_H
#define DEMODSPINTERFACEFFT_H

#include <mockdspinterface.h>
#include <timertemplateqt.h>

class DemoDspInterfaceFft : public MockDspInterface
{
    Q_OBJECT
public:
    DemoDspInterfaceFft(QStringList valueChannelList, int fftOrder);
private slots:
    void onTimer();
private:
    QStringList m_valueChannelList;
    int m_fftOrder;
    TimerTemplateQtPtr m_periodicTimer;
};

#endif // DEMODSPINTERFACEFFT_H
