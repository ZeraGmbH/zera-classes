#ifndef SCPIDELEGATECATALOG_H
#define SCPIDELEGATECATALOG_H

#include "scpidelegatetemplate.h"
#include "scpimodule.h"
#include "scpicmdinfo.h"

namespace SCPIMODULE {

class ScpiDelegateCatalog : public ScpiDelegateTemplate
{
    Q_OBJECT
public:
    ScpiDelegateCatalog(const QString &cmdParent, const QString &cmd,
                        quint8 scpiCmdQueryFlags,
                        cSCPIModule* scpimodule,
                        cSCPICmdInfoPtr scpicmdinfo);
    void executeSCPI(cSCPIClient *client,
                     const QString& scpi,
                     const ScpiTransactionId &scpiTransactionId) override;
    void setOutput(const QVariant &modInterface);

private:
    void setOutput(cSCPICmdInfoPtr scpicmdinfo);
    cSCPIModule* m_pModule;
    cSCPICmdInfoPtr m_pSCPICmdInfo;
    QString m_sAnswer;
};

}

#endif // SCPIDELEGATECATALOG_H

