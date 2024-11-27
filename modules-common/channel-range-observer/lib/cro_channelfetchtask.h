#ifndef CRO_CHANNELFETCHTASK_H
#define CRO_CHANNELFETCHTASK_H

#include "cro_channel.h"
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
    void onChannelFetched(QString channelMName, bool ok);
private:
    ChannelPtr m_rangeObserver;
};

}
#endif // CRO_CHANNELFETCHTASK_H
