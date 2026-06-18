#ifndef STATUSINTERFACE_H
#define STATUSINTERFACE_H

#include "baseinterface.h"
#include "scpitransactionid.h"

namespace SCPIMODULE
{
class cSCPIClient;

// the class for our interface interface connections
// means special functions belonging to the interface itself
class cStatusInterface: public cBaseInterface
{
    Q_OBJECT
public:
    cStatusInterface(cSCPIModule* module, cSCPIInterface* iface);
    bool setupInterface() override;

private slots:
    void executeCmd(SCPIMODULE::cSCPIClient *client, int cmdCode, int statIndex, const QString &sInput, const ScpiTransactionId &scpiTransactionId);

};

}

#endif // STATUSINTERFACE_H
