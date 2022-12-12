#ifndef ADJUSTMENTMODULEACTIVATOR_H
#define ADJUSTMENTMODULEACTIVATOR_H

#include "adjustmentmodulecommon.h"
#include "basemoduleconfiguration.h"
#include "rminterface.h"
#include "pcbinterface.h"
#include "tasksequence.h"
#include "taskparallel.h"
#include <veinmoduleerrorcomponent.h>

namespace ADJUSTMENTMODULE
{

class AdjustmentModuleActivator : public QObject
{
    Q_OBJECT
public:
    AdjustmentModuleActivator(std::shared_ptr<cBaseModuleConfiguration> pConfiguration,
                              AdjustmentModuleActivateDataPtr activationData);
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
    cAdjustmentModuleConfigData *getConfData();
    void openRMConnection();
    void openPcbConnection();
    TaskCompositePtr getChannelsReadTasks();

    TaskSequence m_activationTasks;
    TaskSequence m_deactivationTasks;
    TaskSequence m_reloadRangesTasks;

    AdjustmentModuleActivateDataPtr m_activationData;
    std::shared_ptr<cBaseModuleConfiguration> m_configuration;
    Zera::Server::RMInterfacePtr m_rmInterface;
    Zera::Proxy::ProxyClientPtr m_rmClient;

    const int CONNECTION_TIMEOUT = 25000;
    const int TRANSACTION_TIMEOUT = 3000;
};

}
#endif // ADJUSTMENTMODULEACTIVATOR_H
