#ifndef TASKRMCHANNELSGETAVAIL_H
#define TASKRMCHANNELSGETAVAIL_H

#include "taskservertransactiontemplate.h"
#include "rminterface.h"
#include <QStringList>

class TaskRmChannelsGetAvail : public TaskServerTransactionTemplate
{
    Q_OBJECT
public:
    static TaskCompositePtr create(Zera::Server::RMInterfacePtr rmInterface,
                                   int timeout,
                                   QStringList &channelSysNameList,
                                   std::function<void()> additionalErrorHandler = []{},
                                   QStringList senseResourcesIgnored = m_defaultSenseResourcesIgnored);
    TaskRmChannelsGetAvail(Zera::Server::RMInterfacePtr rmInterface,
                           QStringList &channelSysNameList,
                           QStringList senseResourcesIgnored = m_defaultSenseResourcesIgnored);

private:
    quint32 sendToServer() override;
    bool handleCheckedServerAnswer(QVariant answer) override;
    void fillChannelList(QVariant answer);
    Zera::Server::RMInterfacePtr m_rmInterface;
    QStringList &m_channelSysNameList;
    QStringList m_senseResourcesIgnored;
    static QStringList m_defaultSenseResourcesIgnored;
};

#endif // TASKRMCHANNELSGETAVAIL_H
