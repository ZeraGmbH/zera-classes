#include "taskrmchannelscheckavail.h"
#include "taskrmchannelsgetavail.h"

TaskCompositePtr TaskRmChannelsCheckAvail::create(AbstractRmInterfacePtr rmInterface,
                                                  QStringList expectedChannels,
                                                  int timeout, std::function<void ()> additionalErrorHandler)
{
    return std::make_unique<TaskRmChannelsCheckAvail>(rmInterface,
                                                      expectedChannels,
                                                      timeout, additionalErrorHandler);
}

TaskRmChannelsCheckAvail::TaskRmChannelsCheckAvail(AbstractRmInterfacePtr rmInterface,
                                                   QStringList expectedChannels,
                                                   int timeout, std::function<void ()> additionalErrorHandler) :
    m_taskGetChannelList(TaskRmChannelsGetAvail::create(rmInterface,
                                                        timeout,
                                                        m_receivedChannels,
                                                        additionalErrorHandler)),
    m_expectedChannels(expectedChannels)
{
}

void TaskRmChannelsCheckAvail::start()
{
    connect(m_taskGetChannelList.get(), &TaskComposite::sigFinish,
            this, &TaskRmChannelsCheckAvail::onChannelGetFinish);
    m_taskGetChannelList->start();
}

void TaskRmChannelsCheckAvail::onChannelGetFinish(bool ok)
{
    if(ok) {
        for(const auto &expectedChannel : qAsConst(m_expectedChannels)) {
            if(!m_receivedChannels.contains(expectedChannel)) {
                ok = false;
                break;
            }
        }
    }
    m_taskGetChannelList = nullptr;
    emit sigFinish(ok, getTaskId());
}
