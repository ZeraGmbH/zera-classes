#ifndef TASKCHANNELREADRANGES_H
#define TASKCHANNELREADRANGES_H

#include "taskservertransactiontemplate.h"
#include <pcbinterface.h>

class TaskChannelGetRangeList : public TaskServerTransactionTemplate
{
    Q_OBJECT
public:
    static TaskTemplatePtr create(Zera::PcbInterfacePtr pcbInterface,
                                   QString channelName,
                                   QStringList &targetRangeList,
                                   int timeout, std::function<void()> additionalErrorHandler = []{});
    TaskChannelGetRangeList(Zera::PcbInterfacePtr pcbInterface, QString channelName, QStringList &targetRangeList);
private:
    quint32 sendToServer() override;
    bool handleCheckedServerAnswer(QVariant answer) override;
    Zera::PcbInterfacePtr m_pcbInterface;
    QString m_channelName;
    QStringList &m_targetRangeList;
};

#endif // TASKCHANNELREADRANGES_H
