#ifndef BASEINTERFACE_H
#define BASEINTERFACE_H

#include <QObject>

class VeinPeer;

namespace SCPIMODULE
{

class cSCPIInterface;

class cBaseInterface: public QObject
{
    Q_OBJECT

public:
    cBaseInterface(VeinPeer* peer, cSCPIInterface* iface);
    virtual bool setupInterface() = 0;

protected:
    VeinPeer* m_pPeer;
    cSCPIInterface* m_pSCPIInterface;

};

}

#endif // BASEINTERFACE_H
