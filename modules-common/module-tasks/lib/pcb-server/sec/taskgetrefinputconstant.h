#ifndef TASKGETREFINPUTCONSTANT_H
#define TASKGETREFINPUTCONSTANT_H

#include <taskservertransactiontemplate.h>
#include <pcbinterface.h>

class TaskGetRefInputConstant : public TaskServerTransactionTemplate
{
    Q_OBJECT
public:
    static TaskTemplatePtr create(Zera::PcbInterfacePtr pcbInterface,
                                  QString refInputName,
                                  std::shared_ptr<double> refContantReceived,
                                  int timeout, std::function<void()> additionalErrorHandler = []{});
    TaskGetRefInputConstant(Zera::PcbInterfacePtr pcbInterface,
                            QString refInputName,
                            std::shared_ptr<double> refConstantReceived);
private:
    quint32 sendToServer() override;
    bool handleCheckedServerAnswer(QVariant answer) override;
    Zera::PcbInterfacePtr m_pcbInterface;
    QString m_refInputName;
    std::shared_ptr<double> m_refConstantReceived;
};

#endif // TASKGETREFINPUTCONSTANT_H
