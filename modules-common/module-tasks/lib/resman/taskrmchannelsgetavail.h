#ifndef TASKRMCHANNELSGETAVAIL_H
#define TASKRMCHANNELSGETAVAIL_H

#include "taskservertransactiontemplate.h"
#include <rminterface.h>
#include <QStringList>

class TaskRmChannelsGetAvail : public TaskServerTransactionTemplate
{
    Q_OBJECT
public:
    static TaskTemplatePtr create(Zera::RMInterfacePtr rmInterface,
                                   int timeout,
                                   QStringList &channelMNameList,
                                   std::function<void()> additionalErrorHandler = []{},
                                   QStringList senseResourcesIgnored = m_defaultSenseResourcesIgnored);
    TaskRmChannelsGetAvail(Zera::RMInterfacePtr rmInterface,
                           QStringList &channelMNameList,
                           QStringList senseResourcesIgnored = m_defaultSenseResourcesIgnored);

private:
    quint32 sendToServer() override;
    bool handleCheckedServerAnswer(QVariant answer) override;
    void fillChannelList(QVariant answer);
    Zera::RMInterfacePtr m_rmInterface;
    QStringList &m_channelMNameList;
    QStringList m_senseResourcesIgnored;
    static QStringList m_defaultSenseResourcesIgnored;
};

#endif // TASKRMCHANNELSGETAVAIL_H
