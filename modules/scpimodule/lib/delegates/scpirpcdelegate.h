#ifndef SCPIRPCDELEGATE_H
#define SCPIRPCDELEGATE_H

#include "scpibasedelegate.h"
#include "scpimodule.h"

namespace SCPIMODULE {

class cSCPIRpcDelegate : public ScpiBaseDelegate
{
    Q_OBJECT
public:
    cSCPIRpcDelegate(QString cmdParent, QString cmd, quint8 type, cSCPIModule* scpimodule, cSCPICmdInfoPtr scpicmdinfo);
    void executeSCPI(cSCPIClient *client, QString& sInput) override;
private:
    void executeScpiRpc(cSCPIClient *client, QString& sInput);
    QVariant convertParamStrToType(QString parameter, QString type);
    QStringList extractRpcParams(QString RPC);
    QString extractRpcName(QString RPC);

    cSCPIModule* m_pModule;
    cSCPICmdInfoPtr m_scpicmdinfo;
};
}
#endif // SCPIRPCDELEGATE_H
