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
    cSCPICatalogCmdDelegate(QString cmdParent, QString cmd, quint8 type, cSCPIModule* scpimodule, cSCPICmdInfoPtr scpicmdinfo);
    virtual void executeSCPI(cSCPIClient *client, QString& sInput) override;
    void setOutput(QVariant modInterface);
private:
    void setOutput(cSCPICmdInfoPtr scpicmdinfo);
    cSCPIModule* m_pModule;
    cSCPICmdInfoPtr m_pSCPICmdInfo;
    QString m_sAnswer;
};

}

#endif // SCPICATALOGCMDDELEGATE

