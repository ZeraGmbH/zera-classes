#ifndef TESTDSPVALUES_H
#define TESTDSPVALUES_H

#include "demodspvaluesdft.h"
#include "demodspvaluesfft.h"
#include "demodspvaluespower1.h"
#include "demodspvaluesrange.h"
#include "demodspvaluesrms.h"
#include "demodspvaluesosci.h"
#include "testdspinterface.h"
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
    void firePower1ActualValues(const QList<TestDspInterfacePtr> &dspPower1);
    void fireOsciActualValues(const MockDspInterfacePtr &dspOsci);

    void fireAllActualValues(const MockDspInterfacePtr &dspDft,
                             const MockDspInterfacePtr &dspFft,
                             const MockDspInterfacePtr &dspRange,
                             const MockDspInterfacePtr &dspRms,
                             const QList<TestDspInterfacePtr> &dspPower1,
                             const MockDspInterfacePtr &dspOsci);
private:
    // Why unique pointers: we need to inject params in ctors which have to be calculated first
    std::unique_ptr<DemoDspValuesDft> m_dftValues;
    std::unique_ptr<DemoDspValuesFft> m_fftValues;
    std::unique_ptr<DemoDspValuesRange> m_rangeValues;
    std::unique_ptr<DemoDspValuesRms> m_rmsValues;
    std::unique_ptr<DemoDspValuesPower1> m_power1Values;
    std::unique_ptr<DemoDspValuesOsci> m_osciValues;
    QStringList m_channelList;
};

#endif // TESTDSPVALUES_H
