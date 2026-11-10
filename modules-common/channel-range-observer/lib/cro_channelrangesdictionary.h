#ifndef CRO_CHANNELRANGESDICTIONARY_H
#define CRO_CHANNELRANGESDICTIONARY_H

#include "cro_range.h"

namespace ChannelRangeObserver {

class ChannelRangesDictionary
{
public:
    RangePtr findRangeByAlias(const QString &rangeAlias) const;
    const QStringList &getAvailableRangeAliases() const;
    const QStringList &getAllRangeAliases() const;

    void addMaidenRange(RangePtr range);
    void doAllRangesCompleteJobs();

    void clear();

private:
    void fillAliasToRangeHash();
    void fillSortedAliasList();
    void fillAvailAliasList();

    QList<RangePtr> m_rangesAddedBareThenFilled;
    QHash<QString, RangePtr> m_rangeAliasToRange;
    QStringList m_allRangeAliasListSorted;
    QStringList m_availRangeAliasListSorted;
};

}
#endif // CRO_CHANNELRANGESDICTIONARY_H
