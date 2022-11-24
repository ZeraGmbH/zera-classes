#ifndef ADJUSTMENTMODULEACTIVATOR_H
#define ADJUSTMENTMODULEACTIVATOR_H

#include "adjustmentmodulecommon.h"
#include "adjustmentmodule.h"
#include "moduleactivist.h"
#include "basemoduleconfiguration.h"
#include "rminterface.h"
#include "pcbinterface.h"
#include <QTimer>
#include <QList>
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
                              AdjustmentModuleActivateData &activationData);
    void setupServerResponseHandlers();
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
    bool readIpPortNo(QString channelName);
    void setUpReadIpPortHandler();
    bool openPcbConnection(QString channelName);
    bool readChannelAlias(QString channelName);
    void setUpReadChannelAliasHandler();
    bool regNotifier(QString channelName);
    void setUpRegisterNotifierHandler();
    bool readRangeList(QString channelName);
    void setUpRangeListHandler();
    bool checkExternalVeinComponents();

    bool unregNotifier(int pcbInterfaceNo);
    void setUpUnregisterNotifierHandler();

    AdjustmentModuleActivateData &m_activationData;
    cAdjustmentModule* m_module;
    Zera::Proxy::cProxy *m_proxy;
    std::shared_ptr<cBaseModuleConfiguration> m_configuration;
    Zera::Server::cRMInterface m_rmInterface;
    Zera::Proxy::ProxyClientPtr m_rmClient;

    QHash<quint32, int> m_MsgNrCmdList;
    int m_currentChannel = 0;

    const int CONNECTION_TIMEOUT = 25000;
    const int TRANSACTION_TIMEOUT = 3000;
};

}
#endif // ADJUSTMENTMODULEACTIVATOR_H
