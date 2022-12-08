#ifndef TASKCHANNELUNREGISTERNOTIFIER_H
#define TASKCHANNELUNREGISTERNOTIFIER_H

#include "taskcomposit.h"
#include "adjustmentmodulecommon.h"

namespace ADJUSTMENTMODULE {

class TaskChannelUnregisterNotifier : public TaskComposite
{
    Q_OBJECT
public:
    static std::unique_ptr<TaskComposite> create(AdjustmentModuleActivateDataPtr activationData, QString channelName);
    static std::unique_ptr<TaskComposite> create(AdjustmentModuleActivateDataPtr activationData, QString channelName,
                                                 int timeout, std::function<void()> additionalErrorHandler = []{});
    TaskChannelUnregisterNotifier(AdjustmentModuleActivateDataPtr activationData, QString channelName);
    void start() override;
private slots:
    void onRmAnswer(quint32 msgnr, quint8 reply, QVariant answer);
private:
    AdjustmentModuleActivateDataPtr m_activationData;
    QString m_channelName;
    quint32 m_msgnr;
};

}

#endif // TASKCHANNELUNREGISTERNOTIFIER_H
