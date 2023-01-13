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
    cSCPIParameterDelegate(QString cmdParent, QString cmd, quint8 type, cSCPIModule* scpimodule, cSCPICmdInfo* scpicmdinfo);
    virtual ~cSCPIParameterDelegate();
    bool executeSCPI(cSCPIClient *client, QString& sInput) override;
signals:
    void clientinfoSignal(QString, cSCPIClientInfo*);
private:
    cSCPIModule* m_pModule;
    cSCPICmdInfo* m_pSCPICmdInfo;
};

}
#endif // SCPIPARAMETERDELEGATE_H
