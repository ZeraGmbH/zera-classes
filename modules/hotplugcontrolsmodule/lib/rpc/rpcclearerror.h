#ifndef RPCCLEARERROR_H
#define RPCCLEARERROR_H

#include "cro_systemobserver.h"
#include <vf-cpp-rpc-simplified.h>
#include <pcbinterface.h>
#include <QUuid>

class RpcClearError : public VfCpp::VfCppRpcSimplified
{
public:
    RpcClearError(Zera::PcbInterfacePtr pcbInterface,
                        ChannelRangeObserver::SystemObserverPtr observer,
                        VeinEvent::EventSystem *eventSystem, int entityId);
private slots:
    void callRPCFunction(const QUuid &callId, const QVariantMap &parameters) override;
    void onRpcTaskFinish(bool ok, QUuid rpcCallId);
private:
    Zera::PcbInterfacePtr m_pcbInterface;
    TaskContainerInterfacePtr m_clearErrorQueue;
    ChannelRangeObserver::SystemObserverPtr m_observer;
};

#endif // RPCCLEARERROR_H
