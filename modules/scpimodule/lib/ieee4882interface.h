#ifndef IEEE4882INTERFACE_H
#define IEEE4882INTERFACE_H

#include "baseinterface.h"
#include "scpitransactionid.h"

namespace SCPIMODULE
{

class cSCPIInterface;
class cSCPIInterfaceDelegate;
class cSCPIClient;

// the class for our module interface connections
class cIEEE4882Interface: public cBaseInterface
{
    Q_OBJECT
public:
    cIEEE4882Interface(cSCPIModule* module, cSCPIInterface* iface);
    bool setupInterface() override;

private slots:
    void executeCmd(SCPIMODULE::cSCPIClient* client, int cmdCode, const QString& sInput, const ScpiTransactionId &scpiTransactionId);
};

}

#endif // IEEE4882INTERFACE_H
