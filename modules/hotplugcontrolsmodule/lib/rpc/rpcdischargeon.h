#ifndef RPCDISCHARGEON_H
#define RPCDISCHARGEON_H

#include "cro_systemobserver.h"
#include <pcbinterface.h>
#include <vf-cpp-rpc-simplified.h>
#include <QUuid>

class RpcDischargeOn : public VfCpp::VfCppRpcSimplified
{
public:
    RpcDischargeOn(Zera::PcbInterfacePtr pcbInterface,
                   ChannelRangeObserver::SystemObserverPtr observer,
                   VeinEvent::EventSystem *eventSystem, int entityId);
private slots:
    void callRPCFunction(const QUuid &callId, const QVariantMap &parameters) override;
    void onRpcTaskFinish(bool ok, QUuid rpcCallId);
private:
    Zera::PcbInterfacePtr m_pcbInterface;
    TaskContainerInterfacePtr m_taskQueue;
    ChannelRangeObserver::SystemObserverPtr m_observer;
};

#endif // RPCDISCHARGEON_H
