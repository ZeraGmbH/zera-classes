#ifndef RPCREADLOCKSTATE_H
#define RPCREADLOCKSTATE_H

#include "cro_systemobserver.h"
#include <pcbinterface.h>
#include <taskcontainerinterface.h>
#include <vf-cpp-rpc-simplified.h>
#include <QUuid>

class RPCReadLockState : public VfCpp::VfCppRpcSimplified
{
    Q_OBJECT
public:
    RPCReadLockState(Zera::PcbInterfacePtr pcbInterface, ChannelRangeObserver::SystemObserverPtr observer,
                     VeinEvent::EventSystem *eventSystem, int entityId);
private slots:
    void callRPCFunction(const QUuid &callId, const QVariantMap &parameters) override;
    void onRpcTaskFinish(bool ok, QUuid rpcCallId);
private:
    Zera::PcbInterfacePtr m_pcbInterface;
    TaskContainerInterfacePtr m_readLockStateQueue;
    std::shared_ptr<int> m_lockStateReceived;
    ChannelRangeObserver::SystemObserverPtr m_observer;
};

#endif // RPCREADLOCKSTATE_H
