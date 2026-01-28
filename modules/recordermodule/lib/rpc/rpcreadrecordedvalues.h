#ifndef RPCREADRECORDEDVALUES_H
#define RPCREADRECORDEDVALUES_H

#include <vf-cpp-rpc-simplified.h>
#include <vs_storagerecorder.h>
#include <QUuid>

class RPCReadRecordedValues : public VfCpp::VfCppRpcSimplified
{
    Q_OBJECT
public:
    RPCReadRecordedValues(VeinEvent::EventSystem *eventSystem, VeinStorage::StorageRecordDataPtr recordedStorage,
                          int entityId);
    static QString intToStringWithLeadingDigits(int number); // should go
private slots:
    void callRPCFunction(const QUuid &callId, const QVariantMap &parameters) override;
private:
    static QString createRpcSignature();
    VeinStorage::StorageRecordDataPtr m_recordedStorage;
};

#endif // RPCREADRECORDEDVALUES_H
