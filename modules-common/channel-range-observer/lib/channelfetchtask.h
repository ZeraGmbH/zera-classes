#ifndef CHANNELFETCHTASK_H
#define CHANNELFETCHTASK_H

#include "channelbserver.h"
#include <tasktemplate.h>

class ChannelFetchTask : public TaskTemplate
{
    Q_OBJECT
public:
    ChannelFetchTask(ChannelObserverPtr rangeObserver);
    void start() override;
private slots:
    void onChannelFetched(QString channelMName);
};

#endif // CHANNELFETCHTASK_H
