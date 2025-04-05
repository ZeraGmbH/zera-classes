#ifndef DEMODSPINTERFACEDFT_H
#define DEMODSPINTERFACEDFT_H

#include <mockdspinterface.h>
#include <timertemplateqt.h>

class DemoDspInterfaceDft : public MockDspInterface
{
    Q_OBJECT
public:
    enum ValueTypes {
        RotatingValues,
        FixedValues
    };
    DemoDspInterfaceDft(QStringList valueChannelList,
                        int dftOrder,
                        ValueTypes valueType);
private slots:
    void onTimer();
private:
    QStringList m_valueChannelList;
    int m_dftOrder;
    TimerTemplateQtPtr m_periodicTimer;
    int m_currentAngle = 0;
    ValueTypes m_valueType;
};

#endif // DEMODSPINTERFACEDFT_H
