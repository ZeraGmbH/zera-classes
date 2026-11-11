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
    return m_availRangeNameListSorted;
}

const QStringList &ChannelRangesDictionary::getAllRangeNames() const
{
    return m_allRangeNameListSorted;
}

void ChannelRangesDictionary::addMaidenRange(RangePtr range)
{
    m_rangesAddedBareThenFilled.append(range);
}

void ChannelRangesDictionary::doAllRangesCompleteJobs()
{
    fillNameToRangeHash();
    fillSortedNameList();
    fillAvailNameList();
}

void ChannelRangesDictionary::clear()
{
    m_rangesAddedBareThenFilled.clear();
    m_allRangeNameListSorted.clear();
    m_availRangeNameListSorted.clear();
    m_rangeNameToRange.clear();
}

void ChannelRangesDictionary::fillNameToRangeHash()
{
    const QList<RangePtr> &rangesAddedBareThenFilled = m_rangesAddedBareThenFilled;
    for (const RangePtr &range : rangesAddedBareThenFilled)
        m_rangeNameToRange[range->getName()] = range;
}

void ChannelRangeObserver::ChannelRangesDictionary::fillSortedNameList()
{
    QMap<quint32 /*type*/, QMap<double /*urValue*/, QString /*name*/>> sorter;
    const QList<RangePtr> &rangesAddedBareThenFilled = m_rangesAddedBareThenFilled;
    for (const RangePtr &range : rangesAddedBareThenFilled)
        sorter[range->getType()][-range->getUrValue()] = range->getName(); // biggest ranges first

    QStringList sortedRangeNameList;
    for (auto iterType=sorter.constBegin(); iterType!=sorter.constEnd(); ++iterType) {
        const QMap<double /*urValue*/, QString /*name*/> &typeEntry = iterType.value();
        for (auto iterUr=typeEntry.constBegin(); iterUr!=typeEntry.constEnd(); ++iterUr)
            sortedRangeNameList.append(iterUr.value());
    }
    m_allRangeNameListSorted = sortedRangeNameList;
}

void ChannelRangesDictionary::fillAvailNameList()
{
    QStringList availRangeNameListSorted;
    const QStringList &allRangeNameListSorted = m_allRangeNameListSorted;
    for(const QString &rangeName : allRangeNameListSorted)
        if(m_rangeNameToRange[rangeName]->getAvailable() == true)
            availRangeNameListSorted.append(rangeName);
    m_availRangeNameListSorted = availRangeNameListSorted;
}

}