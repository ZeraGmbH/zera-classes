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
    virtual ~cBaseInterface();
    virtual bool setupInterface() = 0;

    static int getInstanceCount();
protected:
    cSCPIModule* m_pModule;
    cSCPIInterface* m_pSCPIInterface;

private:
    static int m_interfaceCount;
};

}

#endif // BASEINTERFACE_H
