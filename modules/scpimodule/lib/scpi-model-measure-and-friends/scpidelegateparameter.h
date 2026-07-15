#ifndef SCPIDELEGATEPARAMETER_H
#define SCPIDELEGATEPARAMETER_H

#include "scpidelegatetemplate.h"
#include "scpimodule.h"
#include "scpicmdinfo.h"
#include "scpiveintransactioninfo.h"

namespace SCPIMODULE {

class ScpiDelegateParameter : public ScpiDelegateTemplate
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
    ScpiDelegateParameter(const Params &params);
    void executeSCPI(cSCPIClient *client,
                     const QString& scpi,
                     const ScpiTransactionId &scpiTransactionId) override;
signals:
    void clientinfoSignal(QString, SCPIMODULE::SCPIVeinTransactionInfoPtr);

private:
    bool handleFutureComponent(cSCPIClient *client, bool bQuery, const ScpiTransactionId &scpiTransactionId);

    cSCPIModule* m_pModule;
    cSCPICmdInfoPtr m_pSCPICmdInfo;
};

}
#endif // SCPIDELEGATEPARAMETER_H
