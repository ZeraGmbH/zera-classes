#ifndef ADJUSTMENTMODULEACTIVATOR_H
#define ADJUSTMENTMODULEACTIVATOR_H

#include "adjustmentmodulecommon.h"
#include <rminterface.h>
#include <pcbinterface.h>
#include "taskcontainersequence.h"
#include "taskcontainerparallel.h"
#include "networkconnectioninfo.h"
#include <vfmoduleerrorcomponent.h>

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
    void errMsg(QVariant value);

private slots:
    void onActivateContinue(bool ok);
    void onDeactivateContinue(bool ok);
    void onReloadRanges(bool ok);
private:
    void addStaticActivationTasks();
    void addDynChannelActivationTasks();
    void notifyError(QVariant value);
    TaskTemplatePtr getChannelsReadTasks();
    TaskTemplatePtr getChannelsRegisterNotifyTasks();
    TaskTemplatePtr getDeactivationTasks();
    void initChannelInfoHash();
    void fillChannelAliasHash();

    QStringList m_configuredChannels;

    TaskContainerSequence m_activationTasks;
    TaskContainerSequence m_deactivationTasks;
    TaskContainerSequence m_reloadRangesTasks;

    AdjustmentModuleCommonPtr m_commonObjects;
};

#endif // ADJUSTMENTMODULEACTIVATOR_H
