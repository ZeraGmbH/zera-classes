#ifndef INTERFACEINTERFACE_H
#define INTERFACEINTERFACE_H

#include "baseinterface.h"

namespace SCPIMODULE
{

enum scpiinterfacecommands
{
    deviceinterfacecmd
};

class cSCPIClient;

// the class for our interface interface connections
// means special functions belonging to the interface itself
class cInterfaceInterface: public cBaseInterface
{
    Q_OBJECT
public:
    cInterfaceInterface(cSCPIModule* module, cSCPIInterface* iface);
    virtual bool setupInterface();

private slots:
    void executeCmd(cSCPIClient* client, int cmdCode, const QString& sInput);

};

}

#endif // INTERFACEINTERFACE_H
