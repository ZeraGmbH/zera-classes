#ifndef TASKCHANNELPCBCONNECTIONSSTART_H
#define TASKCHANNELPCBCONNECTIONSSTART_H

#include <adjustmentmodulecommon.h>
#include <taskparallel.h>
#include <QStringList>

namespace ADJUSTMENTMODULE {

class TaskChannelPcbConnectionsStart : public TaskComposite
{
    Q_OBJECT
public:
    static std::unique_ptr<TaskComposite> create(AdjustmentModuleActivateDataPtr activateData,QStringList channels,
                                                 QString pcbServerIp, int connectTimeout);
    TaskChannelPcbConnectionsStart(AdjustmentModuleActivateDataPtr activateData, QStringList channels,
                                   QString pcbServerIp, int connectTimeout);
    void start() override;
private:
    AdjustmentModuleActivateDataPtr m_activationData;
    QStringList m_channels;
    QString m_pcbServerIp;
    int m_connectTimeout;
    TaskParallel m_pcbActivationTasks;
};

}
#endif // TASKCHANNELPCBCONNECTIONSSTART_H
