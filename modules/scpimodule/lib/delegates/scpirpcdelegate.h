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

signals:
    void sigClientInfoSignal(QString);
private:
    void executeScpiRpc(cSCPIClient *client, QString& sInput, bool inputIsQuery);
    QVariant convertParamStrToType(QString parameter, QString type);

    cSCPIModule* m_pModule;
    cSCPICmdInfoPtr m_scpicmdinfo;
};
}
#endif // SCPIRPCDELEGATE_H
