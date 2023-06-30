#include "taskchannelscheckavail.h"
#include "taskchannelgetavail.h"

TaskTemplatePtr TaskChannelsCheckAvail::create(Zera::PcbInterfacePtr pcbInterface,
                                                 QStringList expectedChannels,
                                                 int timeout, std::function<void ()> additionalErrorHandler)
{
    return std::make_unique<TaskChannelsCheckAvail>(pcbInterface,
                                                      expectedChannels,
                                                      timeout, additionalErrorHandler);
}

TaskChannelsCheckAvail::TaskChannelsCheckAvail(Zera::PcbInterfacePtr pcbInterface,
                                                   QStringList expectedChannels,
                                                   int timeout, std::function<void ()> additionalErrorHandler) :
    m_taskGetChannelList(TaskChannelGetAvail::create(pcbInterface,
                                                     m_receivedChannels,
                                                     timeout,
                                                     additionalErrorHandler)),
    m_expectedChannels(expectedChannels)
{
}

void TaskChannelsCheckAvail::start()
{
    connect(m_taskGetChannelList.get(), &TaskTemplate::sigFinish,
            this, &TaskChannelsCheckAvail::onChannelGetFinish);
    m_taskGetChannelList->start();
}

void TaskChannelsCheckAvail::onChannelGetFinish(bool ok)
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
