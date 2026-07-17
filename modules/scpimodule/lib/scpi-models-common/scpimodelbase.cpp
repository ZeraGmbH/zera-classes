#include "scpimodelbase.h"

namespace SCPIMODULE
{

int ScpiModelBase::m_instanceCount = 0;

ScpiModelBase::ScpiModelBase()
{
    m_instanceCount++;
}

ScpiModelBase::~ScpiModelBase()
{
    m_instanceCount--;
}

int ScpiModelBase::getInstanceCount()
{
    return m_instanceCount;
}

}
