#include "scpigroupbase.h"

namespace SCPIMODULE
{

int ScpiGroupBase::m_interfaceCount = 0;

ScpiGroupBase::ScpiGroupBase(cSCPIInterface *iface) :
    m_pSCPIInterface(iface)
{
    m_interfaceCount++;
}

ScpiGroupBase::~ScpiGroupBase()
{
    m_interfaceCount--;
}

int ScpiGroupBase::getInstanceCount()
{
    return m_interfaceCount;
}

}
