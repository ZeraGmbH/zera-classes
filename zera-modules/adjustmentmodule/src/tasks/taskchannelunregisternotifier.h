#ifndef TASKCHANNELUNREGISTERNOTIFIER_H
#define TASKCHANNELUNREGISTERNOTIFIER_H

#include "taskcomposit.h"
#include "adjustmentmodulecommon.h"

namespace ADJUSTMENTMODULE {

class TaskChannelUnregisterNotifier : public TaskComposite
{
    Q_OBJECT
public:
    static std::unique_ptr<TaskComposite> create(AdjustmentModuleCommonPtr activationData, QString channelName);
    static std::unique_ptr<TaskComposite> create(AdjustmentModuleCommonPtr activationData, QString channelName,
                                                 int timeout, std::function<void()> additionalErrorHandler = []{});
    TaskChannelUnregisterNotifier(AdjustmentModuleCommonPtr activationData, QString channelName);
    void start() override;
private slots:
    void onRmAnswer(quint32 msgnr, quint8 reply, QVariant answer);
private:
    AdjustmentModuleCommonPtr m_commonObjects;
    QString m_channelName;
    quint32 m_msgnr;
};

}

#endif // TASKCHANNELUNREGISTERNOTIFIER_H
