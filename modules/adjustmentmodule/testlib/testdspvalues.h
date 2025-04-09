#ifndef TESTDSPVALUES_H
#define TESTDSPVALUES_H

#include "demovaluesdspdft.h"
#include "demovaluesdspfft.h"
#include "demovaluesdsprange.h"
#include "demovaluesdsprms.h"
#include <mockdspinterface.h>
#include <memory>

class TestDspValues
{
public:
    TestDspValues(QStringList valueChannelList);
    void setAllValuesSymmetric(float voltage, float current, float angleUi, float frequency, bool invertedSequence = false);
    void setAllValuesSymmetricAc(float voltage, float current, float angleUi, float frequency, bool invertedSequence = false);
    void setAllValuesSymmetricDc(float voltage, float current);

    void fireDftActualValues(MockDspInterfacePtr dspDft);
    void fireFftActualValues(MockDspInterfacePtr dspFft);
    void fireRangeActualValues(MockDspInterfacePtr dspRange);
    void fireRmsActualValues(MockDspInterfacePtr dspRms);

    void fireAllActualValues(MockDspInterfacePtr dspDft,
                             MockDspInterfacePtr dspFft,
                             MockDspInterfacePtr dspRange,
                             MockDspInterfacePtr dspRms);
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

#endif // TESTDSPVALUES_H
