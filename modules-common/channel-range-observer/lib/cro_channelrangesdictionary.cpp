#include "cro_channelrangesdictionary.h"

namespace ChannelRangeObserver {

RangePtr ChannelRangesDictionary::findRangeByName(const QString &rangeName) const
{
    auto iter = m_rangeNameToRange.constFind(rangeName);
    if(iter != m_rangeNameToRange.constEnd())
        return iter.value();
    return nullptr;
}

const QStringList &ChannelRangesDictionary::getAvailableRangeNames() const
{
    return m_availableRangeNames;
}

const QStringList &ChannelRangesDictionary::getAllRangeNamesInServerOrder() const
{
    return m_allRangeNamesOrderedByServer;
};

void ChannelRangesDictionary::addRange(const QString &rangeName, RangePtr range)
{
    if (m_rangeNameToRange.contains(rangeName))
        qCritical("Range with name %s is overwritten!", qPrintable(rangeName));
    m_rangeNameToRange[rangeName] = range;
}

void ChannelRangesDictionary::doAllRangesCompleteJobs(const QStringList &allRangeNamesOrderedByServer)
{
    m_allRangeNamesOrderedByServer = allRangeNamesOrderedByServer;
    for(const QString &rangeName : allRangeNamesOrderedByServer)
        if(m_rangeNameToRange[rangeName]->getAvailable() == true)
            m_availableRangeNames.append(rangeName);
    // TODO: Introduce range sort -> cRangeMeasChannel::setRangeListAlias()
}

void ChannelRangesDictionary::clear()
{
    m_allRangeNamesOrderedByServer.clear();
    m_availableRangeNames.clear();
    m_rangeNameToRange.clear();
}

}