#ifndef DEMOVALUESDSPADJUSTMENT_H
#define DEMOVALUESDSPADJUSTMENT_H

#include "demovaluesdspdft.h"
#include "demovaluesdspfft.h"
#include "demovaluesdsprange.h"
#include "demovaluesdsprms.h"
#include <mockdspinterface.h>
#include <memory>

class DemoValuesDspAdjustment
{
public:
    DemoValuesDspAdjustment(QStringList valueChannelList);
    void setAllValuesSymmetric(float voltage, float current, float angleUi, float frequency, bool invertedSequence = false);
    void fireActualValues(MockDspInterface* dspDft, MockDspInterface* dspFft, MockDspInterface* dspRange, MockDspInterface* dspRms);
private:
    QVector<float> getDspValuesDft();
    QVector<float> getDspValuesFft();
    QVector<float> getDspValuesRange();
    QVector<float> getDspValuesRms();
    std::unique_ptr<DemoValuesDspDft> m_dftValues;
    std::unique_ptr<DemoValuesDspFft> m_fftValues;
    std::unique_ptr<DemoValuesDspRange> m_rangeValues;
    std::unique_ptr<DemoValuesDspRms> m_rmsValues;
    QStringList m_channelList;
};

#endif // DEMOVALUESDSPADJUSTMENT_H
