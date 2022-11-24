#ifndef ADJUSTMENTMODULEACTIVATOR_H
#define ADJUSTMENTMODULEACTIVATOR_H

#include "adjustmentmodulecommon.h"
#include "adjustmentmodule.h"
#include "moduleactivist.h"
#include "basemoduleconfiguration.h"
#include "rminterface.h"
#include "pcbinterface.h"
#include <QState>
#include <QFinalState>
#include <QStateMachine>
#include <QTimer>
#include <functional>
#include <QList>

namespace ADJUSTMENTMODULE
{

class AdjustmentModuleActivator : public cModuleActivist
{
    Q_OBJECT
public:
    AdjustmentModuleActivator(cAdjustmentModule* module,
                              Zera::Proxy::cProxy* proxy,
                              std::shared_ptr<cBaseModuleConfiguration> pConfiguration,
                              AdjustmentModuleActivateData &activationData);
    void setUpActivationStateMachine();
    void setUpDeactivationStateMachine();
public slots:
    void generateInterface() override {}
    void activate() override;
    void deactivate() override;
signals:
    void sigActivationReady();
    void sigDeactivationReady();

private slots:
    void catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer);

private:
    cAdjustmentModuleConfigData *getConfData();

    AdjustmentModuleActivateData &m_activationData;
    cAdjustmentModule* m_module;
    Zera::Proxy::cProxy *m_proxy;
    std::shared_ptr<cBaseModuleConfiguration> m_configuration;
    Zera::Server::cRMInterface m_rmInterface;
    Zera::Proxy::ProxyClientPtr m_rmClient;

    QState m_rmConnectState; // we must connect first to resource manager
    QState m_IdentifyState; // we must identify ourself at resource manager
    QState m_readResourceTypesState; // we ask for a list of all resources
    QState m_readResourceState; // we look for all our resources needed
    QState m_readResourceInfoState; // we look for resource specification
    QState m_registerNotifier; // get informed about range changes
    QState m_readResourceInfoLoopState;
    QState m_pcbConnectionState; // we try to get a connection to all our pcb servers
    QState m_pcbConnectionLoopState;
    QState m_readChnAliasState; // we query all our channels alias
    QState m_readChnAliasLoopState;
    QState m_readRangelistState; // we query the range list for all our channels
    QState m_readRangelistLoopState;
    QState m_searchActualValuesState;
    QFinalState m_activationDoneState;

    QState m_deactivateState;
    QState m_unregisterState;
    QState m_unregisterLoopState;
    QFinalState m_deactivateDoneState;

    QHash<quint32, int> m_MsgNrCmdList;
    int activationIt = 0;
    QList<Zera::Server::cPCBInterface*>::ConstIterator deactivationIt;
};

}
#endif // ADJUSTMENTMODULEACTIVATOR_H
