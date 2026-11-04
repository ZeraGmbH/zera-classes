#ifndef CRO_CHANNELCANCHANGERANGEOBSERVER_H
#define CRO_CHANNELCANCHANGERANGEOBSERVER_H

#include "cro_channel.h"
#include <vs_abstractcomponent.h>
#include <memory>

namespace ChannelRangeObserver {

class ChannelCanChangeRangeObserver : public QObject
{
    Q_OBJECT
public:
    explicit ChannelCanChangeRangeObserver(ChannelPtr channel,
                                           VeinStorage::AbstractComponentPtr rangeAutomaticComponent);

    bool canChangeRange() const;
signals:
    void sigCanChangeRangeChanged();

private slots:
    void onConditionsHaveChanged();
private:
    bool checkChangeRange();
    bool hasMoreThanOneRange();
    bool rangeAutoIsOn();
    bool channelSourceModeIsOn();
    ChannelPtr m_channel;
    VeinStorage::AbstractComponentPtr m_rangeAutomaticComponent;
    bool m_canChangeRange = false;
};

typedef std::shared_ptr<ChannelCanChangeRangeObserver> ChannelCanChangeRangeObserverPtr;

}
#endif // CRO_CHANNELCANCHANGERANGEOBSERVER_H
