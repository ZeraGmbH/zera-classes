#include "vf_storage.h"

Vf_Storage::Vf_Storage(VeinEvent::StorageSystem *storageSystem, QObject *parent, int entityId):
    QObject(parent),
    m_isInitalized(false)
{
    m_entity=new VfCpp::VfCppEntity(entityId);
    m_dataCollect = new VeinDataCollector(storageSystem);
    connect(m_dataCollect, &VeinDataCollector::newStoredValue, this, &Vf_Storage::updateValue);
}

bool Vf_Storage::initOnce()
{
    if(!m_isInitalized){
        m_isInitalized=true;
        m_entity->initModule();
        m_entity->createComponent("EntityName", "Storage", true);
        m_dataCollect->collectRMSValues();
        m_dataCollect->collectPowerValuesForEmobAC();
        m_dataCollect->collectPowerValuesForEmobDC();
        m_storedValues = m_entity->createComponent("StoredValues", QJsonObject(), true);
    }
    return true;
}

VfCpp::VfCppEntity *Vf_Storage::getVeinEntity() const
{
    return m_entity;
}

void Vf_Storage::setVeinEntity(VfCpp::VfCppEntity *entity)
{
    m_entity = entity;
}

void Vf_Storage::updateValue(QJsonObject value)
{
    m_storedValues->setValue(value);
}
