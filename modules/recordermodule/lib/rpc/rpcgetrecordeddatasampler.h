#ifndef RPCGETRECORDEDDATASAMPLER_H
#define RPCGETRECORDEDDATASAMPLER_H

#include <vs_abstractdatabase.h>
#include <vf-cpp-rpc-simplified.h>
#include <QUuid>

class RPCGetRecordedDataSampler : public VfCpp::VfCppRpcSimplified
{
    Q_OBJECT
public:
    RPCGetRecordedDataSampler(VeinEvent::EventSystem *eventSystem, int entityId, VeinStorage::AbstractDatabase *storage);
private slots:
    void callRPCFunction(const QUuid &callId, const QVariantMap &parameters) override;
private:
    static QString createRpcSignature();
    void sendResult(QUuid callId, bool success, QString errorMsg, QJsonObject values);

    VeinStorage::AbstractDatabase *m_storage;
};

#endif // RPCGETRECORDEDDATASAMPLER_H
