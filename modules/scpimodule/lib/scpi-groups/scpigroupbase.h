#ifndef SCPIGROUPBASE_H
#define SCPIGROUPBASE_H

#include "scpiinterface.h"

namespace SCPIMODULE
{

class ScpiGroupBase : public QObject
{
    Q_OBJECT
public:
    ScpiGroupBase(cSCPIInterface* iface);
    virtual ~ScpiGroupBase();

    static int getInstanceCount();
protected:
    cSCPIInterface* m_pSCPIInterface;

private:
    static int m_interfaceCount;
};

}

#endif // SCPIGROUPBASE_H
