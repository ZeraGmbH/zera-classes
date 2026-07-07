#ifndef SCPIMODELSTATUS_H
#define SCPIMODELSTATUS_H

#include "scpimodelbase.h"
#include "scpistatusdefinitions.h"
#include "scpitransactionid.h"

namespace SCPIMODULE
{
class cSCPIClient;

// the class for our interface interface connections
// means special functions belonging to the interface itself
class ScpiModelStatus : public ScpiModelBase
{
    Q_OBJECT
public:
    ScpiModelStatus(cSCPIInterface* iface);
    bool setupScpi();

private slots:
    void executeCmd(SCPIMODULE::cSCPIClient *client,
                    SCPIStatusDefinitions::ScpiStatusCommands cmdCode,
                    SCPIStatusDefinitions::ScpiStatusSystems statIndex,
                    const QString &scpi,
                    const ScpiTransactionId &scpiTransactionId);

};

}

#endif // SCPIMODELSTATUS_H
