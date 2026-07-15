#ifndef SCPIMODELPARAMETERS_H
#define SCPIMODELPARAMETERS_H

#include "scpicmdinfo.h"
#include "scpimodelbase.h"
#include "scpiinterface.h"

namespace SCPIMODULE {

class cSCPIModule;

class ScpiModelParameters : public ScpiModelBase
{
    Q_OBJECT
public:
    explicit ScpiModelParameters(cSCPIModule* scpiModule);

    void setupScpi(cSCPIInterface *scpiInterface);

private:
    void addSCPICommand(cSCPIInterface *scpiInterface, const cSCPICmdInfoPtr &scpiCmdInfo);

    cSCPIModule* m_scpiModule = nullptr;
};

}
#endif // SCPIMODELPARAMETERS_H
