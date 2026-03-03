#ifndef RPCFLIPSWITCHOFF_H
#define RPCFLIPSWITCHOFF_H

#include "cro_systemobserver.h"
#include <pcbinterface.h>
#include <vf-cpp-rpc-simplified.h>
#include <QUuid>

class RpcFlipSwitchOff : public VfCpp::VfCppRpcSimplified
{
public:
    RpcFlipSwitchOff(Zera::PcbInterfacePtr pcbInterface,
                     ChannelRangeObserver::SystemObserverPtr observer,
                     VeinEvent::EventSystem *eventSystem, int entityId);
private slots:
    void callRPCFunction(const QUuid &callId, const QVariantMap &parameters) override;
    void onRpcTaskFinish(bool ok, QUuid rpcCallId);
private:
    Zera::PcbInterfacePtr m_pcbInterface;
    TaskContainerInterfacePtr m_flipSwitchQueue;
    ChannelRangeObserver::SystemObserverPtr m_observer;
};

#endif // RPCFLIPSWITCHOFF_H
