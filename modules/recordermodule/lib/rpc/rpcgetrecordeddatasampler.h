#ifndef RPCGETRECORDEDDATASAMPLER_H
#define RPCGETRECORDEDDATASAMPLER_H

#include <vf_cmd_event_handler_system.h>
#include <vs_abstractdatabase.h>
#include <vf-cpp-rpc-simplified.h>
#include <storagerecorderinterpolation.h>
#include <QUuid>

class RPCGetRecordedDataSampler : public VfCpp::VfCppRpcSimplified
{
    Q_OBJECT
public:
    RPCGetRecordedDataSampler(VeinEvent::EventSystem *eventSystem, StorageRecordDataPtr recorder,int entityId);
private slots:
    void callRPCFunction(const QUuid &callId, const QVariantMap &parameters) override;
private:
    static QString createRpcSignature();
    static QString intToStringWithLeadingDigits(int number);

    StorageRecordDataPtr m_recordedData;
};

#endif // RPCGETRECORDEDDATASAMPLER_H
