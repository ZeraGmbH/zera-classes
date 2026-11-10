#include "cro_channelrangesdata.h"

namespace ChannelRangeObserver {

RangePtr ChannelRangesData::findRangeByName(const QString &rangeName) const
{
    auto iter = m_rangeNameToRange.constFind(rangeName);
    if(iter != m_rangeNameToRange.constEnd())
        return iter.value();
    return nullptr;
};

void ChannelRangesData::addRange(const QString &rangeName, RangePtr range)
{
    if (m_rangeNameToRange.contains(rangeName))
        qCritical("Range with name %s is overwritten!", qPrintable(rangeName));
    m_rangeNameToRange[rangeName] = range;
}

void ChannelRangesData::doAllRangesCompleteJobs()
{
    // TODO: Introduce range sort -> cRangeMeasChannel::setRangeListAlias()
    const QStringList &allRangeNamesOrderedByServer = *m_allRangeNamesOrderedByServer;
    for(const QString &rangeName : allRangeNamesOrderedByServer)
        if(m_rangeNameToRange[rangeName]->getAvailable() == true)
            m_availableRangeNames.append(rangeName);
}


void ChannelRangesData::clear()
{
    m_allRangeNamesOrderedByServer = std::make_shared<QStringList>();
    m_availableRangeNames.clear();
    m_rangeNameToRange.clear();
}

}