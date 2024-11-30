#ifndef TASKRANGEGETURVALUE_H
#define TASKRANGEGETURVALUE_H

#include "taskservertransactiontemplate.h"
#include <pcbinterface.h>

class TaskRangeGetUrValue : public TaskServerTransactionTemplate
{
    Q_OBJECT
public:
    static TaskTemplatePtr create(Zera::PcbInterfacePtr pcbInterface,
                                  QString channelMName, QString rangeName,
                                  double &valueReceived,
                                  int timeout, std::function<void()> additionalErrorHandler = []{});
    TaskRangeGetUrValue(Zera::PcbInterfacePtr pcbInterface,
                        QString channelMName, QString rangeName,
                        double &valueReceived);
private:
    quint32 sendToServer() override;
    bool handleCheckedServerAnswer(QVariant answer) override;
    Zera::PcbInterfacePtr m_pcbInterface;
    QString m_channelMName;
    QString m_rangeName;
    double &m_valueReceived;
};

#endif // TASKRANGEGETURVALUE_H
