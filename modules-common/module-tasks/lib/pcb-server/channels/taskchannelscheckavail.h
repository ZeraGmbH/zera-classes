#ifndef TASKRMCHANNELSCHECKAVAIL_H
#define TASKRMCHANNELSCHECKAVAIL_H

#include "tasktemplate.h"
#include <pcbinterface.h>
#include <QStringList>

class TaskChannelsCheckAvail : public TaskTemplate
{
    Q_OBJECT
public:
    static TaskTemplatePtr create(Zera::PcbInterfacePtr pcbInterface,
                                   QStringList expectedChannels,
                                   int timeout, std::function<void()> additionalErrorHandler = []{});
    TaskChannelsCheckAvail(Zera::PcbInterfacePtr pcbInterface,
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
