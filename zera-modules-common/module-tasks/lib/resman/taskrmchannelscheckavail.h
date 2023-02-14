#ifndef TASKRMCHANNELSCHECKAVAIL_H
#define TASKRMCHANNELSCHECKAVAIL_H

#include "tasktemplate.h"
#include <rminterface.h>
#include <QStringList>

class TaskRmChannelsCheckAvail : public TaskTemplate
{
    Q_OBJECT
public:
    static TaskTemplatePtr create(Zera::RMInterfacePtr rmInterface,
                                   QStringList expectedChannels,
                                   int timeout, std::function<void()> additionalErrorHandler = []{});
    TaskRmChannelsCheckAvail(Zera::RMInterfacePtr rmInterface,
                             QStringList expectedChannels,
                             int timeout, std::function<void()> additionalErrorHandler = []{});
    void start() override;
private slots:
    void onChannelGetFinish(bool ok);
private:
    TaskTemplatePtr m_taskGetChannelList;
    QStringList m_receivedChannels;
    QStringList m_expectedChannels;
};

#endif // TASKRMCHANNELSCHECKAVAIL_H
