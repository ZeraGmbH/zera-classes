#ifndef CHANNELFETCHTASK_H
#define CHANNELFETCHTASK_H

#include "channel.h"
#include <tasktemplate.h>

namespace ChannelRangeObserver {

class ChannelFetchTask;
typedef std::unique_ptr<ChannelFetchTask> ChannelFetchTaskPtr;

class ChannelFetchTask : public TaskTemplate
{
    Q_OBJECT
public:
    static ChannelFetchTaskPtr create(ChannelPtr rangeObserver);
    ChannelFetchTask(ChannelPtr rangeObserver);
    void start() override;
private slots:
    void onChannelFetched(QString channelMName);
private:
    ChannelPtr m_rangeObserver;
};

}
#endif // CHANNELFETCHTASK_H
