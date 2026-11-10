#include "cro_channelrangesdictionary.h"

namespace ChannelRangeObserver {

RangePtr ChannelRangesDictionary::findRangeByAlias(const QString &rangeAlias) const
{
    auto iter = m_rangeAliasToRange.constFind(rangeAlias);
    if(iter != m_rangeAliasToRange.constEnd())
        return iter.value();
    return nullptr;
}

const QStringList &ChannelRangesDictionary::getAvailableRangeAliases() const
{
    return m_availRangeAliasListSorted;
}

const QStringList &ChannelRangesDictionary::getAllRangeAliases() const
{
    return m_allRangeAliasListSorted;
};

void ChannelRangesDictionary::addMaidenRange(RangePtr range)
{
    m_rangesAddedBareThenFilled.append(range);
}

void ChannelRangesDictionary::doAllRangesCompleteJobs()
{
    fillAliasToRangeHash();
    fillSortedAliasList();
    fillAvailAliasList();
}

void ChannelRangesDictionary::clear()
{
    m_rangesAddedBareThenFilled.clear();
    m_allRangeAliasListSorted.clear();
    m_availRangeAliasListSorted.clear();
    m_rangeAliasToRange.clear();
}

void ChannelRangesDictionary::fillAliasToRangeHash()
{
    const QList<RangePtr> &rangesAddedBareThenFilled = m_rangesAddedBareThenFilled;
    for (const RangePtr &range : rangesAddedBareThenFilled)
        m_rangeAliasToRange[range->getAlias()] = range;
}

void ChannelRangeObserver::ChannelRangesDictionary::fillSortedAliasList()
{
    QMap<quint32 /*type*/, QMap<double /*urValue*/, QString /*alias*/>> sorter;
    const QList<RangePtr> &rangesAddedBareThenFilled = m_rangesAddedBareThenFilled;
    for (const RangePtr &range : rangesAddedBareThenFilled)
        sorter[range->getType()][-range->getUrValue()] = range->getAlias(); // biggest ranges first

    QStringList sortedRangeAliasList;
    for (auto iterType=sorter.constBegin(); iterType!=sorter.constEnd(); ++iterType) {
        const QMap<double /*urValue*/, QString /*alias*/> &typeEntry = iterType.value();
        for (auto iterUr=typeEntry.constBegin(); iterUr!=typeEntry.constEnd(); ++iterUr)
            sortedRangeAliasList.append(iterUr.value());
    }
    m_allRangeAliasListSorted = sortedRangeAliasList;
}

void ChannelRangesDictionary::fillAvailAliasList()
{
    QStringList availRangeAliasListSorted;
    const QStringList &allRangeAliasListSorted = m_allRangeAliasListSorted;
    for(const QString &rangeAlias : allRangeAliasListSorted)
        if(m_rangeAliasToRange[rangeAlias]->getAvailable() == true)
            availRangeAliasListSorted.append(rangeAlias);
    m_availRangeAliasListSorted = availRangeAliasListSorted;
}

}