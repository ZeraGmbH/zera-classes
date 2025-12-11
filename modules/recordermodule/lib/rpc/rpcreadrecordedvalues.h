#ifndef RPCREADRECORDEDVALUES_H
#define RPCREADRECORDEDVALUES_H

#include "veindatacollector.h"
#include <vf-cpp-rpc-simplified.h>
#include <QUuid>

class RPCReadRecordedValues : public VfCpp::VfCppRpcSimplified
{
    Q_OBJECT
public:
    RPCReadRecordedValues(VeinEvent::EventSystem *eventSystem, std::shared_ptr<VeinDataCollector> collector,
                          int entityId);
private slots:
    void callRPCFunction(const QUuid &callId, const QVariantMap &parameters) override;
    void onRpcFinish(QUuid callId, bool success, QString errorMsg, QJsonObject values);
private:
    std::shared_ptr<VeinDataCollector> m_collector;
};

#endif // RPCREADRECORDEDVALUES_H
