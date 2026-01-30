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
                QVariant storedValue = storageDb->getStoredValue(m_entityId, componentName);
                double newValue = storedValue.toDouble();
                if (cData->oldValue() == storedValue || (qIsNaN(oldValue) && qIsNaN(newValue))) {
                    VfModuleParameter *param = hashIter.value();
                    param->veinTransaction(commandEvent->peerId(),
                                           cData->newValue(), cData->oldValue(),
                                           cData->eventCommand());
                    commandEvent->accept(); // it is an event for us ... the parameter will do the rest
                }
                // WTF is the idea ignoring those with improper old values???
                /*else
                    qWarning("Ignore param change due to old different in %i/%s: Old param %s / old storage %s",
                              m_entityId,
                              qPrintable(componentName),
                              qPrintable(cData->oldValue().toString()),
                              qPrintable(storageDb->getStoredValue(m_entityId, componentName).toString()));*/
            }
        }
    }
}

void VfEventSytemModuleParam::setParameterMap(const QMap<QString, VfModuleParameter *> &parameterHash)
{
    m_parameterHash = parameterHash;
}
