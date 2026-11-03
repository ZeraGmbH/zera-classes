#include "cro_channelcanchangerangeobserver.h"

namespace ChannelRangeObserver {

ChannelCanChangeRangeObserver::ChannelCanChangeRangeObserver(ChannelPtr channel) :
    ChannelCanChangeRangeObserver(channel, nullptr)
{
}

ChannelCanChangeRangeObserver::ChannelCanChangeRangeObserver(ChannelPtr channel,
                                                             VeinStorage::AbstractComponentPtr rangeAutomaticComponent) :
    m_channel(channel),
    m_rangeAutomaticComponent(rangeAutomaticComponent)
{
}

bool ChannelCanChangeRangeObserver::canChangeRange() const
{
    // conditions for yes:
    // * more than one range
    // * not part of active range automatic
    // * not in source mode (we can adapt that for upper ranges later)
}



}