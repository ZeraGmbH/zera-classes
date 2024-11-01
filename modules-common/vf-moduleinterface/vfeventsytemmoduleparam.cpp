#include "vfeventsytemmoduleparam.h"

VfEventSytemModuleParam::VfEventSytemModuleParam(int entityId, VeinStorage::AbstractEventSystem *storageSystem) :
    VfEventSystemCommandFilter(VeinEvent::CommandEvent::EventSubtype::TRANSACTION),
    m_entityId(entityId),
    m_storageSystem(storageSystem)
{
}

void VfEventSytemModuleParam::processCommandEvent(VeinEvent::CommandEvent *commandEvent)
{
    if (commandEvent->eventData()->entityId() == m_entityId) {
        // is it a command event for setting component data
        if (commandEvent->eventData()->type() == VeinComponent::ComponentData::dataType()) {
            VeinComponent::ComponentData* cData = static_cast<VeinComponent::ComponentData*> (commandEvent->eventData());
            QString cName = cData->componentName();
            // does this component data belong to our module
            auto hashIter = m_parameterHash.find(cName);
            if(hashIter != m_parameterHash.end()) {
                // we only take new values if the old values are equal
                double oldValue = cData->oldValue().toDouble();
                double newValue = m_storageSystem->getStoredValue(m_entityId, cName).toDouble();
                if (cData->oldValue() == m_storageSystem->getStoredValue(m_entityId, cName) ||
                        (qIsNaN(oldValue) && qIsNaN(newValue))) {
                    VfModuleParameter *param = hashIter.value();
                    param->transaction(commandEvent->peerId(), cData->newValue(), cData->oldValue(), cData->eventCommand());
                    commandEvent->accept(); // it is an event for us ... the parameter will do the rest
                }
            }
        }
    }
}

void VfEventSytemModuleParam::setParameterMap(const QMap<QString, VfModuleParameter *> &parameterHash)
{
    m_parameterHash = parameterHash;
}
