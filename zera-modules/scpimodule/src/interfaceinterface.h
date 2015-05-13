#ifndef INTERFACEINTERFACE_H
#define INTERFACEINTERFACE_H

#include <QString>
#include <QList>

#include "baseinterface.h"

namespace SCPIMODULE
{

enum scpiinterfacecommands
{
    deviceinterfacecmd
};


class cSCPIInterfaceDelegate;
class cSCPIClient;

// the class for our interface interface connections
// means special functions belonging to the interface itself

class cInterfaceInterface: public cBaseInterface
{
    Q_OBJECT

public:
    cInterfaceInterface(VeinPeer* peer, cSCPIInterface* iface);
    virtual ~cInterfaceInterface();

    virtual bool setupInterface();

private:
    QList<cSCPIInterfaceDelegate*> m_scpiInterfaceDelegateList; // our delegatelist for parameter cmd's ... needed to clean up

private slots:
    void executeCmd(cSCPIClient* client, int cmdCode, QString& sInput);

};

}

#endif // INTERFACEINTERFACE_H
