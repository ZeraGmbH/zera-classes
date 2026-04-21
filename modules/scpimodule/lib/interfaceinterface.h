#ifndef INTERFACEINTERFACE_H
#define INTERFACEINTERFACE_H

#include "baseinterface.h"
#include "scpiclient.h"

namespace SCPIMODULE
{

// the class for our interface interface connections
// means special functions belonging to the interface itself
class cInterfaceInterface : public cBaseInterface
{
    Q_OBJECT
public:
    cInterfaceInterface(cSCPIModule* module, cSCPIInterface* iface);
    virtual bool setupInterface() override;

private slots:
    void executeCmd(SCPIMODULE::cSCPIClient* client, int cmdCode, const QString& sInput);
};

}

#endif // INTERFACEINTERFACE_H
