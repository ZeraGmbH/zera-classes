#ifndef SCPIPROPERTYDELEGATE
#define SCPIPROPERTYDELEGATE

#include "scpibasedelegate.h"
#include "scpimodule.h"
#include "scpicmdinfo.h"

namespace SCPIMODULE {

class cSCPIPropertyDelegate: public ScpiBaseDelegate
{
    Q_OBJECT
public:
    cSCPIPropertyDelegate(QString cmdParent, QString cmd, quint8 type, cSCPIModule* scpimodule, cSCPICmdInfo* scpicmdinfo);
    virtual ~cSCPIPropertyDelegate();
    virtual void executeSCPI(cSCPIClient *client, QString& sInput) override;
    void setOutput(cSCPICmdInfo* scpicmdinfo);
    void setOutput(QVariant modInterface);
private:
    cSCPIModule* m_pModule;
    cSCPICmdInfo* m_pSCPICmdInfo;
    QString m_sAnswer;
};

}

#endif // SCPIPROPERTYDELEGATE

