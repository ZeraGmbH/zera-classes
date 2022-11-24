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
    void setupServerResponseHandlers();
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
    bool openRMConnection();
    bool sendRmIdent();
    void setUpRmIdentHandler();
    bool readResourceTypes();
    void setUpResourceTypeHandler();
    bool readChannels();
    void setUpReadChannelsHandler();
    bool readPortNo(int channelNo);
    void setUpReadPortHandler();
    bool openPcbConnection(int channelNo);
    bool readChannelAlias(int channelNo);
    void setUpReadChannelAliasHandler();
    AdjustmentModuleActivateData &m_activationData;
    cAdjustmentModule* m_module;
    Zera::Proxy::cProxy *m_proxy;
    std::shared_ptr<cBaseModuleConfiguration> m_configuration;
    Zera::Server::cRMInterface m_rmInterface;
    Zera::Proxy::ProxyClientPtr m_rmClient;

    QState m_registerNotifier; // get informed about range changes
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
    int m_loopCountForHandler = 0;
    QList<Zera::Server::cPCBInterface*>::ConstIterator deactivationIt;

    const int CONNECTION_TIMEOUT = 25000;
    const int TRANSACTION_TIMEOUT = 3000;
};

}
#endif // ADJUSTMENTMODULEACTIVATOR_H
