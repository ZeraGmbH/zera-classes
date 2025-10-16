#ifndef RPCREADLOCKSTATE_H
#define RPCREADLOCKSTATE_H

#include "vfeventsytemmoduleparam.h"
#include <vf-cpp-rpc-simplified.h>

class RPCReadLockState : public VfCpp::VfCppRpcSimplified
{
    Q_OBJECT
public:
    RPCReadLockState(VfEventSytemModuleParam *eventSystem, int entityId);
    void onReadLockStateCompleted(const QUuid &callId, bool success, QVariant errorMsg, QVariant value);
signals:
    void sigReadLockState(const QUuid &callId);
private slots:
    void callRPCFunction(const QUuid &callId, const QVariantMap &parameters) override;
};

#endif // RPCREADLOCKSTATE_H
