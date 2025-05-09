#ifndef IEEE4882INTERFACE_H
#define IEEE4882INTERFACE_H

#include "baseinterface.h"

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
    virtual bool setupInterface();

private slots:
    void executeCmd(cSCPIClient* client, int cmdCode, const QString& sInput);
};

}

#endif // IEEE4882INTERFACE_H
