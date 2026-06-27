#ifndef TESTDSPVALUES_H
#define TESTDSPVALUES_H

#include "demovaluesdspdft.h"
#include "demovaluesdspfft.h"
#include "demovaluesdsppower1.h"
#include "demovaluesdsprange.h"
#include "demovaluesdsprms.h"
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
    void firePower1ActualValues(QList<TestDspInterfacePtr> dspPower1);

    void fireAllActualValues(const MockDspInterfacePtr &dspDft,
                             const MockDspInterfacePtr &dspFft,
                             const MockDspInterfacePtr &dspRange,
                             const MockDspInterfacePtr &dspRms,
                             const QList<TestDspInterfacePtr> &dspPower1);
private:
    // Why unique pointers: we need to inject params in ctors which have to be calculated first
    std::unique_ptr<DemoValuesDspDft> m_dftValues;
    std::unique_ptr<DemoValuesDspFft> m_fftValues;
    std::unique_ptr<DemoValuesDspRange> m_rangeValues;
    std::unique_ptr<DemoValuesDspRms> m_rmsValues;
    std::unique_ptr<DemoValuesDspPower1> m_power1Values;
    QStringList m_channelList;
};

#endif // TESTDSPVALUES_H
