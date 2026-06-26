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
    TestDspValues(const QStringList &valueChannelList, int dftOrder);
    void setAllValuesSymmetricAc(float voltage, float current, float angleUi, float frequency, bool invertedSequence = false);
    void setAllValuesSymmetricDc(float voltage, float current);

    void fireDftActualValues(const MockDspInterfacePtr &dspDft);
    void fireFftActualValues(const MockDspInterfacePtr &dspFft);
    void fireRangeActualValues(const MockDspInterfacePtr &dspRange);
    void fireRmsActualValues(const MockDspInterfacePtr &dspRms);

    void fireAllActualValues(const MockDspInterfacePtr &dspDft,
                             const MockDspInterfacePtr &dspFft,
                             const MockDspInterfacePtr &dspRange,
                             const MockDspInterfacePtr &dspRms);
private:
    QVector<float> getDspValuesDft();
    QVector<float> getDspValuesFft();
    QVector<float> getDspValuesRange();
    QVector<float> getDspValuesRms();
    // Why unique pointers: we need to inject params in ctors which have to be calculated first
    std::unique_ptr<DemoValuesDspDft> m_dftValues;
    std::unique_ptr<DemoValuesDspFft> m_fftValues;
    std::unique_ptr<DemoValuesDspRange> m_rangeValues;
    std::unique_ptr<DemoValuesDspRms> m_rmsValues;
    QStringList m_channelList;
};

#endif // TESTDSPVALUES_H
