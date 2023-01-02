#include "vfinputcomponentseventsystem.h"
#include "vfmodulecomponent.h"

VfInputComponentsEventSystem::VfInputComponentsEventSystem() :
    VfCommandFilterEventSystem(VeinEvent::CommandEvent::EventSubtype::NOTIFICATION)
{
}

void VfInputComponentsEventSystem::setInputList(QList<VfModuleComponentInput *> &inputComponentList)
{
    m_inputComponentList = inputComponentList;
}

void VfInputComponentsEventSystem::processCommandEvent(VeinEvent::CommandEvent *commandEvent)
{
    // is it a command event for setting component data
    if (commandEvent->eventData()->type() == VeinComponent::ComponentData::dataType()) {
        VeinComponent::ComponentData* componentData = static_cast<VeinComponent::ComponentData*> (commandEvent->eventData());
        QString componentName = componentData->componentName();
        int entityId = componentData->entityId();
        // now let's look if we need this input
        for (int i = 0; i < m_inputComponentList.count(); i++) {
            VfModuleComponentInput *vmci = m_inputComponentList.at(i);
            if ( (vmci->m_sName == componentName) && (vmci->m_nEntityId == entityId) ) {
                vmci->setValue(componentData->newValue());
                emit vmci->sigValueChanged(componentData->newValue());
                // break; // we don't break here -> so a module can use an input several times
            }
        }
    }
}
