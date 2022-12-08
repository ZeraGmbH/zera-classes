#ifndef ADJUSTMENTMODULEACTIVATOR_H
#define ADJUSTMENTMODULEACTIVATOR_H

#include "adjustmentmodulecommon.h"
#include "adjustmentmodule.h"
#include "moduleactivist.h"
#include "basemoduleconfiguration.h"
#include "rminterface.h"
#include "pcbinterface.h"
#include "tasksequence.h"
#include <functional>

namespace ADJUSTMENTMODULE
{

class AdjustmentModuleActivator : public cModuleActivist
{
    Q_OBJECT
public:
    AdjustmentModuleActivator(cAdjustmentModule* module,
                              Zera::Proxy::cProxy* proxy,
                              std::shared_ptr<cBaseModuleConfiguration> pConfiguration,
                              AdjustmentModuleActivateDataPtr activationData);
public slots:
    void generateInterface() override {}
    void activate() override;
    void deactivate() override;
signals:
    void sigActivationReady();
    void sigDeactivationReady();

private slots:
    void catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer);
    void activateContinue(bool ok);
    void deactivateContinue(bool ok);

private:
    cAdjustmentModuleConfigData *getConfData();

    TaskSequence m_activationTasks;

    void openRMConnection();
    bool checkExternalVeinComponents();

    AdjustmentModuleActivateDataPtr m_activationData;
    cAdjustmentModule* m_module;
    Zera::Proxy::cProxy *m_proxy;
    std::shared_ptr<cBaseModuleConfiguration> m_configuration;
    Zera::Server::RMInterfacePtr m_rmInterface;
    Zera::Proxy::ProxyClientPtr m_rmClient;

    QHash<quint32, int> m_MsgNrCmdList;
    int m_currentChannel = 0;

    const int CONNECTION_TIMEOUT = 25000;
    const int TRANSACTION_TIMEOUT = 3000;
};

}
#endif // ADJUSTMENTMODULEACTIVATOR_H
