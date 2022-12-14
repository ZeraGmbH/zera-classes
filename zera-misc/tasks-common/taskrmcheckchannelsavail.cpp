#include "taskrmcheckchannelsavail.h"
#include "tasktimeoutdecorator.h"
#include "reply.h"

TaskCompositePtr TaskRmCheckChannelsAvail::create(Zera::Server::RMInterfacePtr rmInterface,
                                                  QStringList expectedChannels,
                                                  int timeout, std::function<void ()> additionalErrorHandler)
{
    return TaskTimeoutDecorator::wrapTimeout(timeout,
                                             std::make_unique<TaskRmCheckChannelsAvail>(
                                                 rmInterface,
                                                 expectedChannels),
                                             additionalErrorHandler);
}

TaskRmCheckChannelsAvail::TaskRmCheckChannelsAvail(Zera::Server::RMInterfacePtr rmInterface,
                                                   QStringList expectedChannels) :
    m_rmInterface(rmInterface),
    m_expectedChannels(expectedChannels)
{
}

void TaskRmCheckChannelsAvail::start()
{
    connect(m_rmInterface.get(), &Zera::Server::cRMInterface::serverAnswer, this, &TaskRmCheckChannelsAvail::onServerAnswer);
    m_msgnr = m_rmInterface->getResources("SENSE");
}

void TaskRmCheckChannelsAvail::onServerAnswer(quint32 msgnr, quint8 reply, QVariant answer)
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
