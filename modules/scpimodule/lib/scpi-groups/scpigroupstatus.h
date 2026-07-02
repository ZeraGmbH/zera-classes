#ifndef SCPIGROUPSTATUS_H
#define SCPIGROUPSTATUS_H

#include "scpigroupbase.h"
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
    void executeCmd(SCPIMODULE::cSCPIClient *client, int cmdCode, int statIndex, const QString &sInput, const ScpiTransactionId &scpiTransactionId);

};

}

#endif // SCPIGROUPSTATUS_H
