#include "cro_channelrangesdictionary.h"

namespace ChannelRangeObserver {

RangePtr ChannelRangesDictionary::findRangeByName(const QString &rangeName) const
{
    auto iter = m_rangeNameToRange.constFind(rangeName);
    if(iter != m_rangeNameToRange.constEnd())
        return iter.value();
    return nullptr;
}

const QStringList &ChannelRangesDictionary::getAvailRangeNames() const
{
    return m_availRangeNameListSorted;
}

void ChannelRangesDictionary::addMaidenRange(RangePtr range)
{
    m_rangesAddedBareThenFilled.append(range);
}

void ChannelRangesDictionary::doAllRangesCompleteJobs()
{
    removeAvailableNotSet();
    fillNameToRangeHash();
    sortRanges();
}

void ChannelRangesDictionary::clear()
{
    m_rangesAddedBareThenFilled.clear();
    m_availRangeNameListSorted.clear();
    m_rangeNameToRange.clear();
}

void ChannelRangesDictionary::removeAvailableNotSet()
{
    QList<RangePtr> rangesAvailable;
    for (const RangePtr &range : qAsConst(m_rangesAddedBareThenFilled))
        if (range->getAvailable())
            rangesAvailable.append(range);
    m_rangesAddedBareThenFilled = rangesAvailable;
}

void ChannelRangesDictionary::fillNameToRangeHash()
{
    const QList<RangePtr> &rangesAddedBareThenFilled = m_rangesAddedBareThenFilled;
    for (const RangePtr &range : rangesAddedBareThenFilled)
        m_rangeNameToRange[range->getName()] = range;
}

void ChannelRangeObserver::ChannelRangesDictionary::sortRanges()
{
    // We found the pragmatic sort approach in rangemodule:
    // Note: The outer sort is on range type which is a binary mask. Whatsoever - it does what we expect
    QMap<quint32 /*type*/, QMap<double /*urValue*/, QString /*name*/>> sorter;
    const QList<RangePtr> &rangesAddedBareThenFilled = m_rangesAddedBareThenFilled;
    for (const RangePtr &range : rangesAddedBareThenFilled)
        sorter[-range->getType()][-range->getUrValue()] = range->getName(); // biggest ranges first

    QStringList sortedRangeNameList;
    for (auto iterType=sorter.constBegin(); iterType!=sorter.constEnd(); ++iterType) {
        const QMap<double /*urValue*/, QString /*name*/> &typeEntry = iterType.value();
        for (auto iterUr=typeEntry.constBegin(); iterUr!=typeEntry.constEnd(); ++iterUr)
            sortedRangeNameList.append(iterUr.value());
    }
    m_availRangeNameListSorted = sortedRangeNameList;
}

}