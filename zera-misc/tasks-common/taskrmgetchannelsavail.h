#ifndef TASKRMGETCHANNELSAVAIL_H
#define TASKRMGETCHANNELSAVAIL_H

#include "taskservertransactiontemplate.h"
#include "abstractrminterface.h"
#include <QStringList>

class TaskRmGetChannelsAvail : public TaskServerTransactionTemplate
{
    Q_OBJECT
public:
    static TaskCompositePtr create(AbstractRmInterfacePtr rmInterface,
                                   int timeout,
                                   QStringList &channelSysNameList,
                                   std::function<void()> additionalErrorHandler = []{},
                                   QStringList senseResourcesIgnored = m_defaultSenseResourcesIgnored);
    TaskRmGetChannelsAvail(AbstractRmInterfacePtr rmInterface,
                           QStringList &channelSysNameList,
                           QStringList senseResourcesIgnored = m_defaultSenseResourcesIgnored);
    quint32 sendToServer() override;
protected:
    bool handleServerAnswer(QVariant answer) override;
private:
    void fillChannelList(QVariant answer);
    AbstractRmInterfacePtr m_rmInterface;
    QStringList &m_channelSysNameList;
    QStringList m_senseResourcesIgnored;
    static QStringList m_defaultSenseResourcesIgnored;
};

#endif // TASKRMGETCHANNELSAVAIL_H
