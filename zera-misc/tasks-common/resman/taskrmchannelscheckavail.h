#ifndef TASKRMCHANNELSCHECKAVAIL_H
#define TASKRMCHANNELSCHECKAVAIL_H

#include "taskcomposit.h"
#include "rminterface.h"
#include <QStringList>

class TaskRmChannelsCheckAvail : public TaskComposite
{
    Q_OBJECT
public:
    static TaskCompositePtr create(Zera::Server::RMInterfacePtr rmInterface,
                                   QStringList expectedChannels,
                                   int timeout, std::function<void()> additionalErrorHandler = []{});
    TaskRmChannelsCheckAvail(Zera::Server::RMInterfacePtr rmInterface,
                             QStringList expectedChannels,
                             int timeout, std::function<void()> additionalErrorHandler = []{});
    void start() override;
private slots:
    void onChannelGetFinish(bool ok);
private:
    TaskCompositePtr m_taskGetChannelList;
    QStringList m_receivedChannels;
    QStringList m_expectedChannels;
};

#endif // TASKRMCHANNELSCHECKAVAIL_H
