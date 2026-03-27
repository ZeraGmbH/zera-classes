#ifndef RPCREADPRUEFGROESSENSTATUS_H
#define RPCREADPRUEFGROESSENSTATUS_H

#include <taskcontainerinterface.h>
#include "cro_systemobserver.h"
#include <vf-cpp-rpc-simplified.h>
#include <pcbinterface.h>
#include <QUuid>

class RPCReadPruefgroessenStatus : public VfCpp::VfCppRpcSimplified
{
public:
    // We sail ontop of René's flags 0-15
    static const int ErrorValueReturnedOnNak = (1<<16);

    RPCReadPruefgroessenStatus(Zera::PcbInterfacePtr pcbInterface,
                               ChannelRangeObserver::SystemObserverPtr observer,
                               VeinEvent::EventSystem *eventSystem, int entityId);
private slots:
    void callRPCFunction(const QUuid &callId, const QVariantMap &parameters) override;
    void onRpcTaskFinish(bool ok, QUuid rpcCallId);
private:
    Zera::PcbInterfacePtr m_pcbInterface;
    TaskContainerInterfacePtr m_readErrorQueue;
    std::shared_ptr<int> m_emobPruefgroessenStatus;
    ChannelRangeObserver::SystemObserverPtr m_observer;
};

#endif // RPCREADPRUEFGROESSENSTATUS_H
