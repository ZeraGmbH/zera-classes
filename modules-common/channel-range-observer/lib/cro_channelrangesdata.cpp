#include "cro_channelrangesdata.h"

namespace ChannelRangeObserver {

void ChannelRangesData::clear()
{
    m_allRangeNamesOrderedByServer = std::make_shared<QStringList>();
    m_availableRangeNames.clear();
    m_rangeNameToRange.clear();
};

}