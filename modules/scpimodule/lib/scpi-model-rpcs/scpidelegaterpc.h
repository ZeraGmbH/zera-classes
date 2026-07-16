#ifndef SCPIDELEGATERPC_H
#define SCPIDELEGATERPC_H

#include "scpidelegatetemplate.h"
#include "scpimodule.h"
#include <QJsonObject>

namespace SCPIMODULE {

class ScpiDelegateRpc : public ScpiDelegateTemplate
{
    Q_OBJECT
public:
    struct Params {
        const QString cmdParent;
        const QString cmd;
        const quint8 scpiQueryCmdFlags = 0;
        const int entityId = 0;
        const QString rpcSignature;
        const QJsonObject veinComponentInfo;
        cSCPIModule* scpimodule = nullptr;
    };
    ScpiDelegateRpc(const Params& params);
    void executeSCPI(cSCPIClient *client,
                     const QString& scpi,
                     const ScpiTransactionId &scpiTransactionId) override;
signals:
    void sigClientInfoSignal(QString);

private:
    void executeScpiRpc(cSCPIClient *client, const QString& scpi, bool inputIsQuery, const ScpiTransactionId &scpiTransactionId);
    void handleRpcFinish(const QVariantMap &resultData, const SCPIVeinTransactionInfoPtr &transactionInfo, bool inputIsQuery);
    QVariant convertParamStrToType(const QString &parameter, const QString &type);

    cSCPIModule* m_pModule;
    const int m_entityId = 0;
    const QString m_rpcSignature;
    const QJsonObject m_veinComponentInfo;
};
}
#endif // SCPIDELEGATERPC_H
