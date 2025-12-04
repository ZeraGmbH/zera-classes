#ifndef RPCREADERROR_H
#define RPCREADERROR_H

#include <taskcontainerinterface.h>
#include "cro_systemobserver.h"
#include <vf-cpp-rpc-simplified.h>
#include <pcbinterface.h>
#include <QUuid>

class RPCReadError : public VfCpp::VfCppRpcSimplified
{
public:
    RPCReadError(Zera::PcbInterfacePtr pcbInterface,
                  ChannelRangeObserver::SystemObserverPtr observer,
                  VeinEvent::EventSystem *eventSystem, int entityId);
private slots:
    void callRPCFunction(const QUuid &callId, const QVariantMap &parameters) override;
    void onRpcTaskFinish(bool ok, QUuid rpcCallId);
private:
    Zera::PcbInterfacePtr m_pcbInterface;
    TaskContainerInterfacePtr m_readErrorQueue;
    std::shared_ptr<int> m_emobErrorStatus;
    ChannelRangeObserver::SystemObserverPtr m_observer;
};

#endif // RPCREADERROR_H
