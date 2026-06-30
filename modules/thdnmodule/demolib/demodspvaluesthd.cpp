#include "demodspvaluesthd.h"
#include <math.h>

DemoDspValuesThd::DemoDspValuesThd(const QStringList &valueChannelList) :
    m_valueChannelList(valueChannelList),
    m_valueGenerator([](int) { return 0.5; })
{
}

DemoDspValuesThd::DemoDspValuesThd(int entityId,
                                     const QStringList &valueChannelList,
                                     std::function<double(int)> valueGenerator) :
    m_entityId(entityId),
    m_valueChannelList(valueChannelList),
    m_valueGenerator(valueGenerator)
{
}

void DemoDspValuesThd::setAllValuesSymmetric()
{
    int valueCount = m_valueChannelList.count();
    QVector<float> demoValues(valueCount);
    // This has room for enhancements...
    for(int i=0; i<valueCount; ++i) {
        double randomVal = m_valueGenerator(m_entityId);
        double randomDeviation = 0.95 + 0.1 * randomVal;
        float val = (10+i) * randomDeviation;
        float thdr = val / sqrt(1 + (val * val));
        demoValues[i] = thdr;
    }
    m_dspValues = demoValues;
}

const QVector<float> &DemoDspValuesThd::getDspValues() const
{
    return m_dspValues;
}
