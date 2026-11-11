#ifndef CRO_CHANNELRANGESDICTIONARY_H
#define CRO_CHANNELRANGESDICTIONARY_H

#include "cro_range.h"

namespace ChannelRangeObserver {

class ChannelRangesDictionary
{
public:
    RangePtr findRangeByName(const QString &rangeName) const;
    const QStringList &getAvailableRangeNames() const;
    const QStringList &getAllRangeNames() const; // Is this really of interest or just causing confusion???

    void addMaidenRange(RangePtr range);
    void doAllRangesCompleteJobs();

    void clear();

private:
    void fillNameToRangeHash();
    void fillSortedNameList();
    void fillAvailNameList();

    QList<RangePtr> m_rangesAddedBareThenFilled;
    QHash<QString, RangePtr> m_rangeNameToRange;
    QStringList m_allRangeNameListSorted;
    QStringList m_availRangeNameListSorted;
};

}
#endif // CRO_CHANNELRANGESDICTIONARY_H
