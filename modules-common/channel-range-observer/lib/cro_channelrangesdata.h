#ifndef CRO_CHANNELRANGESDATA_H
#define CRO_CHANNELRANGESDATA_H

#include "cro_range.h"
#include <memory>

namespace ChannelRangeObserver {

class ChannelRangesData
{
public:
    void clear();

    std::shared_ptr<QStringList> m_allRangeNamesOrderedByServer = std::make_shared<QStringList>();
    QStringList m_availableRangeNames;
    QHash<QString, RangePtr> m_rangeNameToRange;
};

}
#endif // CRO_CHANNELRANGESDATA_H
