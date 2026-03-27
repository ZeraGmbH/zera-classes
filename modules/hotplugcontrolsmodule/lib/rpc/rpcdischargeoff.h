#ifndef RPCDISCHARGEOFF_H
#define RPCDISCHARGEOFF_H

#include "cro_systemobserver.h"
#include <pcbinterface.h>
#include <vf-cpp-rpc-simplified.h>
#include <QUuid>

class RpcDischargeOff : public VfCpp::VfCppRpcSimplified
{
public:
    RpcDischargeOff(Zera::PcbInterfacePtr pcbInterface,
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

#endif // RPCDISCHARGEOFF_H
