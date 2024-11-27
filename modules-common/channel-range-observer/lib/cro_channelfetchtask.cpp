#include "cro_channelfetchtask.h"

namespace ChannelRangeObserver {

ChannelFetchTaskPtr ChannelFetchTask::create(ChannelPtr rangeObserver)
{
    return std::make_unique<ChannelFetchTask>(rangeObserver);
}

ChannelFetchTask::ChannelFetchTask(ChannelPtr rangeObserver) :
    m_rangeObserver(rangeObserver)
{
    connect(m_rangeObserver.get(), &Channel::sigFetchComplete,
            this, &ChannelFetchTask::onChannelFetched);
}

void ChannelFetchTask::start()
{
    m_rangeObserver->startFetch();
}

void ChannelFetchTask::onChannelFetched(QString channelMName, bool ok)
{
    Q_UNUSED(channelMName)
    finishTask(ok);
}

}
