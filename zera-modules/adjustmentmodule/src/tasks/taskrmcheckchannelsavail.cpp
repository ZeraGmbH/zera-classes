#include "taskrmcheckchannelsavail.h"
#include "reply.h"

std::unique_ptr<TaskComposite> TaskRmCheckChannelsAvail::create(Zera::Server::RMInterfacePtr rmInterface, QStringList expectedChannels)
{
    return std::make_unique<TaskRmCheckChannelsAvail>(rmInterface, expectedChannels);
}

TaskRmCheckChannelsAvail::TaskRmCheckChannelsAvail(Zera::Server::RMInterfacePtr rmInterface, QStringList expectedChannels) :
    m_rmInterface(rmInterface),
    m_expectedChannels(expectedChannels)
{
}

void TaskRmCheckChannelsAvail::start()
{
    connect(m_rmInterface.get(), &Zera::Server::cRMInterface::serverAnswer, this, &TaskRmCheckChannelsAvail::onRmAnswer);
    m_msgnr = m_rmInterface->getResources("SENSE");
}

void TaskRmCheckChannelsAvail::onRmAnswer(quint32 msgnr, quint8 reply, QVariant answer)
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
