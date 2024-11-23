#include "channelfetchtask.h"

ChannelFetchTask::ChannelFetchTask(ChannelObserverPtr rangeObserver)
{
    connect(rangeObserver.get(), &ChannelObserver::sigFetchComplete,
            this, &ChannelFetchTask::onChannelFetched);
}

void ChannelFetchTask::start()
{

}

void ChannelFetchTask::onChannelFetched(QString channelMName)
{
    finishTask(true);
}
