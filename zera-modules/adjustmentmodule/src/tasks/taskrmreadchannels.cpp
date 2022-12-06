#include "taskrmreadchannels.h"
#include "reply.h"

std::unique_ptr<TaskRmReadChannels> TaskRmReadChannels::create(Zera::Server::RMInterfacePtr rmInterface, QStringList expectedChannels)
{
    return std::make_unique<TaskRmReadChannels>(rmInterface, expectedChannels);
}

TaskRmReadChannels::TaskRmReadChannels(Zera::Server::RMInterfacePtr rmInterface, QStringList expectedChannels) :
    m_rmInterface(rmInterface),
    m_expectedChannels(expectedChannels)
{
}

void TaskRmReadChannels::start()
{
    connect(m_rmInterface.get(), &Zera::Server::cRMInterface::serverAnswer, this, &TaskRmReadChannels::onRmAnswer);
    m_msgnr = m_rmInterface->getResources("SENSE");
}

void TaskRmReadChannels::onRmAnswer(quint32 msgnr, quint8 reply, QVariant answer)
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
