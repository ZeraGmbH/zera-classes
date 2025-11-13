#ifndef RPCACTIVATEPUSHBUTTON_H
#define RPCACTIVATEPUSHBUTTON_H

#include "cro_systemobserver.h"
#include <pcbinterface.h>
#include <taskcontainerinterface.h>
#include <vf-cpp-rpc-simplified.h>
#include <QObject>

class RpcActivatePushButton : public VfCpp::VfCppRpcSimplified
{
    Q_OBJECT
public:
    RpcActivatePushButton(Zera::PcbInterfacePtr pcbInterface,
                          ChannelRangeObserver::SystemObserverPtr observer,
                          VeinEvent::EventSystem *eventSystem,
                          int entityId);
private slots:
    void callRPCFunction(const QUuid &callId, const QVariantMap &parameters) override;
    void onRpcTaskFinish(bool ok, QUuid rpcCallId);
private:
    Zera::PcbInterfacePtr m_pcbInterface;
    ChannelRangeObserver::SystemObserverPtr m_observer;
    TaskContainerInterfacePtr m_activatePushButtonQueue;
};

#endif // RPCACTIVATEPUSHBUTTON_H
