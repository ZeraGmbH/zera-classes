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
    struct Params {
        const QString cmdParent;
        const QString cmd;
        quint8 scpiQueryCmdFlags = 0;
        cSCPIModule* scpimodule = nullptr;
        const cSCPICmdInfoPtr scpicmdinfo;
    };
    ScpiDelegateCatalog(const Params &params);
    void executeSCPI(cSCPIClient *client,
                     const QString& scpi,
                     const ScpiTransactionId &scpiTransactionId) override;
    void setOutputFromInfModuleInterface(const QVariant &modInterface);

private:
    void setOutputInitial(const cSCPICmdInfoPtr &scpicmdinfo);
    void setOutputFromComponentInfo(const QJsonObject &componentInfo);

    cSCPIModule* m_pModule;
    cSCPICmdInfoPtr m_pSCPICmdInfo;
    QString m_sAnswer;
};

}

#endif // SCPIDELEGATECATALOG_H

