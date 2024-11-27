#include "cro_rangefetchtask.h"

namespace ChannelRangeObserver {

RangeFetchTaskPtr RangeFetchTask::create(RangePtr range)
{
    return std::make_unique<RangeFetchTask>(range);
}

RangeFetchTask::RangeFetchTask(RangePtr range) :
    m_range(range)
{
    connect(m_range.get(), &Range::sigFetchComplete,
            this, &RangeFetchTask::onRangeFetched);
}

void RangeFetchTask::start()
{
    m_range->startFetch();
}

void RangeFetchTask::onRangeFetched(QString channelMName, QString rangeName, bool ok)
{
    Q_UNUSED(channelMName)
    Q_UNUSED(rangeName)
    finishTask(ok);

}

}
