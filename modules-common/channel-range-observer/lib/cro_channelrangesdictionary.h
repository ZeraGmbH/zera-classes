#ifndef CRO_CHANNELRANGESDATA_H
#define CRO_CHANNELRANGESDATA_H

#include "cro_range.h"

namespace ChannelRangeObserver {

class ChannelRangesDictionary
{
public:
    RangePtr findRangeByName(const QString &rangeName) const;
    const QStringList &getAvailableRangeNames() const;
    const QStringList &getAllRangeNamesInServerOrder() const;

    void addRange(const QString &rangeName, RangePtr range);
    void doAllRangesCompleteJobs(const QStringList &allRangeNamesOrderedByServer);

    void clear();

private:
    QHash<QString, RangePtr> m_rangeNameToRange;
    QStringList m_allRangeNamesOrderedByServer;
    QStringList m_availableRangeNames;
};

}
#endif // CRO_CHANNELRANGESDATA_H
