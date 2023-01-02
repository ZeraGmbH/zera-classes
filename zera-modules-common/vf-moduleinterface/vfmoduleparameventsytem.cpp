#include "vfmoduleparameventsytem.h"

VfModuleParamEventSytem::VfModuleParamEventSytem(int entityId, VeinEvent::StorageSystem *storageSystem) :
    VfCommandFilterEventSystem(VeinEvent::CommandEvent::EventSubtype::TRANSACTION),
    m_entityId(entityId),
    m_storageSystem(storageSystem)
{
}

void VfModuleParamEventSytem::processCommandEvent(VeinEvent::CommandEvent *t_cEvent)
{
    if (t_cEvent->eventData()->entityId() == m_entityId) {
        // is it a command event for setting component data
        if (t_cEvent->eventData()->type() == VeinComponent::ComponentData::dataType()) {
            VeinComponent::ComponentData* cData = static_cast<VeinComponent::ComponentData*> (t_cEvent->eventData());
            QString cName = cData->componentName();
            // does this component data belong to our module
            auto hashIter = m_parameterHash.find(cName);
            if(hashIter != m_parameterHash.end()) {
                // we only take new values if the old values are equal
                if (cData->oldValue() == m_storageSystem->getStoredValue(m_entityId, cName)) {
                    VfModuleParameter *param = hashIter.value();
                    param->transaction(t_cEvent->peerId(), cData->newValue(), cData->oldValue(), cData->eventCommand());
                    t_cEvent->accept(); // it is an event for us ... the parameter will do the rest
                }
            }
        }
    }
}

void VfModuleParamEventSytem::setParameterHash(const QHash<QString, VfModuleParameter *> &parameterHash)
{
    m_parameterHash = parameterHash;
}
