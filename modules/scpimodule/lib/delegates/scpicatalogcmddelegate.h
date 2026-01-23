#ifndef SCPICATALOGCMDDELEGATE
#define SCPICATALOGCMDDELEGATE

#include "scpibasedelegate.h"
#include "scpimodule.h"
#include "scpicmdinfo.h"

namespace SCPIMODULE {

class cSCPICatalogCmdDelegate : public ScpiBaseDelegate
{
    Q_OBJECT
public:
    cSCPICatalogCmdDelegate(const QString &cmdParent, const QString &cmd, quint8 type, cSCPIModule* scpimodule, cSCPICmdInfoPtr scpicmdinfo);
    virtual void executeSCPI(cSCPIClient *client, const QString& scpi) override;
    void setOutput(const QVariant &modInterface);
private:
    void setOutput(cSCPICmdInfoPtr scpicmdinfo);
    cSCPIModule* m_pModule;
    cSCPICmdInfoPtr m_pSCPICmdInfo;
    QString m_sAnswer;
};

}

#endif // SCPICATALOGCMDDELEGATE

