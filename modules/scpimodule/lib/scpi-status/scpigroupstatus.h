#ifndef SCPIGROUPSTATUS_H
#define SCPIGROUPSTATUS_H

#include "scpigroupbase.h"
#include "scpistatusdefinitions.h"
#include "scpitransactionid.h"

namespace SCPIMODULE
{
class cSCPIClient;

// the class for our interface interface connections
// means special functions belonging to the interface itself
class ScpiGroupStatus: public ScpiGroupBase
{
    Q_OBJECT
public:
    ScpiGroupStatus(cSCPIInterface* iface);
    bool setupScpi();

private slots:
    void executeCmd(SCPIMODULE::cSCPIClient *client,
                    SCPIStatusDefinitions::ScpiStatusCommands cmdCode,
                    SCPIStatusDefinitions::ScpiStatusSystems statIndex,
                    const QString &scpi,
                    const ScpiTransactionId &scpiTransactionId);

};

}

#endif // SCPIGROUPSTATUS_H
