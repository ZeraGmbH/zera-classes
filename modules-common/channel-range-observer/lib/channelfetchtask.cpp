#include "channelfetchtask.h"

ChannelFetchTaskPtr ChannelFetchTask::create(ChannelObserverPtr rangeObserver)
{
    return std::make_unique<ChannelFetchTask>(rangeObserver);
}

ChannelFetchTask::ChannelFetchTask(ChannelObserverPtr rangeObserver) :
    m_rangeObserver(rangeObserver)
{
    connect(m_rangeObserver.get(), &ChannelObserver::sigFetchComplete,
            this, &ChannelFetchTask::onChannelFetched);
}

void ChannelFetchTask::start()
{
    m_rangeObserver->startFetch();
}

void ChannelFetchTask::onChannelFetched(QString channelMName)
{
    finishTask(true);
}
