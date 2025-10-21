#ifndef SCPIPARAMETERDELEGATE_H
#define SCPIPARAMETERDELEGATE_H

#include "scpibasedelegate.h"
#include "scpimodule.h"
#include "scpicmdinfo.h"
#include "scpiclientinfo.h"

namespace SCPIMODULE {

class cSCPIParameterDelegate: public ScpiBaseDelegate
{
    Q_OBJECT
public:
    cSCPIParameterDelegate(QString cmdParent, QString cmd, quint8 type, cSCPIModule* scpimodule, cSCPICmdInfoPtr scpicmdinfo);
    void executeSCPI(cSCPIClient *client, QString& sInput) override;
signals:
    void clientinfoSignal(QString, SCPIMODULE::SCPIClientInfoPtr);
private:
    void executeScpiRpc(cSCPIClient *client, QString& sInput, cSCPICommand cmd);
    bool isCommandRPC(QString& sInput);
    QVariant convertParamStrToType(QString parameter, QString type);
    QStringList extractRpcParams(QString RPC);
    QString extractRpcName(QString RPC);
    cSCPIModule* m_pModule;
    cSCPICmdInfoPtr m_pSCPICmdInfo;
};

}
#endif // SCPIPARAMETERDELEGATE_H
