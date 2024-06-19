#include "vf_storage.h"

Vf_Storage::Vf_Storage(QObject *parent, int entityId):
    QObject(parent),
    m_isInitalized(false)
{
    m_entity=new VfCpp::VfCppEntity(entityId);
}

bool Vf_Storage::initOnce()
{
    if(!m_isInitalized){
        m_isInitalized=true;
        m_entity->initModule();
        m_entity->createComponent("EntityName", "Storage", true);
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
