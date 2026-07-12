#ifndef SCPIRPCDELEGATE_H
#define SCPIRPCDELEGATE_H

#include "scpidelegatetemplate.h"
#include "scpimodule.h"

namespace SCPIMODULE {

class cSCPIRpcDelegate : public ScpiDelegateTemplate
{
    Q_OBJECT
public:
    cSCPIRpcDelegate(const QString &cmdParent, const QString &cmd,
                     quint8 scpiCmdQueryFlags,
                     cSCPIModule* scpimodule,
                     cSCPICmdInfoPtr scpicmdinfo);
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
#endif // SCPIRPCDELEGATE_H
