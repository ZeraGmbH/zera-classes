#include "taskrmchannelscheckavail.h"
#include "tasktimeoutdecorator.h"
#include "reply.h"

TaskCompositePtr TaskRmChannelsCheckAvail::create(AbstractRmInterfacePtr rmInterface,
                                                  QStringList expectedChannels,
                                                  int timeout, std::function<void ()> additionalErrorHandler)
{
    return TaskTimeoutDecorator::wrapTimeout(timeout,
                                             std::make_unique<TaskRmChannelsCheckAvail>(
                                                 rmInterface,
                                                 expectedChannels),
                                             additionalErrorHandler);
}

TaskRmChannelsCheckAvail::TaskRmChannelsCheckAvail(AbstractRmInterfacePtr rmInterface,
                                                   QStringList expectedChannels) :
    m_rmInterface(rmInterface),
    m_expectedChannels(expectedChannels)
{
}

void TaskRmChannelsCheckAvail::start()
{
    connect(m_rmInterface.get(), &AbstractRmInterface::serverAnswer,
            this, &TaskRmChannelsCheckAvail::onServerAnswer);
    m_msgnr = m_rmInterface->getResources("SENSE");
}

void TaskRmChannelsCheckAvail::onServerAnswer(quint32 msgnr, quint8 reply, QVariant answer)
{
    if(m_msgnr == msgnr) {
        bool allPresent = true;
        if (reply == ack) {
            QString sAnswer = answer.toString();
            for(const auto &expectedChannel : qAsConst(m_expectedChannels)) {
                if(!sAnswer.contains(expectedChannel)) {
                    allPresent = false;
                    break;
                }
            }
        }
        else
            allPresent = false;
        finishTask(allPresent);
    }
}
