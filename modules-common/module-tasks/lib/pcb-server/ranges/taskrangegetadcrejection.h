#ifndef TASKRANGEGETADCREJECTION_H
#define TASKRANGEGETADCREJECTION_H

#include <taskservertransactiontemplate.h>
#include <pcbinterface.h>

class TaskRangeGetAdcRejection : public TaskServerTransactionTemplate
{
    Q_OBJECT
public:
    static TaskTemplatePtr create(Zera::PcbInterfacePtr pcbInterface,
                                  QString channelMName, QString rangeName,
                                  std::shared_ptr<double> valueReceived,
                                  int timeout, std::function<void()> additionalErrorHandler = []{});
    TaskRangeGetAdcRejection(Zera::PcbInterfacePtr pcbInterface,
                             QString channelMName, QString rangeName,
                             std::shared_ptr<double> valueReceived);
private:
    quint32 sendToServer() override;
    bool handleCheckedServerAnswer(QVariant answer) override;
    Zera::PcbInterfacePtr m_pcbInterface;
    QString m_channelMName;
    QString m_rangeName;
    std::shared_ptr<double> m_valueReceived;
};

#endif // TASKRANGEGETADCREJECTION_H
