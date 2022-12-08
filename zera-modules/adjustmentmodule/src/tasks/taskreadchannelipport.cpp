#include "taskreadchannelipport.h"
#include "reply.h"

std::unique_ptr<TaskComposite> TaskReadChannelIpPort::create(Zera::Server::RMInterfacePtr rmInterface, QString channelName, QHash<QString, int> &channelPortHash)
{
    return std::make_unique<TaskReadChannelIpPort>(rmInterface, channelName, channelPortHash);
}

TaskReadChannelIpPort::TaskReadChannelIpPort(Zera::Server::RMInterfacePtr rmInterface, QString channelName, QHash<QString, int> &channelPortHash) :
    m_rmInterface(rmInterface),
    m_channelName(channelName),
    m_channelPortHash(channelPortHash)
{
}

void TaskReadChannelIpPort::start()
{
    connect(m_rmInterface.get(), &Zera::Server::cRMInterface::serverAnswer, this, &TaskReadChannelIpPort::onRmAnswer);
    m_msgnr = m_rmInterface->getResourceInfo("SENSE", m_channelName);
}

void TaskReadChannelIpPort::onRmAnswer(quint32 msgnr, quint8 reply, QVariant answer)
{
    if(m_msgnr == msgnr) {
        bool ok = false;
        QStringList sl = answer.toString().split(';', Qt::SkipEmptyParts);
        if (reply == ack && sl.length() >= 4) {
            int port = sl.at(3).toInt(&ok);
            if (ok)
                m_channelPortHash[m_channelName] = port;
        }
        finishTask(ok);
    }
}
