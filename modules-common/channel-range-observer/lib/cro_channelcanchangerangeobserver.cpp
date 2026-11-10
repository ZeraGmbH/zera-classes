#include "cro_channelcanchangerangeobserver.h"

namespace ChannelRangeObserver {

ChannelCanChangeRangeObserver::ChannelCanChangeRangeObserver(ChannelPtr channel,
                                                             VeinStorage::AbstractComponentPtr rangeAutomaticComponent) :
    m_channel(channel),
    m_rangeAutomaticComponent(rangeAutomaticComponent)
{
    checkChangeRange();

    connect(m_channel.get(), &Channel::sigFetchDoneChannel,
            this, &ChannelCanChangeRangeObserver::onConditionsHaveChanged);
    connect(m_channel.get(), &Channel::sigSourceModeOnChanged,
            this, &ChannelCanChangeRangeObserver::onConditionsHaveChanged);
    if (m_rangeAutomaticComponent != nullptr)
        connect(m_rangeAutomaticComponent.get(), &VeinStorage::AbstractComponent::sigValueChange,
                this, &ChannelCanChangeRangeObserver::onConditionsHaveChanged);
}

bool ChannelCanChangeRangeObserver::canChangeRange() const
{
    return m_canChangeRange;
}

void ChannelCanChangeRangeObserver::onConditionsHaveChanged()
{
    if(checkChangeRange())
        emit sigCanChangeRangeChanged();
}

bool ChannelCanChangeRangeObserver::checkChangeRange()
{
    bool newChange = hasMoreThanOneRange() && !rangeAutoIsOn() && !channelSourceModeIsOn();
    if (m_canChangeRange != newChange) {
        m_canChangeRange = newChange;
        return true;
    }
    return false;
}

bool ChannelCanChangeRangeObserver::hasMoreThanOneRange()
{
    return m_channel->getAvailRangeAliases().size() > 1;
}

bool ChannelCanChangeRangeObserver::rangeAutoIsOn()
{
    if (m_rangeAutomaticComponent == nullptr)
        return false;
    return m_rangeAutomaticComponent->getValue().toInt() != 0;
}

bool ChannelCanChangeRangeObserver::channelSourceModeIsOn()
{
    return m_channel->isSourceModeOn();
}

}