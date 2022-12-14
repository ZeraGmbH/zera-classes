#ifndef ADJUSTMENTMODULEACTIVATOR_H
#define ADJUSTMENTMODULEACTIVATOR_H

#include "adjustmentmodulecommon.h"
#include "rminterface.h"
#include "pcbinterface.h"
#include "tasksequence.h"
#include "taskparallel.h"
#include "socket.h"
#include <veinmoduleerrorcomponent.h>

namespace ADJUSTMENTMODULE
{

class AdjustmentModuleActivator : public QObject
{
    Q_OBJECT
public:
    AdjustmentModuleActivator(QStringList configuredChannels,
                              AdjustmentModuleCommonPtr activationData);
    void activate();
    void deactivate();
    void reloadRanges();
signals:
    void sigActivationReady();
    void sigDeactivationReady();
    void sigRangesReloaded();
    void errMsg(QVariant value, int dest = globalDest);

private slots:
    void onActivateContinue(bool ok);
    void onDeactivateContinue(bool ok);
    void onReloadRanges(bool ok);
private:
    void addStaticActivationTasks();
    void addDynChannelActivationTasks();
    TaskCompositePtr getChannelsReadTasks();
    TaskCompositePtr getChannelsRegisterNotifyTasks();
    TaskCompositePtr getDeactivationTasks();
    void initChannelInfoHash();
    void fillChannelAliasHash();

    QStringList m_configuredChannels;

    TaskSequence m_activationTasks;
    TaskSequence m_deactivationTasks;
    TaskSequence m_reloadRangesTasks;

    AdjustmentModuleCommonPtr m_commonObjects;
};

}
#endif // ADJUSTMENTMODULEACTIVATOR_H
