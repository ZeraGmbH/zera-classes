#include "scpimodelbase.h"

namespace SCPIMODULE
{

int ScpiModelBase::m_interfaceCount = 0;

ScpiModelBase::ScpiModelBase(cSCPIInterface *iface) :
    m_pSCPIInterface(iface)
{
    m_interfaceCount++;
}

ScpiModelBase::~ScpiModelBase()
{
    m_interfaceCount--;
}

int ScpiModelBase::getInstanceCount()
{
    return m_interfaceCount;
}

}
