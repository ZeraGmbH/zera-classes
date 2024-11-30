#ifndef TASKRANGEGETTYPE_H
#define TASKRANGEGETTYPE_H

#include <taskservertransactiontemplate.h>
#include <pcbinterface.h>

class TaskRangeGetType : public TaskServerTransactionTemplate
{
    Q_OBJECT
public:
    static TaskTemplatePtr create(Zera::PcbInterfacePtr pcbInterface,
                                  QString channelMName, QString rangeName,
                                  int &valueReceived,
                                  int timeout, std::function<void()> additionalErrorHandler = []{});
    TaskRangeGetType(Zera::PcbInterfacePtr pcbInterface,
                     QString channelMName, QString rangeName,
                     int &valueReceived);
private:
    quint32 sendToServer() override;
    bool handleCheckedServerAnswer(QVariant answer) override;
    Zera::PcbInterfacePtr m_pcbInterface;
    QString m_channelMName;
    QString m_rangeName;
    int &m_valueReceived;
};

#endif // TASKRANGEGETTYPE_H
