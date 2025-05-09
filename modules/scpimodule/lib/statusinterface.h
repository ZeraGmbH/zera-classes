#ifndef STATUSINTERFACE_H
#define STATUSINTERFACE_H

#include "baseinterface.h"

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
    virtual bool setupInterface();

private slots:
    void executeCmd(cSCPIClient *client, int cmdCode, int statIndex, const QString &sInput);

};

}

#endif // STATUSINTERFACE_H
