#include "veinstoragefilter.h"

VeinStorageFilter::VeinStorageFilter(VeinEvent::StorageSystem* storage, Settings settings) :
    m_storage{storage},
    m_settings(settings)
{
}

VeinStorageFilter::~VeinStorageFilter()
{
    clear();
}

bool VeinStorageFilter::add(int entityId, QString componentName)
{
    VeinEvent::StorageComponentInterfacePtr actualComponent = m_storage->getComponent(entityId, componentName);
    if(!m_filteredEntityComponents.contains(entityId) || !m_filteredEntityComponents[entityId].contains(componentName)) {
        if(actualComponent) {
            m_filteredEntityComponents[entityId].insert(componentName);
            if(m_settings.m_fireCurrentValidOnAddFiter)
                fireActual(entityId, componentName, actualComponent);
            auto conn = connect(actualComponent.get(), &VeinEvent::StorageComponentInterface::sigValueChange, this, [=](QVariant newValue) {
                emit sigComponentValue(entityId, componentName, newValue, actualComponent->getTimestamp());
            });
            m_componentChangeConnections.append(conn);
            return true;
        }
    }
    return false;
}

void VeinStorageFilter::clear()
{
    for(const auto &connection : qAsConst(m_componentChangeConnections))
        disconnect(connection);
    m_componentChangeConnections.clear();
    m_filteredEntityComponents.clear();
}

void VeinStorageFilter::fireActual(int entityId, QString componentName, VeinEvent::StorageComponentInterfacePtr actualComponent)
{
    const QVariant currValue = actualComponent->getValue();
    if(currValue.isValid())
        emit sigComponentValue(entityId, componentName, currValue, actualComponent->getTimestamp());
}
