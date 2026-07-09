#include "scpimodelbase.h"

namespace SCPIMODULE
{

int ScpiModelBase::m_interfaceCount = 0;

ScpiModelBase::ScpiModelBase()
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
