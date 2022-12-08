#ifndef TASKCHANNELREGISTERNOTIFIER_H
#define TASKCHANNELREGISTERNOTIFIER_H

#include "taskcomposit.h"
#include "adjustmentmodulecommon.h"

namespace ADJUSTMENTMODULE {

class TaskChannelRegisterNotifier : public TaskComposite
{
    Q_OBJECT
public:
    static std::unique_ptr<TaskComposite> create(AdjustmentModuleActivateDataPtr activationData, QString channelName);
    static std::unique_ptr<TaskComposite> create(AdjustmentModuleActivateDataPtr activationData, QString channelName,
                                                 int timeout, std::function<void()> additionalErrorHandler = []{});
    TaskChannelRegisterNotifier(AdjustmentModuleActivateDataPtr activationData, QString channelName);
    void start() override;
private slots:
    void onRmAnswer(quint32 msgnr, quint8 reply, QVariant answer);
private:
    AdjustmentModuleActivateDataPtr m_activationData;
    QString m_channelName;
    quint32 m_msgnr;
};

}

#endif // TASKCHANNELREGISTERNOTIFIER_H
