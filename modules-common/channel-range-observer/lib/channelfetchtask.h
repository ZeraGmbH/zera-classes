#ifndef CHANNELFETCHTASK_H
#define CHANNELFETCHTASK_H

#include "channelbserver.h"
#include <tasktemplate.h>

class ChannelFetchTask;
typedef std::unique_ptr<ChannelFetchTask> ChannelFetchTaskPtr;

class ChannelFetchTask : public TaskTemplate
{
    Q_OBJECT
public:
    static ChannelFetchTaskPtr create(ChannelObserverPtr rangeObserver);
    ChannelFetchTask(ChannelObserverPtr rangeObserver);
    void start() override;
private slots:
    void onChannelFetched(QString channelMName);
private:
    ChannelObserverPtr m_rangeObserver;
};


#endif // CHANNELFETCHTASK_H
