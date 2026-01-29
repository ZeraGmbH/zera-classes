#include "abstractmetascpicomponent.h"

int AbstractMetaScpiComponent::m_instanceCount = 0;

AbstractMetaScpiComponent::AbstractMetaScpiComponent()
{
    m_instanceCount++;
}

AbstractMetaScpiComponent::~AbstractMetaScpiComponent()
{
    m_instanceCount--;
}

int AbstractMetaScpiComponent::getInstanceCount()
{
    return m_instanceCount;
}
