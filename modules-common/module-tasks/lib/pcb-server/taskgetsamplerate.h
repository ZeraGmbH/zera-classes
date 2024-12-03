#ifndef TASKGETSAMPLERATE_H
#define TASKGETSAMPLERATE_H

#include "taskservertransactiontemplate.h"
#include <pcbinterface.h>

class TaskGetSampleRate : public TaskServerTransactionTemplate
{
    Q_OBJECT
public:
    static TaskTemplatePtr create(Zera::PcbInterfacePtr pcbInterface,
                                  int& valueReceived,
                                  int timeout, std::function<void()> additionalErrorHandler = []{});
    TaskGetSampleRate(Zera::PcbInterfacePtr pcbInterface,
                        int& valueReceived);
private:
    quint32 sendToServer() override;
    bool handleCheckedServerAnswer(QVariant answer) override;
    Zera::PcbInterfacePtr m_pcbInterface;
    int& m_valueReceived;
};

#endif // TASKGETSAMPLERATE_H
