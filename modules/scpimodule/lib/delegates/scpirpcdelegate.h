#ifndef SCPIRPCDELEGATE_H
#define SCPIRPCDELEGATE_H

#include "scpibasedelegate.h"
#include "scpimodule.h"

namespace SCPIMODULE {

class cSCPIRpcDelegate : public ScpiBaseDelegate
{
    Q_OBJECT
public:
    cSCPIRpcDelegate(const QString &cmdParent, const QString &cmd, quint8 type, cSCPIModule* scpimodule, cSCPICmdInfoPtr scpicmdinfo);
    void executeSCPI(cSCPIClient *client, const QString& scpi) override;

signals:
    void sigClientInfoSignal(QString);
private:
    void executeScpiRpc(cSCPIClient *client, const QString& scpi, bool inputIsQuery);
    QVariant convertParamStrToType(const QString &parameter, QString type);

    cSCPIModule* m_pModule;
    cSCPICmdInfoPtr m_scpicmdinfo;
};
}
#endif // SCPIRPCDELEGATE_H
