#include "modulevalidator.h"
#include <vcmp_remoteproceduredata.h>
#include <vcmp_errordata.h>

ModuleValidator::ModuleValidator(int entityId,
                                 VeinEvent::StorageSystem *storageSystem) :
    VeinCommandFilterEventSystem(VeinEvent::CommandEvent::EventSubtype::TRANSACTION),
    m_entityId(entityId),
    m_storageSystem(storageSystem)
{
}

void ModuleValidator::processCommandEvent(VeinEvent::CommandEvent *t_cEvent)
{
    if (t_cEvent->eventData()->entityId() == m_entityId) {
        // is it a command event for setting component data
        if (t_cEvent->eventData()->type() == VeinComponent::ComponentData::dataType()) {
            // only transactions will be handled
            VeinComponent::ComponentData* cData = static_cast<VeinComponent::ComponentData*> (t_cEvent->eventData());
            if (t_cEvent->eventSubtype() == VeinEvent::CommandEvent::EventSubtype::TRANSACTION) {
                QString cName = cData->componentName();
                // does this component data belong to our module
                auto hashIter = m_Parameter2ValidateHash.find(cName);
                if(hashIter != m_Parameter2ValidateHash.end()) {
                    // we only take new values if the old values are equal
                    if (cData->oldValue() == m_storageSystem->getStoredValue(m_entityId, cName)) {
                        cVeinModuleParameter *param = hashIter.value();
                        param->transaction(t_cEvent->peerId(), cData->newValue(), cData->oldValue(), cData->eventCommand());
                        t_cEvent->accept(); // it is an event for us ... the parameter will do the rest
                    }
                }
            }
        }
    }
}

void ModuleValidator::setParameterHash(QHash<QString, cVeinModuleParameter *> &parameterhash)
{
    m_Parameter2ValidateHash = parameterhash;
}
