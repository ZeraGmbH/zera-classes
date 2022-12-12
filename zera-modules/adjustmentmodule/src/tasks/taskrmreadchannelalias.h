#ifndef TASKRMREADCHANNELALIAS_H
#define TASKRMREADCHANNELALIAS_H

#include "rminterface.h"
#include "taskcomposit.h"
#include "adjustmentmodulecommon.h"

namespace ADJUSTMENTMODULE {

class TaskRmReadChannelAlias : public TaskComposite
{
    Q_OBJECT
public:
    static std::unique_ptr<TaskComposite> create(AdjustmentModuleCommonPtr activationData, QString channelName);
    static std::unique_ptr<TaskComposite> create(AdjustmentModuleCommonPtr activationData, QString channelName,
                                                 int timeout, std::function<void()> additionalErrorHandler = []{});
    TaskRmReadChannelAlias(AdjustmentModuleCommonPtr activationData, QString channelName);
    void start() override;
private slots:
    void onRmAnswer(quint32 msgnr, quint8 reply, QVariant answer);
private:
    AdjustmentModuleCommonPtr m_commonObjects;
    QString m_channelName;
    quint32 m_msgnr;
};
}
#endif // TASKRMREADCHANNELALIAS_H
