#ifndef CRO_CHANNELRANGESDICTIONARY_H
#define CRO_CHANNELRANGESDICTIONARY_H

#include "cro_range.h"

namespace ChannelRangeObserver {

class ChannelRangesDictionary
{
public:
    RangePtr findRangeByName(const QString &rangeName) const;
    const QStringList &getAvailRangeNames() const;

    void addMaidenRange(RangePtr range);
    void doAllRangesCompleteJobs();

    void clear();

private:
    void removeAvailableNotSet();
    void fillNameToRangeHash();
    void sortRanges();

    QList<RangePtr> m_rangesAddedBareThenFilled;
    QHash<QString, RangePtr> m_rangeNameToRange;
    QStringList m_availRangeNameListSorted;
};

}
#endif // CRO_CHANNELRANGESDICTIONARY_H
