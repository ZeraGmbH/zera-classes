#include "cro_channelfetchtask.h"

namespace ChannelRangeObserver {

ChannelFetchTaskPtr ChannelFetchTask::create(ChannelPtr channel)
{
    return std::make_unique<ChannelFetchTask>(channel);
}

ChannelFetchTask::ChannelFetchTask(ChannelPtr channel) :
    m_rangeObserver(channel)
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
