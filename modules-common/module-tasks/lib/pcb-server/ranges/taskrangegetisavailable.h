#ifndef TASKRANGEGETISAVAILABLE_H
#define TASKRANGEGETISAVAILABLE_H

#include "taskservertransactiontemplate.h"
#include <pcbinterface.h>

class TaskRangeGetIsAvailable : public TaskServerTransactionTemplate
{
    Q_OBJECT
public:
    static TaskTemplatePtr create(Zera::PcbInterfacePtr pcbInterface,
                                  QString channelMName, QString rangeName,
                                  bool &valueReceived,
                                  int timeout, std::function<void ()> additionalErrorHandler = []{});
    TaskRangeGetIsAvailable(Zera::PcbInterfacePtr pcbInterface,
                            QString channelMName, QString rangeName,
                            bool &valueReceived);
private:
    quint32 sendToServer() override;
    bool handleCheckedServerAnswer(QVariant answer) override;
    Zera::PcbInterfacePtr m_pcbInterface;
    QString m_channelMName;
    QString m_rangeName;
    bool &m_valueReceived;
};

#endif // TASKRANGEGETISAVAILABLE_H
