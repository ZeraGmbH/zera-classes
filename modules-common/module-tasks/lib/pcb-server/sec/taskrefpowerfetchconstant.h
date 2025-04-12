#ifndef TASKREFPOWERFETCHCONSTANT_H
#define TASKREFPOWERFETCHCONSTANT_H

#include <taskservertransactiontemplate.h>
#include <pcbinterface.h>

class TaskRefPowerFetchConstant : public TaskServerTransactionTemplate
{
    Q_OBJECT
public:
    static TaskTemplatePtr create(Zera::PcbInterfacePtr pcbInterface,
                                  QString refPowerName, // fo0;fo1...
                                  std::shared_ptr<double> refPowerConstantReceived,
                                  int timeout, std::function<void()> additionalErrorHandler = []{});
    TaskRefPowerFetchConstant(Zera::PcbInterfacePtr pcbInterface,
                              QString refPowerName,
                              std::shared_ptr<double> refPowerConstantReceived);
private:
    quint32 sendToServer() override;
    bool handleCheckedServerAnswer(QVariant answer) override;
    Zera::PcbInterfacePtr m_pcbInterface;
    QString m_refPowerName;
    std::shared_ptr<double> m_refPowerConstantReceived;
};

#endif // TASKREFPOWERFETCHCONSTANT_H
