#ifndef CRO_CHANNELRANGESDATA_H
#define CRO_CHANNELRANGESDATA_H

#include "cro_range.h"
#include <memory>

namespace ChannelRangeObserver {

class ChannelRangesData
{
public:
    RangePtr findRangeByName(const QString &rangeName) const;
    const QStringList &getAvailableRangeNames() const;

    const QStringList &getAllRangeNamesInServerOrder() const;

    void addRange(const QString &rangeName, RangePtr range);
    void doAllRangesCompleteJobs();

    void clear();

    std::shared_ptr<QStringList> m_allRangeNamesOrderedByServer = std::make_shared<QStringList>();

private:
    QStringList m_availableRangeNames;
    QHash<QString, RangePtr> m_rangeNameToRange;
};

}
#endif // CRO_CHANNELRANGESDATA_H
