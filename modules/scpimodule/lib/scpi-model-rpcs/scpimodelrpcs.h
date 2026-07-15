#ifndef SCPIMODELRPCS_H
#define SCPIMODELRPCS_H

#include "scpimodelbase.h"
#include "scpicmdinfo.h"
#include "scpiinterface.h"

namespace SCPIMODULE {

class cSCPIModule;

class ScpiModelRpcs : public ScpiModelBase
{
    Q_OBJECT
public:
    ScpiModelRpcs(cSCPIModule* scpiModule);
    void setupScpi(cSCPIInterface *scpiInterface);

private:
    void addRPCCommand(cSCPIInterface *scpiInterface, const cSCPICmdInfoPtr &scpiCmdInfo);

    cSCPIModule* m_scpiModule = nullptr;
};

}

#endif // SCPIMODELRPCS_H
