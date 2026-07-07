#ifndef SCPIMODELBASE_H
#define SCPIMODELBASE_H

#include "scpiinterface.h"

namespace SCPIMODULE
{

class ScpiModelBase : public QObject
{
    Q_OBJECT
public:
    ScpiModelBase(cSCPIInterface* iface);
    virtual ~ScpiModelBase();

    static int getInstanceCount();
protected:
    cSCPIInterface* m_pSCPIInterface;

private:
    static int m_interfaceCount;
};

}

#endif // SCPIMODELBASE_H
