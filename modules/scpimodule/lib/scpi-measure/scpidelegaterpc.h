#ifndef SCPIDELEGATERPC_H
#define SCPIDELEGATERPC_H

#include "scpidelegatetemplate.h"
#include "scpimodule.h"

namespace SCPIMODULE {

class ScpiDelegateRpc : public ScpiDelegateTemplate
{
    Q_OBJECT
public:
    struct Params {
        const QString cmdParent;
        const QString cmd;
        const quint8 scpiCmdQueryFlags = 0;
        cSCPIModule* scpimodule = nullptr;
        const cSCPICmdInfoPtr scpicmdinfo;
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
    cSCPICmdInfoPtr m_scpicmdinfo;
};
}
#endif // SCPIDELEGATERPC_H
