#ifndef CRO_CHANNELCANCHANGERANGEOBSERVER_H
#define CRO_CHANNELCANCHANGERANGEOBSERVER_H

#include "cro_channel.h"
#include <vs_abstractcomponent.h>

namespace ChannelRangeObserver {

class ChannelCanChangeRangeObserver : public QObject
{
    Q_OBJECT
public:
    explicit ChannelCanChangeRangeObserver(ChannelPtr channel); // AUX
    explicit ChannelCanChangeRangeObserver(ChannelPtr channel, VeinStorage::AbstractComponentPtr rangeAutomaticComponent);

    bool canChangeRange() const;
signals:
    void sigCanChangeRangeChanged();

private:
    ChannelPtr m_channel;
    VeinStorage::AbstractComponentPtr m_rangeAutomaticComponent;
};

}
#endif // CRO_CHANNELCANCHANGERANGEOBSERVER_H
