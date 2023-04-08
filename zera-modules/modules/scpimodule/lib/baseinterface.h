#ifndef BASEINTERFACE_H
#define BASEINTERFACE_H

#include <QObject>

namespace SCPIMODULE
{

class cSCPIModule;
class cSCPIInterface;

class cBaseInterface: public QObject
{
    Q_OBJECT

public:
    cBaseInterface(cSCPIModule *module, cSCPIInterface* iface);
    virtual bool setupInterface() = 0;

protected:
    cSCPIModule* m_pModule;
    cSCPIInterface* m_pSCPIInterface;

};

}

#endif // BASEINTERFACE_H
