#ifndef TASKGETSAMPLERATE_H
#define TASKGETSAMPLERATE_H

#include "taskservertransactiontemplate.h"
#include <pcbinterface.h>

class TaskGetSampleRate : public TaskServerTransactionTemplate
{
    Q_OBJECT
public:
    static TaskTemplatePtr create(Zera::PcbInterfacePtr pcbInterface,
                                  std::shared_ptr<int> valueReceived,
                                  int timeout, std::function<void()> additionalErrorHandler = []{});
    TaskGetSampleRate(Zera::PcbInterfacePtr pcbInterface,
                      std::shared_ptr<int> valueReceived);
private:
    quint32 sendToServer() override;
    bool handleCheckedServerAnswer(QVariant answer) override;
    Zera::PcbInterfacePtr m_pcbInterface;
    std::shared_ptr<int> m_valueReceived;
};

#endif // TASKGETSAMPLERATE_H
