#include "vfeventsytemmoduleparam.h"

VfEventSytemModuleParam::VfEventSytemModuleParam(int entityId, const VeinStorage::AbstractEventSystem *storageSystem) :
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
            const VeinComponent::ComponentData* cData = static_cast<VeinComponent::ComponentData*> (commandEvent->eventData());
            const QString componentName = cData->componentName();
            // does this component data belong to our module
            auto hashIter = m_parameterHash.constFind(componentName);
            if(hashIter != m_parameterHash.constEnd()) {
                // we only take new values if the old values are equal
                double oldValue = cData->oldValue().toDouble();
                const VeinStorage::AbstractDatabase *storageDb = m_storageSystem->getDb();
                double newValue = storageDb->getStoredValue(m_entityId, componentName).toDouble();
                if (cData->oldValue() == storageDb->getStoredValue(m_entityId, componentName) ||
                        (qIsNaN(oldValue) && qIsNaN(newValue))) {
                    VfModuleParameter *param = hashIter.value();
                    param->veinTransaction(commandEvent->peerId(), cData->newValue(), cData->oldValue(), cData->eventCommand());
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
