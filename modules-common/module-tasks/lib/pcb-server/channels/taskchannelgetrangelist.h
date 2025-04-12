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
                                   std::shared_ptr<QStringList> rangeList,
                                   int timeout, std::function<void()> additionalErrorHandler = []{});
    TaskChannelGetRangeList(Zera::PcbInterfacePtr pcbInterface, QString channelName, std::shared_ptr<QStringList>rangeList);
private:
    quint32 sendToServer() override;
    bool handleCheckedServerAnswer(QVariant answer) override;
    Zera::PcbInterfacePtr m_pcbInterface;
    QString m_channelName;
    std::shared_ptr<QStringList> m_rangeList;
};

#endif // TASKCHANNELREADRANGES_H
